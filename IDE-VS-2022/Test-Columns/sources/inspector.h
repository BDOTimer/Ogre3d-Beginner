///----------------------------------------------------------------------------|
/// "inspector.h"
///----------------------------------------------------------------------------:
#ifndef INSPECTOR_H
#define INSPECTOR_H
#include "well.h"
#include "ui.h"

///---------|
/// Models. |
///---------:
namespace mdl
{  
    ///------------------------------------------------------------------------|
    /// InspectorRoot.
    ///---------------------------------------------------------- InspectorRoot:
    struct  InspectorRoot
            :   Base
            ,   OgreBites::ApplicationContext
            ,   OgreBites::InputListener
    {       InspectorRoot( ): OgreBites::ApplicationContext("Test-Columns-2025")
            {}
        
        Ogre::Root*           root;
        Ogre::SceneManager* scnMgr;

        Camera camera;
        Ninja   ninja;
        Sphere sphere;
        Lights lights;
        UI         ui;
        Tree     tree;
        
        Ogre::RTShader::ShaderGenerator* shadergen;

    protected:

        ///-------------------------------------------|
        /// Свой путь к "ogre.h"                      |
        ///-------------------------------------------:
        void createRoot() override;

        void setup() override
        {   
            OgreBites::ApplicationContext::setup();
            addInputListener(this);

            Base::pInspectorRoot = this;
            Base::ctx             = this;

            root   = getRoot();
            scnMgr = root->createSceneManager();
            scnMgr->setAmbientLight(ColourValue(0, 0, 0));
            scnMgr->setShadowTechnique(
                ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);

        /// root->loadPlugin("OgreAssimp");

            shadergen
          = RTShader::ShaderGenerator::getSingletonPtr();
            shadergen->addSceneManager(scnMgr);

            camera.setup(scnMgr);
            ninja .setup(scnMgr);
            sphere.setup(scnMgr);
            lights.setup(scnMgr);
            tree  .setup(scnMgr);

            Plane plane(Vector3::UNIT_Y, 0);

            MeshManager::getSingleton().createPlane(
                "ground", RGN_DEFAULT,
                plane,
                1500, 1500, 20, 20,
                true,
                1, 5, 5,
                Vector3::UNIT_Z
            );

            Entity* groundEntity = scnMgr->createEntity("ground");
            scnMgr  ->getRootSceneNode    ()
                    ->createChildSceneNode()
                    ->attachObject(groundEntity);

            groundEntity->setCastShadows(false);
            groundEntity->setMaterialName("Examples/Rockwall");
        }

        bool keyPressed(const KeyboardEvent& evt)
        {   if (evt.keysym.sym == SDLK_ESCAPE)
            {   getRoot()->queueEndRendering();
            }
            return true;
        }
    };
}

#endif // INSPECTOR_H

