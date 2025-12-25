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
        {   o << std::format("id: {} ---> {}\n", 
                it->id, 
                ConfigGame::get().decode4DescriptionGems(it->id));
        }   o << '\n';
        return o;
    }

    std::ostream& operator<<(std::ostream& o, const Gm_t& gm)
    {   l(gm.size()) l(gm.back().size())

        const char* pad{"   |"};

        auto line
        {   [&o, &gm, &pad]()
            {   o << pad << std::string(gm.back().size()  * 5, '-') << "\b|\n";
            }
        };

        for(auto r{gm.crbegin()}; r != gm.crend(); ++r)
        {   
            line();
            o << pad;

            for(const auto  e : *r)
            {   char c{'.'}; if(nullptr !=   e) c = '0' + (int8_t)e->id;
                o << std::format(" {:2} |", c);
            }   o << '\n';
        }       line();
                o << '\n';
        return  o;
    }

    void Well::infoNewGame2Console() const
    {   std::cout << '\n' << std::format("{}\n{}{:2}{}\n{}\n",
        "|----------------------------------|",
        "|     Новая игра  - ", Glob::cntGame," создана!    |",
        "|----------------------------------|");
    }

    ///---------------------------------|
    /// Ищем где совпало.               |
    ///---------------------------------:
    int WellLogic::findMatchGems()
    {   
        for       (auto& r : gm)
        {   for   (auto& e : r )
            {   if(nullptr != e)
                {   e->match.reset();
                }
            }
        }

        for(    size_t h{}; h < H; ++h)
        {   for(size_t w{}; w < W; ++w)
            {   
                ///------------------------|
                /// В фокусе только 1 раз! |
                ///------------------------:
                const auto& a{gm[h][w]};

                if(nullptr == a)
                {   continue;
                }
                    
                ///------------------------|
                /// Горизонталь.           |
                ///------------------------:
                if(size_t i = w + 1; i < W )
                {   const auto& b{gm[h][i]};
                        
                    if(nullptr != b && a->id == b->id)
                    {   b->match.addLG(a->match.getLG());
                    }
                }

                ///------------------------|
                /// Вертикаль.             |
                ///------------------------:
                if(size_t j = h + 1; j < H )
                {   const auto& b{gm[j][w]};
                        
                    if(nullptr != b && a->id == b->id)
                    {   b->match.addLV(a->match.getLV());
                    }
                }

                ///------------------------|
                /// Диагональ "Плюс".      |
                ///------------------------:
                if(size_t j = h + 1, i = w + 1; j < H && i < W )
                {   const auto& b{gm[j][i]};
                        
                    if(nullptr != b && a->id == b->id)
                    {   b->match.addL1(a->match.getL1());
                    }
                }

                ///------------------------|
                /// Диагональ "Минус".     |
                ///------------------------:
                if(size_t j = h + 1, i = w - 1; j < H && i < W )
                {   const auto& b{gm[j][i]};
                        
                    if(nullptr != b && a->id == b->id)
                    {   b->match.addL5(a->match.getL5());
                    }
                }
            }
        }

        int cnt{};

        for       (const auto& r : gm)
        {   for   (const auto& e : r )
            {   if(nullptr != e)
                {   if(e->match.doIsMatch())
                    {
                        ++cnt;
                    }
                }
            }
        }

        SetScore(statisticScore += cnt);

        return cnt;
    }

    void GemData::setGem2Well    (GemData* gem)
    {   ASSERT(pwellLogic)
        pwellLogic->setGem(gem);
    }

} // namespace mdl

