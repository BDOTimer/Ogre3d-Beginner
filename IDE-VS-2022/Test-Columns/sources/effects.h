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
 
    struct  Effects   : Glob
    {       Effects() : weather(*this)
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

        void update       (float seconds)
        {   weather.updateSecund(seconds);
        }

        ///--------------------------------------------------------------------|
        /// Плавность смены интенсивности  ...
        ///--------------------------------------------------------------------:
        struct  Weather
        {       Weather(Effects& e) : effects(e) {}

            void updateSecund(float seconds)
            {   
                if(unsigned (seconds) % ttp[ir] == 0)
                {   
                    ir += rand()% 3 - 1;

                    if     (ir <  0) ir = 0;
                    else if(ir >= N) ir = N - 2;

                    effects.setRate(inn[ir]);
                }
            }

        private:
            Effects& effects;
            int      ir   {};
            static constexpr const int N{8};
            const std::array<float,N> inn{  0,  1,  2,  8, 25, 50, 100, 150 };
            const std::array<int,  N> ttp{  5,  5, 10, 10, 10, 20,  10, 20  };
        }weather;

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
            return t;
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