#sys
from json import loads
from datetime import datetime
from threading import Thread

#local
from dp_api import options

from utils import package, args_parser

from user_lvl_api import app
from user_lvl_api.app_monitor import AppMonitor
from user_lvl_api.etw_session import ETWSession

class Tags:
    full_name = "PackageFullName"
    family_name = "PackageFamilyName"
    version = "Version"


class LogParser:
    def __init__(self, levels = [], providers = [], show_timestamp = True):
        self.levels = levels
        self.providers = providers
        self.show_timestamp = show_timestamp
    
    def __call__(self, msg):
        if msg is None:
            return
        events = loads(msg).get("Events")
        if events is not None:
            for event in events:
                level = event["Level"]
                provider = event["ProviderName"]
                if level in self.levels and provider in self.providers:
                    if self.show_timestamp:
                        # Convert webkit timestamp to unix timestamp
                        unix_timestamp = event["Timestamp"] / 1e7 - 11644473600
                        sys.stdout.write(datetime.fromtimestamp(unix_timestamp).\
                                         strftime("%Y-%m-%d %H:%M:%S.%f "))
                    string_message = event["StringMessage"]
                    if string_message.endswith("\n"):
                        string_message = string_message[:-1]
                    print string_message


def parse_version(v):
    return [v["Major"], v["Minor"], v["Build"], v["Revision"]]


def post_check(condition, msg):
    if condition:
        print msg + "Succeeded."
        return True
    else:
        print msg + "Failed."
        return False

def deploy(cli_args):
    app_package = package.Package(cli_args.app_path)
    deps = []
    for dep_path in cli_args.deps_paths:
        deps.append(package.Package(dep_path))
    
    if cli_args.force:
        if not app.uninstall(Tags.family_name, app_package.identity_name):
            print "Failed to uninstall the app, deploy will not start."
            return False
        
    package_info = app.get_installed_package_info(Tags.family_name, \
                                                  app_package.identity_name)

    if package_info is not None:
        installed_version = parse_version(package_info[Tags.version])
        if installed_version >= app_package.version:
            print "Installed version: " + str(installed_version)
            print "Deploying version: " + str(app_package.version)
            print "Deploy will have no effect. Uninstall current" \
                  "version first or use '-force' flag."
            return False

    fine = app.deploy(app_package, deps, cli_args.cer_path)
    # Post-check
    status_msg = "\rChecking 'deploy' result: "
    print status_msg + "...",
    package_info = app.get_installed_package_info(Tags.family_name, \
                                                  app_package.identity_name)
    installed_version = parse_version(package_info[Tags.version])
    return post_check(fine and package_info is not None and \
                      installed_version == app_package.version,\
                      status_msg)


def uninstall(cli_args):
    fine = app.uninstall(Tags.full_name, cli_args.package_full_name)
    # Post-check
    status_msg = "\rChecking 'uninstall' result: "
    print status_msg + "...",
    return post_check(fine and \
           not app.is_installed(Tags.full_name, cli_args.package_full_name), \
           status_msg)


def run(cli_args):
    if not stop(cli_args):
        print "Failed to stop the app, app will not be runned."
        return False
    attach(cli_args, app.start, Tags.full_name, cli_args.package_full_name)


def stop(cli_args):
    fine = app.stop(Tags.full_name, cli_args.package_full_name)
    # Post-check
    status_msg = "\rChecking 'stop' result: "
    print status_msg + "...",
    return post_check(fine and \
            not app.is_running(Tags.full_name, cli_args.package_full_name), \
            status_msg)


def attach(cli_args, callback = None, *args):

    def deadline_callback(etw_session, app_monitor):
        if etw_session is not None:
            etw_session.session_ws.close()
        if app_monitor is not None:
            app_monitor.session_ws.close()

    log_parser = logger.LogParser(cli_args.log_levels, cli_args.channels, \
                                  not cli_args.no_timestamp)
    etw_session = None
    app_monitor = None

    try:
        app_monitor = AppMonitor(etw_session, \
                                 cli_args.package_full_name, \
                                 deadline_callback, \
                                 cli_args.wait_time, \
                                 cli_args.stop_on_close)

        etw_session = ETWSession(app_monitor, \
                                 cli_args.guids, \
                                 log_parser, \
                                 callback, \
                                 *args)

        if cli_args.package_full_name is not None:
            if not app.is_installed(Tags.full_name, cli_args.package_full_name):
                print "App is not installed. Install the app or use attach " \
                       "without app monitor."
                return
            # Will be terminated by timeout or from etw_session
            Thread(target = app_monitor.start).start()

        etw_session.start()
    except:
        deadline_callback(etw_session, app_monitor)
        raise

def list(cli_args):
    if cli_args.what == "installed":
        return app.list_installed()
    if cli_args.what == "running":
        return app.list_running()
    print "Only 'installed' and 'running' options are available."
    return False

        
def error(cli_args = None):
    print "Args parsing error."


def parse_and_do(cli_args):
    return {
        "deploy" : deploy,
        "uninstall" : uninstall,
        "run" : run,
        "attach" : attach,
        "stop" : stop,
        "list": list
    }.get(cli_args.s_name, error)(cli_args)
        

def main():
    cli_args = args_parser.parser.parse_args()

    options.set_url(cli_args.url)
    options.set_timeout(cli_args.timeout)

    parse_and_do(cli_args)


if __name__ == "__main__":
    main()