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
    /// https://rigmodels.com/?searchkeyword=skybox
    /// https://sketchfab.com/feed
    ///-------------------------------------------------------------------- Sky:
    struct  Sky : Glob
    {       Sky()
            {   
            }

        inline static std::array<const char*, 8> nameSky
        {   "",
            "",
            "Examples/SpaceSkyBox" ,
            "Examples/TrippySkyBox",
            "Examples/CloudyNoonSkyBox",
            "Examples/StormySkyBox",
            "Examples/EarlyMorningSkyBox",
            "Examples/CloudySky"
        };

        void setup()
        {   
            box[0]      .setup("T_Skybox_day_7_D_proc");
            //box[0].node->scale(1000.f, 1000.f, 1000.f);
            box[0].node->pitch(Degree(-90));
            box[0].node->setVisible(true);

            box[1]  .setup("Cartoon_Desert_Skybox_2");
            box[1].node->setScale(100.f, 100.f, 100.f);
            box[1].node->setVisible(false);

        /// Glob::scnMgr->setSkyDome(
        ///    true, "Examples/CloudySky", 5, 8, 5000);

            Glob::scnMgr->setSkyBox(
                true,
                nameSky[2],
                7000,
                true,                 // отрисовывать первым
                Quaternion::IDENTITY, // ориентация
                "General"
            );
            nd = Glob::scnMgr->getSkyNode();
            nd->setVisible(false);
        }

        void update()
        {   nd->yaw(Radian(0.1f  * deltaTime));
            if(n == 0 ) box[0].node->roll(Radian( 0.03f * deltaTime));
            if(n == 1 ) box[1].node->yaw (Radian(-0.03f * deltaTime));
        }

        void toggle()
        {   
            if(j)
            {
                if(1 == j)
                {
                    box[1].node->setVisible(true);
                    n = 1;
                }
                else box[1].node->setVisible(false);

                if(j > 1)
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
                    //nd->setVisible(false);////////////////////////////////////

                }
                
                box[0].node->setVisible(false);
            }
            else {box[0].node->setVisible(true); n = 0; }

            j = ++j < nameSky.size() ? j : 0;
        }
        
    private:
        Model     box[2];
        SceneNode* nd{nullptr};
        unsigned j{1};
        unsigned n{0};
    };

}

#endif // SKY_H

