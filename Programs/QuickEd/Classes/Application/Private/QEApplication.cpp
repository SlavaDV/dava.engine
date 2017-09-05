#include "Application/QEApplication.h"
#include "Application/QEGlobal.h"

#include "Modules/UpdateViewsSystemModule/UpdateViewsSystemModule.h"
#include "Modules/LegacySupportModule/LegacySupportModule.h"
#include "Modules/DocumentsModule/DocumentsModule.h"
#include "Modules/ProjectModule/ProjectModule.h"
#include "Classes/Application/ReflectionExtensions.h"

#include <QtTools/InitQtTools.h>

#include <TArc/Core/Core.h>
#include <TArc/Utils/ModuleCollection.h>
#include <TArc/SharedModules/SettingsModule/SettingsModule.h>
#include <TArc/SharedModules/ThemesModule/ThemesModule.h>
#include <TArc/SharedModules/ActionManagementModule/ActionManagementModule.h>
#include <TArc/WindowSubSystem/ActionUtils.h>

#include <DocDirSetup/DocDirSetup.h>

#include <Render/Renderer.h>
#include <Tools/TextureCompression/PVRConverter.h>
#include <Particles/ParticleEmitter.h>

#include <UI/UIControlSystem.h>
#include <UI/Input/UIInputSystem.h>
#include <UI/Layouts/UILayoutSystem.h>
#include <UI/RichContent/UIRichContentSystem.h>
#include <UI/Scroll/UIScrollBarLinkSystem.h>

#include <FileSystem/FileSystem.h>

#include <QFileInfo>
#include <QCryptographicHash>
#include <QDir>

namespace QEApplicationDetail
{
DAVA::KeyedArchive* CreateOptions()
{
    DAVA::KeyedArchive* appOptions = new DAVA::KeyedArchive();

    appOptions->SetInt32("bpp", 32);
    appOptions->SetInt32("rhi_threaded_frame_count", 1);
    appOptions->SetInt32("renderer", rhi::RHI_GLES2);
    appOptions->SetBool("trackFont", true);
    return appOptions;
}
}

QEApplication::QEApplication(DAVA::Vector<DAVA::String>&& cmdLine_)
    : cmdLine(std::move(cmdLine_))
{
}

DAVA::TArc::BaseApplication::EngineInitInfo QEApplication::GetInitInfo() const
{
    EngineInitInfo initInfo;
    initInfo.runMode = DAVA::eEngineRunMode::GUI_EMBEDDED;
    initInfo.modules = {
        "JobManager",
        "NetCore",
        "LocalizationSystem",
        "SoundSystem",
        "DownloadManager",
    };
    initInfo.options.Set(QEApplicationDetail::CreateOptions());
    return initInfo;
}

void QEApplication::Init(const DAVA::EngineContext* engineContext)
{
    using namespace DAVA;
#if defined(__DAVAENGINE_MACOS__)
    const String pvrTexToolPath = "~res:/PVRTexToolCLI";
#elif defined(__DAVAENGINE_WIN32__)
    const String pvrTexToolPath = "~res:/PVRTexToolCLI.exe";
#endif
    PVRConverter::Instance()->SetPVRTexTool(pvrTexToolPath);

    Texture::SetPixelization(true);

    FileSystem* fs = engineContext->fileSystem;

    auto copyFromOldFolder = [&]
    {
        FileSystem::eCreateDirectoryResult createResult = DocumentsDirectorySetup::CreateApplicationDocDirectory(fs, "QuickEd");
        if (createResult != DAVA::FileSystem::DIRECTORY_EXISTS)
        {
            DAVA::FilePath documentsOldFolder = fs->GetUserDocumentsPath() + "QuickEd/";
            DAVA::FilePath documentsNewFolder = DocumentsDirectorySetup::GetApplicationDocDirectory(fs, "QuickEd");
            engineContext->fileSystem->RecursiveCopy(documentsOldFolder, documentsNewFolder);
        }
    };
    copyFromOldFolder(); // todo: remove function some versions after
    DocumentsDirectorySetup::SetApplicationDocDirectory(fs, "QuickEd");

    engineContext->logger->SetLogFilename("QuickEd.txt");

    ParticleEmitter::FORCE_DEEP_CLONE = true;

    UIControlSystem* uiControlSystem = engineContext->uiControlSystem;
    uiControlSystem->GetLayoutSystem()->SetAutoupdatesEnabled(true);
    uiControlSystem->GetSystem<UIScrollBarLinkSystem>()->SetRestoreLinks(true);
    uiControlSystem->GetSystem<UIRichContentSystem>()->SetEditorMode(true);

    UIInputSystem* inputSystem = uiControlSystem->GetInputSystem();
    inputSystem->BindGlobalShortcut(KeyboardShortcut(eInputElements::KB_LEFT), UIInputSystem::ACTION_FOCUS_LEFT);
    inputSystem->BindGlobalShortcut(KeyboardShortcut(eInputElements::KB_RIGHT), UIInputSystem::ACTION_FOCUS_RIGHT);
    inputSystem->BindGlobalShortcut(KeyboardShortcut(eInputElements::KB_UP), UIInputSystem::ACTION_FOCUS_UP);
    inputSystem->BindGlobalShortcut(KeyboardShortcut(eInputElements::KB_DOWN), UIInputSystem::ACTION_FOCUS_DOWN);

    inputSystem->BindGlobalShortcut(KeyboardShortcut(eInputElements::KB_TAB), UIInputSystem::ACTION_FOCUS_NEXT);
    inputSystem->BindGlobalShortcut(KeyboardShortcut(eInputElements::KB_TAB, eModifierKeys::SHIFT), UIInputSystem::ACTION_FOCUS_PREV);

    engineContext->logger->Log(Logger::LEVEL_INFO, QString("Qt version: %1").arg(QT_VERSION_STR).toStdString().c_str());

    BaseApplication::Init(engineContext);
}

void QEApplication::Cleanup()
{
    cmdLine.clear();
}

bool QEApplication::AllowMultipleInstances() const
{
    return true;
}

QString QEApplication::GetInstanceKey() const
{
    DAVA::String appPath = cmdLine.front();
    QFileInfo appFileInfo(QString::fromStdString(appPath));

    const QString appUid = "{BCDF3F30-2706-4E94-8F9E-4C21EB567334}";
    const QString appUidPath = QCryptographicHash::hash((appUid + appFileInfo.absoluteDir().absolutePath()).toUtf8(), QCryptographicHash::Sha1).toHex();
    return appUidPath;
}

void QEApplication::CreateModules(DAVA::TArc::Core* tarcCore) const
{
    using namespace DAVA::TArc;
    InitColorPickerOptions(true);
    InitQtTools();

    tarcCore->CreateModule<SettingsModule>();
    tarcCore->CreateModule<ThemesModule>();
    tarcCore->CreateModule<ActionManagementModule>();
    tarcCore->CreateModule<LegacySupportModule>();
    tarcCore->CreateModule<UpdateViewsSystemModule>();
    tarcCore->CreateModule<ProjectModule>();
    tarcCore->CreateModule<DocumentsModule>();

    for (const DAVA::ReflectedType* type : ModuleCollection::Instance()->GetGuiModules())
    {
        tarcCore->CreateModule(type);
    }
}

void QEApplication::RegisterReflectionExtensions()
{
    ::RegisterReflectionExtensions();
}
