///----------------------------------------------------------------------------|
/// 
///----------------------------------------------------------------------------:
#pragma warning(push, 0)
#pragma warning(disable: 4275 4251 4305)
#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include <iostream>
#pragma warning(pop)


#include <cstdlib>
struct  _1start
{       _1start()
        {
        #ifdef _WIN32
            _putenv_s("OGRE_CONFIG_DIR",
                       "D:\\!!!pro-cyberX\\myTests\\Ogre3d-Beginner\\bin");
        #else
            setenv("OGRE_CONFIG_DIR", "/myapp/config", 1);
        #endif
        }
}_1start$;



using namespace Ogre;
using namespace OgreBites;

class BasicTutorial1
        : public ApplicationContext
        , public InputListener
{
public:
    BasicTutorial1();
    virtual ~BasicTutorial1() {}

    void setup();
    bool keyPressed(const KeyboardEvent& evt);

    void locateResourcesX() ///override
    {
        // Не создавать директорию в Documents
        // ApplicationContext::locateResources();
    }
};


BasicTutorial1::BasicTutorial1()
    : ApplicationContext("OgreTutorialApp")
{   
}


void BasicTutorial1::setup()
{
    ApplicationContext::setup();
    addInputListener(this);

    Root* root = getRoot();

    SceneManager* scnMgr = root->createSceneManager();

    RTShader::ShaderGenerator* shadergen = 
        RTShader::ShaderGenerator::getSingletonPtr();
    shadergen->addSceneManager(scnMgr);

    // -- tutorial section start --

    scnMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));

    Light* light = scnMgr->createLight("MainLight");
    SceneNode* lightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    lightNode->attachObject(light);
    lightNode->setPosition(20, 80, 50);

    Camera* cam = scnMgr->createCamera("myCam");
    cam->setNearClipDistance(5); // specific to this sample
    cam->setAutoAspectRatio(true);

    SceneNode* camNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    camNode->attachObject(cam);
    camNode->setPosition (0,  0, 140);
    camNode->setPosition (0, 47, 222);

    getRenderWindow()->addViewport(cam);

    Entity*  ogreEntity = scnMgr->createEntity("ogrehead.mesh");
    SceneNode* ogreNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    ogreNode->attachObject(ogreEntity);

    Entity*  ogreEntity2 = scnMgr->createEntity("ogrehead.mesh");
    SceneNode* ogreNode2 = scnMgr->
        getRootSceneNode()->createChildSceneNode(Vector3(84, 48, 0));
    ogreNode2->attachObject(ogreEntity2);

    Entity*  ogreEntity3 = scnMgr->createEntity("ogrehead.mesh");
    SceneNode* ogreNode3 = scnMgr->getRootSceneNode()->createChildSceneNode();
    ogreNode3->setPosition (0, 104, 0);
    ogreNode3->setScale    (2, 1.2f, 1);
    ogreNode3->attachObject(ogreEntity3);

    Entity*  ogreEntity4 = scnMgr->createEntity("ogrehead.mesh");
    SceneNode* ogreNode4 = scnMgr->getRootSceneNode()->createChildSceneNode();
    ogreNode4->setPosition(-84, 48, 0);
    ogreNode4->roll(Degree(-90));
    ogreNode4->attachObject(ogreEntity4);

    // -- tutorial section end --
}


bool BasicTutorial1::keyPressed(const KeyboardEvent& evt)
{
    if (evt.keysym.sym == SDLK_ESCAPE)
    {
        getRoot()->queueEndRendering();
    }
    return true;
}


int main(int argc, char **argv)
{


    std::cout << "Hi, I'm Columns!\n\n";
    
    ///------------------------------------------------|
    /// Отключаем логи в MeshManager.                  |
    ///------------------------------------------------:
///    Ogre::MeshManager::getSingleton().setVerbose(false);


    try
    {
        Ogre::LogManager logMgr;
        logMgr.createLog("", false, false, false);  // Пустой лог

    	BasicTutorial1 app;
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

