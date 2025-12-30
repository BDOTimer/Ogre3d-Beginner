#pragma warning(push, 0)
#pragma warning(disable: 4275 4251)
#include <exception>
#include <iostream>

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include "OgreCameraMan.h"

using namespace Ogre;
using namespace OgreBites;

SceneManager*      scnMgr;
ApplicationContext*   ctx;

namespace mdl
{
        ///------------------------------------------------------------------------|
    /// Camera.
    ///----------------------------------------------------------------- Camera:
    struct  Camera
    {   
        Ogre::SceneNode* camNode;
        Ogre::SceneNode* camGoal;
        Ogre::Camera*        cam;
        Ogre::Viewport*       vp;

        std::unique_ptr<OgreBites::CameraMan> man;

        Ogre::Camera* get() const { return cam; }

        float val{2200.0f};

        void setup(SceneNode* nodeUser)
        {   
            cam = scnMgr->createCamera("myCam");
            cam->setNearClipDistance  (5);

            camGoal = scnMgr->getRootSceneNode()->createChildSceneNode();
            camGoal->setPosition(0, 850, 0);

            camNode = nodeUser->createChildSceneNode();
            camNode->setPosition   (0, 999, 1900);
            camNode->lookAt(Vector3(0, 1200, 0), Node::TransformSpace::TS_WORLD);
            camNode->attachObject  (cam);

            vp = ctx->getRenderWindow()->addViewport(cam);
            vp->setBackgroundColour(ColourValue(0, 0, 0.02f));

            cam->setAspectRatio(Real(vp->getActualWidth ()) /
                                Real(vp->getActualHeight()));

            cam->setAutoAspectRatio (true);

            ///------------------|
            /// Manager.         |
            ///------------------:
            man = std::make_unique<OgreBites::CameraMan>(camNode);

            man->setStyle(OgreBites::CS_ORBIT); // или  CS_FREELOOK
            man->setTarget(camGoal);
            man->setTopSpeed(100);
            man->setFixedYaw(true);
            ctx->addInputListener(man.get());
        }
    }camera;

    struct Tree
    {       
        Ogre::Entity*  entity;
        SceneNode*     node  ;

        void setup(SceneNode* nodeBase)
        {   
            entity = scnMgr->createEntity("SnowyPineTree.mesh");
            entity ->setCastShadows(true);

            node = nodeBase->createChildSceneNode("Tree");
            node->attachObject(entity);

            node->setPosition(-260, 0, -100);
            node->setScale   (   5, 5,    5);
        }
    };
}


using namespace Ogre;
using namespace OgreBites;

class TutorialApplication
        : public ApplicationContext
        , public InputListener
{
public:
    TutorialApplication();
    virtual ~TutorialApplication();

    void setup();
    bool keyPressed(const KeyboardEvent& evt);
};


TutorialApplication::TutorialApplication()
    : ApplicationContext("OgreTutorialApp")
{
}


TutorialApplication::~TutorialApplication()
{
}

void TutorialApplication::setup()
{
    ApplicationContext::setup();
    addInputListener(this);

    Root* root = getRoot();
    scnMgr = root->createSceneManager();

    ctx = this;

    RTShader::ShaderGenerator* shadergen = RTShader::ShaderGenerator::getSingletonPtr();
    shadergen->addSceneManager(scnMgr);

    scnMgr->setAmbientLight(ColourValue(0.1, 0.1, 0.1));
    //scnMgr->setShadowTechnique(ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);

    //scnMgr->setAmbientLight(ColourValue(0.1, 0.1, 0.1));
    scnMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
    scnMgr->setShadowTextureSettings(2048, 2);  // Высокое качество
    scnMgr->setShadowFarDistance(3000);
    scnMgr->setShadowDirectionalLightExtrusionDistance(2000);
    scnMgr->setShadowTextureSelfShadow(true);

    SceneNode* camNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    mdl::camera.setup(camNode);

    SceneNode* baseNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    mdl::Tree tree; tree.setup(baseNode);

    Entity* ninjaEntity = scnMgr->createEntity("ninja.mesh");
    ninjaEntity->setCastShadows(true);

    scnMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ninjaEntity);

    Plane plane(Vector3::UNIT_Y, 0);

    MeshManager::getSingleton().createPlane(
            "ground", RGN_DEFAULT,
            plane,
            1500, 1500, 20, 20,
            true,
            1, 5, 5,
            Vector3::UNIT_Z);

    Entity* groundEntity = scnMgr->createEntity("ground");
    scnMgr->getRootSceneNode()->createChildSceneNode()
          ->attachObject (groundEntity);
    groundEntity->setCastShadows(false);
    groundEntity->setMaterialName("Examples/Rockwall");

    ///------------|
    /// SpotLight  |
    ///------------:
    if(0)
    {
        Light* spotLight = scnMgr->createLight("SpotLight");

        spotLight->setDiffuseColour (0.7, 0.7, 0.7);
        spotLight->setSpecularColour(0.3, 0.3, 0.3);

        spotLight->setType(Light::LT_SPOTLIGHT);

        SceneNode* spotLightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
        spotLightNode->attachObject(spotLight);
        spotLightNode->setDirection(-1, -1, 0);
        spotLightNode->setPosition(Vector3(200, 200, 0));

        spotLight->setSpotlightRange(Degree(35), Degree(50));
    }

    ///------------|
    /// DirLight   |
    ///------------:
    if(1)
    {
        Light* directionalLight = scnMgr->createLight("DirectionalLight");
        directionalLight->setType(Light::LT_DIRECTIONAL);

        directionalLight->setDiffuseColour (ColourValue(0.7, 0.7, 0.7));
        directionalLight->setSpecularColour(ColourValue(0.4, 0.4, 0.3));

        SceneNode* directionalLightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
        directionalLightNode->attachObject(directionalLight);
        directionalLightNode->setDirection(Vector3(0, -1, 1));
    }

    ///------------|
    /// PointLight |
    ///------------:
    if(0)
    {
        Light* pointLight = scnMgr->createLight("PointLight");
        pointLight->setType(Light::LT_POINT);

        pointLight->setDiffuseColour (0.7, 0.7, 0.7);
        pointLight->setSpecularColour(0.3, 0.3, 0.3);

        SceneNode* pointLightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
        pointLightNode->attachObject(pointLight);
        pointLightNode->setPosition(Vector3(0, 150, 250));
    }
}


bool TutorialApplication::keyPressed(const KeyboardEvent& evt)
{
    if (evt.keysym.sym == SDLK_ESCAPE)
    {
        getRoot()->queueEndRendering();
    }
    return true;
}


int main(int argc, char **argv)
{
    try
    {
        TutorialApplication app;
        app.initApp();
        app.getRoot()->startRendering();
        app.closeApp();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error occurred during execution: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
#pragma warning(pop)