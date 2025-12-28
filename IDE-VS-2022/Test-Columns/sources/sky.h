///----------------------------------------------------------------------------|
/// "sky.h"
///----------------------------------------------------------------------------:
#ifndef SKY_H
#define SKY_H
#include "config-game.h"

///---------|
/// Models. |
///---------:
namespace mdl
{   
    using namespace Ogre;
    using namespace OgreBites;

    ///------------------------------------------------------------------------|
    /// Небо.
    ///-------------------------------------------------------------------- Sky:
    struct  Sky : Glob
    {       Sky()
            {   
            }

        inline static std::array<const char*, 6> nameSky
        {   "Examples/SpaceSkyBox" ,
            "Examples/TrippySkyBox",
            "Examples/CloudyNoonSkyBox",
            "Examples/StormySkyBox",
            "Examples/EarlyMorningSkyBox",
            "Examples/CloudySky"
        };

        void setup(unsigned i = 2) // 2
        {   
            Glob::scnMgr->setSkyBox(
                true,
                nameSky[i % nameSky.size()],
                5000,
                true,                 // отрисовывать первым
                Quaternion::IDENTITY, // ориентация
                "General"
            );
        }

        void update()
        {   SceneNode*  nd = Glob::scnMgr->getSkyNode();
                        nd->yaw(Radian(0.1f * deltaTime));
        }
        
        private:

    };

}

#endif // SKY_H

