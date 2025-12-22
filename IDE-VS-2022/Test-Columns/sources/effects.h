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
 
    struct  Effects : Glob
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
        
            try
            {   snowPS = scnMgr->createParticleSystem(
                    "SnowParticleSystem", "Examples/Snow");
        
                snowPS->setDefaultDimensions(15.0f, 15.0f);
        
                if (ParticleEmitter* emitter = snowPS->getEmitter(0))
                {
                    emitter->setParameter("width", "2500");
                    emitter->setParameter("height", "2500");
                    emitter->setParameter("time_to_live", "8");
                    emitter->setParameter("velocity", "200");
                    emitter->setParameter("emission_rate", "1");
                }
                
                if (ParticleAffector* affector = snowPS->getAffector(0))
                {
                    affector->setParameter("randomness", "100");
                }
        
                SceneNode* snowNode 
                    = Glob::nodeBase->createChildSceneNode("SnowNode");
                snowNode->attachObject(snowPS);
                snowNode->setPosition(0, 1500, 0);
        
            }
            catch (Exception& e)
            {   std::cerr
                    << "Error creating snow effect: " 
                    << e.getFullDescription() << std::endl;
            }
        }
        
        void destroySnowEffect()
        {
            if (snowPS != nullptr) 
            {
                SceneNode* snowNode = snowPS->getParentSceneNode();
                if (snowNode) snowNode->detachObject(snowPS);

                scnMgr->destroyParticleSystem(snowPS);
                snowPS = nullptr;
                if (snowNode && snowNode->getName() == "SnowNode")
                {   scnMgr->destroySceneNode(snowNode);
                }
            }
        }
 
        void setSnow(bool emitting)
        {   if (snowPS != nullptr)
            {   snowPS->setEmitting(emitting);
            }
        }

        void setRate(float n)
        {   if (ParticleEmitter* emitter = snowPS->getEmitter(0))
            {   emitter->setEmissionRate(n);
            }
        }

        /// TODO: плавность смены интенсивности ...
        bool isSnow{false};
        void update(float seconds)
        {   
            if(unsigned (seconds) % 30 == 0)
            {   setSnow(isSnow = !isSnow);

                if(isSnow)
                {   setRate(float((rand()% 3) * 50 + 1));
                }
            }
        }
        
        private:
            ParticleSystem* snowPS{nullptr};
    };
 

    ///------------------------------------------------------------------------|
    /// ...
    ///------------------------------------------------------------------------:
    struct  AutoCotrollerSin : Glob
    {       AutoCotrollerSin(float& val, std::function<void()> cb) :
                K(val)
            {   this->val = &val;
                callback  =   cb;
            }

        float update()
        {   auto ss1 = std::sinf(t);
            *val = K + a * ss1;
            callback();

            t += speed * Glob::deltaTime;
        }

    private:
        float* val;
        float  speed{0.2f};
        float  a    {600.f};
        const float  K;
        float t{0};
        int  cnt{};

        std::function<void()> callback;
    };
}
 
#endif // EFFECTS_H