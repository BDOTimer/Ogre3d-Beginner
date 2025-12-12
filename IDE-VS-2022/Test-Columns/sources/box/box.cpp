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
#include "OgreOverlaySystem.h"
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
            std::cout << "RUN: createRoot() override\n\n";

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
}