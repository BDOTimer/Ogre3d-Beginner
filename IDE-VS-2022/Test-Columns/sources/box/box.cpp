///----------------------------------------------------------------------------|
/// box.cpp
///----------------------------------------------------------------------------:
#include "../config-game.h"
#include "../inspector.h"

#pragma warning(push, 0)
#pragma warning(disable: 4275 4251 4305)
///-------------------------------------------|
/// Свой путь к "ogre.h"                      |
///-------------------------------------------:
#include "OgreConfigPaths.h"
#pragma warning(pop)

///---------|
/// Models. |
///---------:
namespace mdl
{  
        ///-------------------------------------------|
        /// Свой путь к "ogre.h"                      |
        ///-------------------------------------------:
        void InspectorRoot::createRoot()
        {
        /// std::cout << "RUN: createRoot() override\n\n";

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
            mRoot = OGRE_NEW Ogre::Root("");
#else
            Ogre::String pluginsPath;
    #ifndef OGRE_BITES_STATIC_PLUGINS
            pluginsPath = mFSLayer->getConfigFilePath("plugins.cfg");

        if (!Ogre::FileSystemLayer::fileExists(pluginsPath))
        {
            pluginsPath = Ogre::FileSystemLayer::resolveBundlePath(
                OGRE_CONFIG_DIR "/plugins.cfg"
            );
        }
    #endif
            ///---------------------------------------|
            /// Наш првильный путь: "ogre.h"          |
            ///---------------------------------------:
            Ogre::String ogreCfgPath{"ogre.cfg"};
            mRoot = OGRE_NEW Ogre::Root(
                pluginsPath,
            /// mFSLayer->getWritablePath("ogre.cfg"), /// Было.
                ogreCfgPath,
                mFSLayer->getWritablePath("ogre.log")
            );
#endif

#ifdef OGRE_BITES_STATIC_PLUGINS
            mStaticPluginLoader.load();
#endif
            mOverlaySystem = OGRE_NEW Ogre::OverlaySystem();
        }

        ///-------------------------------------------|
        /// Установка иконки на окно.                 |
        ///-------------------------------------------:
        void InspectorRoot::setWindowIcon(Ogre::RenderWindow* window)
        {
#ifdef _WIN32
            HWND  hwnd   ; window->getCustomAttribute("WINDOW", &hwnd);
            HICON hIcon{};

            if (hwnd)
            {   /*
                hIcon = (HICON)LoadImage(
                    NULL,
                    L"icon.ico",
                    IMAGE_ICON,
                    0, 0,
                    LR_LOADFROMFILE | LR_DEFAULTSIZE
                );
                */
                if (!hIcon)
                {   // Из ресурсов (если есть .rc файл)
                    hIcon = LoadIcon(GetModuleHandle(NULL),
                                     MAKEINTRESOURCE(101));
                }
            
                if (hIcon)
                {   // WM_SETICON - сообщение Windows API
                    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
                    SendMessage(hwnd, WM_SETICON, ICON_BIG  , (LPARAM)hIcon);
                }
            }
#endif
        }

    ///------------------------------------------------------------------------|
    /// Дебажный вывод.
    ///------------------------------------------------------------------------:
    std::ostream& operator<<(std::ostream& o, const std::vector<igm_t>& m)
    {   for(const auto it : m)
        {   o << std::format("id: {}\n", it->id);
        }   o << '\n';
        return o;
    }

    std::ostream& operator<<(std::ostream& o, const Gm_t& gm)
    {   l(gm.size()) l(gm.back().size())

        auto line
        {   [&o, &gm]()
            {   o << std::string(gm.back().size()  * 5, '-') << std::endl;
            }
        };

        for(auto r{gm.crbegin()}; r != gm.crend(); ++r)
        {   
            line(); 

            for(const auto  e : *r)
            {   char c{'.'}; if(nullptr !=   e) c = '0' + (int8_t)e->id;
                o << std::format(" {:2} |", c);
            }   o << '\n';
        }       line();
                o << '\n';
        return  o;
    }
}