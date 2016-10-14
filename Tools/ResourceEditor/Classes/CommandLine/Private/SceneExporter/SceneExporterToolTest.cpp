#include "CommandLine/SceneExporterTool.h"
#include "CommandLine/Private/REConsoleModuleTestUtils.h"

#include "Base/BaseTypes.h"
#include "Testing/TArcUnitTests.h"

#include <functional>

namespace SETestDetail
{
const DAVA::String projectStr = "~doc:/Test/SceneExporterTool/";
const DAVA::String scenePathnameStr = projectStr + "DataSource/3d/Scene/testScene.sc2";
}

DAVA_TARC_TESTCLASS(SceneExporterToolTest)
{
    void TestExportedTextures(const DAVA::FilePath& folder)
    {
        using namespace DAVA;

        ScopedPtr<FileList> fileList(new FileList(folder));
        for (int32 i = 0, count = fileList->GetCount(); i < count; ++i)
        {
            const FilePath& pathname = fileList->GetPathname(i);
            if (fileList->IsDirectory(i) && !fileList->IsNavigationDirectory(i))
            {
                TestExportedTextures(pathname);
            }
            else if (pathname.IsEqualToExtension(".tex"))
            {
                std::unique_ptr<TextureDescriptor> descriptor(TextureDescriptor::CreateFromFile(pathname));
                if (descriptor)
                {
                    for (eGPUFamily gpu : { eGPUFamily::GPU_MALI, eGPUFamily::GPU_ADRENO })
                    {
                        Vector<FilePath> pathes;
                        descriptor->CreateLoadPathnamesForGPU(gpu, pathes);

                        TEST_VERIFY(pathes.size() == 2);
                        for (const FilePath& path : pathes)
                        {
                            TEST_VERIFY(FileSystem::Instance()->Exists(path));
                        }
                    }
                }
                else
                {
                    TEST_VERIFY(false);
                }
            }
        }
    }

    DAVA::FilePath FindTexturePathname(const DAVA::FilePath& folder)
    {
        using namespace DAVA;

        ScopedPtr<FileList> fileList(new FileList(folder));
        for (int32 i = 0, count = fileList->GetCount(); i < count; ++i)
        {
            FilePath pathname = fileList->GetPathname(i);
            if (fileList->IsDirectory(i) && !fileList->IsNavigationDirectory(i))
            {
                pathname = FindTexturePathname(pathname);
            }

            if (pathname.IsEqualToExtension(".tex"))
            {
                return pathname;
            }
        }

        return FilePath();
    }

    DAVA_TEST (ExportSceneTest)
    {
        using namespace DAVA;

        REConsoleModuleTestUtils::CreateProjectInfrastructure(SETestDetail::projectStr);
        REConsoleModuleTestUtils::CreateScene(SETestDetail::scenePathnameStr);

        FilePath dataPath = SETestDetail::projectStr + "Data/3d/";
        FilePath dataSourcePath = SETestDetail::projectStr + "DataSource/3d/";

        String sceneRelativePathname = FilePath(SETestDetail::scenePathnameStr).GetRelativePathname(dataSourcePath);

        {
            Vector<String> cmdLine =
            {
              "ResourceEditor",
              "-sceneexporter",
              "-indir",
              dataPath.GetAbsolutePathname(),
              "-outdir",
              dataSourcePath.GetAbsolutePathname(),
              "-processfile",
              sceneRelativePathname,
              "-gpu",
              "mali,adreno",
              "-hd"
            };

            std::unique_ptr<REConsoleModuleCommon> tool = std::make_unique<SceneExporterTool>(cmdLine);
            REConsoleModuleTestUtils::ExecuteModule(tool.get());

            TEST_VERIFY(FileSystem::Instance()->Exists(dataPath + sceneRelativePathname));
            TestExportedTextures(dataPath);

            REConsoleModuleTestUtils::ClearTestFolder(dataPath);
        }

        {
            Vector<String> cmdLine =
            {
              "ResourceEditor",
              "-sceneexporter",
              "-scene",
              "-indir",
              FilePath(dataPath).GetAbsolutePathname(),
              "-outdir",
              FilePath(dataSourcePath).GetAbsolutePathname(),
              "-processfolder",
              "./",
              "-gpu",
              "mali,adreno",
              "-hd"
            };

            std::unique_ptr<REConsoleModuleCommon> tool = std::make_unique<SceneExporterTool>(cmdLine);
            REConsoleModuleTestUtils::ExecuteModule(tool.get());

            TEST_VERIFY(FileSystem::Instance()->Exists(dataPath + sceneRelativePathname));
            TestExportedTextures(dataPath);

            REConsoleModuleTestUtils::ClearTestFolder(dataPath);
        }

        REConsoleModuleTestUtils::ClearTestFolder(SETestDetail::projectStr);
    }

    DAVA_TEST (ExportTextureTest)
    {
        using namespace DAVA;

        REConsoleModuleTestUtils::CreateProjectInfrastructure(SETestDetail::projectStr);
        REConsoleModuleTestUtils::CreateScene(SETestDetail::scenePathnameStr);

        FilePath dataPath = SETestDetail::projectStr + "Data/3d";
        FilePath dataSourcePath = SETestDetail::projectStr + "DataSource/3d/";

        {
            FilePath texturePathname = FindTexturePathname(dataSourcePath);
            String textureRelativePathname = texturePathname.GetRelativePathname(dataSourcePath);

            Vector<String> cmdLine =
            {
              "ResourceEditor",
              "-sceneexporter",
              "-indir",
              dataPath.GetAbsolutePathname(),
              "-outdir",
              dataSourcePath.GetAbsolutePathname(),
              "-processfile",
              textureRelativePathname,
              "-gpu",
              "mali,adreno",
              "-hd"
            };

            std::unique_ptr<REConsoleModuleCommon> tool = std::make_unique<SceneExporterTool>(cmdLine);
            REConsoleModuleTestUtils::ExecuteModule(tool.get());

            TEST_VERIFY(FileSystem::Instance()->Exists(dataPath + textureRelativePathname));
            TestExportedTextures(dataPath);

            REConsoleModuleTestUtils::ClearTestFolder(dataPath);
        }

        {
            Vector<String> cmdLine =
            {
              "ResourceEditor",
              "-sceneexporter",
              "-texture",
              "-indir",
              FilePath(dataPath).GetAbsolutePathname(),
              "-outdir",
              FilePath(dataSourcePath).GetAbsolutePathname(),
              "-processfolder",
              "./",
              "-gpu",
              "mali,adreno",
              "-hd"
            };

            std::unique_ptr<REConsoleModuleCommon> tool = std::make_unique<SceneExporterTool>(cmdLine);
            REConsoleModuleTestUtils::ExecuteModule(tool.get());

            TestExportedTextures(dataPath);

            REConsoleModuleTestUtils::ClearTestFolder(dataPath);
        }

        REConsoleModuleTestUtils::ClearTestFolder(SETestDetail::projectStr);
    }

    DAVA_TEST (ExportFileListTest)
    {
        using namespace DAVA;

        REConsoleModuleTestUtils::CreateProjectInfrastructure(SETestDetail::projectStr);
        REConsoleModuleTestUtils::CreateScene(SETestDetail::scenePathnameStr);

        FilePath dataPath = SETestDetail::projectStr + "Data/3d";
        FilePath dataSourcePath = SETestDetail::projectStr + "DataSource/3d/";
        FilePath texturePathname = FindTexturePathname(dataSourcePath);

        String sceneRelativePathname = FilePath(SETestDetail::scenePathnameStr).GetRelativePathname(dataSourcePath);
        String textureRelativePathname = texturePathname.GetRelativePathname(dataSourcePath);

        FilePath linksFilePathname = SETestDetail::projectStr + "links.txt";
        {
            ScopedPtr<File> linksFile(File::Create(linksFilePathname, File::WRITE | File::CREATE));
            if (linksFile)
            {
                linksFile->WriteLine(sceneRelativePathname);
                linksFile->WriteLine(textureRelativePathname);
            }
            else
            {
                TEST_VERIFY(false);
            }
        }

        Vector<String> cmdLine =
        {
          "ResourceEditor",
          "-sceneexporter",
          "-indir",
          dataPath.GetAbsolutePathname(),
          "-outdir",
          dataSourcePath.GetAbsolutePathname(),
          "-processfilelist",
          linksFilePathname.GetAbsolutePathname(),
          "-gpu",
          "mali,adreno",
          "-hd"
        };

        std::unique_ptr<REConsoleModuleCommon> tool = std::make_unique<SceneExporterTool>(cmdLine);
        REConsoleModuleTestUtils::ExecuteModule(tool.get());

        TEST_VERIFY(FileSystem::Instance()->Exists(dataPath + sceneRelativePathname));
        TEST_VERIFY(FileSystem::Instance()->Exists(dataPath + textureRelativePathname));
        TestExportedTextures(dataPath);

        REConsoleModuleTestUtils::ClearTestFolder(SETestDetail::projectStr);
    }

    BEGIN_FILES_COVERED_BY_TESTS()
    FIND_FILES_IN_TARGET(TArc)
    DECLARE_COVERED_FILES("SceneExporterTool.cpp")
    END_FILES_COVERED_BY_TESTS();
}
;
