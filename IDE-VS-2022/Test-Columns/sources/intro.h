///----------------------------------------------------------------------------|
/// "intro.h"
///----------------------------------------------------------------------------:
#ifndef INTRO_H
#define INTRO_H
#include "primitives.h"

///---------|
/// Models. |
///---------:
namespace mdl
{   
    using namespace Ogre;
    using namespace OgreBites;

    ///-----------------------------|
    /// Rand.                       |
    ///-----------------------------:
    struct Rand
    {   Rand(            ){   srand((unsigned)time(0));}
        Rand(unsigned sid){   srand(sid)              ;}
        int operator(    )(int range_min, int range_max) const
        {   return rand() % (range_max - range_min) + range_min;
        }
    };

    inline Rand rrand;


    ///------------------------------------------------------------------------|
    /// 
    ///------------------------------------------------------------------------:
    struct InstancingNinja :  Glob
    {
        void setup()
        {
            const size_t numInstances = 1000;
            const size_t batchSize    =  100;

            InstanceManager* im = scnMgr->createInstanceManager(
                "myInstancer",
                "SnowyPineTree.mesh",
                ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                InstanceManager::HWInstancingBasic,
                batchSize
            );

            int count{};

            for     (int x = -5; x < 5 && count < numInstances; ++x)
            {   for (int z = -5; z < 5 && count < numInstances; ++z)
                {
                    InstancedEntity* ent = im->createInstancedEntity(
                        "Examples/Instancing/HWBasic/tree");

                    const auto&[X,Z,S] = getRnd();

                    ent->setPosition({ X, 0, Z });
                    ent->setScale   ({ S, S, S });
                    //ent->setCastShadows(true);
                    

                    ent->setRenderingDistance(0); // 0 = всегда рендерить
                    ent->setRenderQueueGroup(RENDER_QUEUE_MAIN);
                    ent->setVisibilityFlags(0xFFFFFFFF);

                    count++;
                }
            }
        }

        bool update([[maybe_unused]] const FrameEvent& evt)
        {

            return true;
        }

    private:
        std::tuple<float, float, float> getRnd() const
        {   
            float a = 6.28f * rrand(   0,  360) / 360;
            float r = (float) rrand(2000, 5000);

            float x = r * cosf(a);
            float z = r * sinf(a);
            float s = (float)rrand( 3, 15 );

            return {x, z, s};
        }
    };
}

#endif // INTRO_H

