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
        {   ""
            "Examples/SpaceSkyBox" ,
            "Examples/TrippySkyBox",
            "Examples/CloudyNoonSkyBox",
            "Examples/StormySkyBox",
            "Examples/EarlyMorningSkyBox",
            "Examples/CloudySky"
        };

        void setup()
        {   
            box      .setup("T_Skybox_day_7_D_proc");
            box.node->scale(6000.f, 6000.f, 6000.f);
            box.node->pitch(Degree(-90));
            box.node->setVisible(true);

            Glob::scnMgr->setSkyBox(
                true,
                nameSky[1],
                7000,
                true,                 // отрисовывать первым
                Quaternion::IDENTITY, // ориентация
                "General"
            );
            nd = Glob::scnMgr->getSkyNode();
        }

        void update()
        {   nd->yaw(Radian(0.1f  * deltaTime));
            box.node->roll(Radian(0.03f * deltaTime));
        }

        void toggle()
        {   
            if(j)
            {
                Glob::scnMgr->setSkyBox(
                        true,
                        nameSky[j],
                        7000,
                        true,                 // отрисовывать первым
                        Quaternion::IDENTITY, // ориентация
                        "General"
                    );

                nd = Glob::scnMgr->getSkyNode();
                box.node->setVisible(false);
            }
            else box.node->setVisible(true);

            j = ++j < nameSky.size() ? j : 0;
        }
        
    private:
        Model     box;
        SceneNode* nd{nullptr};
        unsigned j{1};
    };

}

#endif // SKY_H

