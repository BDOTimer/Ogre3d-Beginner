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
        
                
                if (ParticleEmitter* emitter = snowPS->getEmitter(0))
                {
                    emitter->setParameter("width", "2500");
                    emitter->setParameter("height", "2500");
                    emitter->setParameter("time_to_live", "8");
                    emitter->setParameter("velocity", "200");
                    emitter->setParameter("emission_rate", "50");
                }
                
                if (ParticleAffector* affector = snowPS->getAffector(0))
                {
                    affector->setParameter("randomness", "100");
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
                SceneNode* snowNode = snowPS->getParentSceneNode();
                if (snowNode) snowNode->detachObject(snowPS);
                scnMgr->destroyParticleSystem(snowPS);
                snowPS = nullptr;
                if (snowNode && snowNode->getName() == "SnowNode")
                    scnMgr->destroySceneNode(snowNode);
            }
        }

        void setEmittingSnow(bool emitting)
        {
            if (snowPS != nullptr)
                snowPS->setEmitting(emitting);
        }
        
        private:
            ParticleSystem* snowPS = nullptr;

    };

}

#endif // EFFECTS_H

