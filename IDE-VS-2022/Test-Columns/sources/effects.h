///----------------------------------------------------------------------------|
/// "effects.h"
/// в задачах:
///     - "Пар над ёлкой"
///     - "Снег идёт"
///----------------------------------------------------------------------------:
#ifndef EFFECTS_H
#define EFFECTS_H
#include "config-game.h"

///---------|
/// Models. |
///---------:
namespace mdl
{   
    using namespace Ogre;
    using namespace OgreBites;

    struct  Effects : Base
    {       Effects()
            {   
            }

        void setup()
        {   
            createSnowEffect();
        }

        void createSnowEffect()
        {
            if (snowPS != nullptr) return;
        
            try {
        
                snowPS = scnMgr->createParticleSystem("SnowParticleSystem", "Examples/Snow");
        
                snowPS->setDefaultDimensions(15.0f, 15.0f);
        
                
                if (snowPS->getNumEmitters() > 0) 
                {
        			snowPS->getEmitter(0)->setParameter("width", "2500");
        			snowPS->getEmitter(0)->setParameter("height", "2500");
                    snowPS->getEmitter(0)->setParameter("time_to_live", "8");
        			snowPS->getEmitter(0)->setParameter("velocity", "200");
        			snowPS->getEmitter(0)->setParameter("emission_rate", "50");
                }
        
                if (snowPS->getNumAffectors() > 0)
                {
                    snowPS->getAffector(0)->setParameter("randomness", "100");
                }
        
                SceneNode* snowNode = scnMgr->getRootSceneNode()->createChildSceneNode("SnowNode");
                snowNode->attachObject(snowPS);
                snowNode->setPosition(0, 1500, 0);
        
            }
            catch (Exception& e) {
                
                std::cerr << "Error creating snow effect: " << e.getFullDescription() << std::endl;
            }
        }
        
        void removeSnowEffect()
        {
            if (snowPS != nullptr) 
            {
                scnMgr->destroyParticleSystem(snowPS);
                snowPS = nullptr;
            }
        }
        
        private:
            ParticleSystem* snowPS = nullptr;

    };

}

#endif // EFFECTS_H

