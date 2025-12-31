///----------------------------------------------------------------------------|
/// "light.h"
///----------------------------------------------------------------------------:
#ifndef LIGHT_H
#define LIGHT_H
#include "config-game.h"

///---------|
/// Models. |
///---------:
namespace mdl
{   
    using namespace Ogre;
    using namespace OgreBites;

    ///------------------------------------------------------------------------|
    /// 
    ///------------------------------------------------------------------------:
    struct  LightBase : Glob
    {       LightBase()
            {   
            }

        #define COLOR inline static const ColourValue
          COLOR colourW{ColourValue::White};
          COLOR colour8{  0.8f, 0.8f, 0.8f};
          COLOR colour5{  0.5f, 0.5f, 0.5f};
          COLOR colour2{  0.2f, 0.2f, 0.2f};
          COLOR colour0{  0.0f, 0.0f, 0.0f};
        #undef  COLOR

        Ogre::ColourValue diffuse {colourW};
        Ogre::ColourValue specular{colourW};

        Light*     light{nullptr};
        SceneNode* node {nullptr};

        ///---------------------------------------|
        /// Инициализация.                        |
        ///---------------------------------------:
        void setup(const std::string&    name)
        {   light = scnMgr->createLight (name);
            light->setDiffuseColour  (diffuse);
            light->setSpecularColour(specular);
        }

        void on () { light->setVisible(true ); }
        void off() { light->setVisible(false); }
    };


    ///------------------------------------------------------------------------|
    /// LightDir
    ///--------------------------------------------------------------- LightDir:
    struct  LightDir : LightBase
    {       LightDir()
            {   
            }

        ///---------------------------------------|
        /// Инициализация.                        |
        ///---------------------------------------:
        void setup(const std::string&    name)
        {   
            diffuse  = colour5;
            specular = colour5;

            LightBase::setup(name);

            light->setType(Light::LT_DIRECTIONAL);

            node = scnMgr->getRootSceneNode()->createChildSceneNode();
            node->attachObject(light);
            node->setDirection(Vector3(0.5f, -1, -1).normalisedCopy());

            light->setCastShadows      (true);
            light->setShadowFarDistance(3000.0f);
        }

        ///---------------------------------------|
        /// Динамика.                             |
        ///---------------------------------------:
        void update()
        {   
        }

        ///---------------------------------------|
        /// Обработка клавиш.                     |
        ///---------------------------------------:
        bool keyPressed([[maybe_unused]] const KeyboardEvent& evt)
        {
            return false;
        }
        
        private:

    };


    ///------------------------------------------------------------------------|
    /// LightSpot
    ///-------------------------------------------------------------- LightSpot:
    struct  LightSpot : LightBase
    {       LightSpot()
            {   
            }

        ///---------------------------------------|
        /// Инициализация.                        |
        ///---------------------------------------:
        void setup(const std::string& name)
        {   
            diffuse  = colour5;
            specular = colour8;
            LightBase::setup(name);

            light->setType(Light::LT_SPOTLIGHT);

            node = scnMgr->getRootSceneNode()->createChildSceneNode();
            node->attachObject(light);
            node->setDirection(Vector3(0, -0.5f, -1).normalisedCopy());
            node->setPosition (Vector3(0, 900, 1400));

            light->setSpotlightRange   (Degree(100), Degree(120));
            light->setCastShadows      (true   );
            light->setShadowFarDistance(2000.0f);
        }

        ///---------------------------------------|
        /// Динамика.                             |
        ///---------------------------------------:
        void update()
        {   
        }

        ///---------------------------------------|
        /// Обработка клавиш.                     |
        ///---------------------------------------:
        bool keyPressed([[maybe_unused]] const KeyboardEvent& evt)
        {
            return false;
        }
        
        private:

    };


    ///------------------------------------------------------------------------|
    /// ManLights
    ///-------------------------------------------------------------- ManLights:
    struct  ManLights
    {       ManLights()
            {   
            }

        ///---------------------------------------|
        /// Инициализация.                        |
        ///---------------------------------------:
        void setup()
        {   lightDir .setup("LDir" );
            lightSpot.setup("LSpot");
        }

        void doMenu()
        {   lightDir .on ();
            lightSpot.off();
        }

        void doOnePlayer()
        {   lightDir .off();
            lightSpot.on ();
        }

        void doTwoPlayers()
        {   lightDir .on ();
            lightSpot.off();
        }

        ///---------------------------------------|
        /// Динамика.                             |
        ///---------------------------------------:
        void update()
        {   
        }

        ///---------------------------------------|
        /// Обработка клавиш.                     |
        ///---------------------------------------:
        bool keyPressed([[maybe_unused]] const KeyboardEvent& evt)
        {
            return false;
        }
        
    private:
        LightDir  lightDir ;
        LightSpot lightSpot;
    };

}

#endif // LIGHT_H

