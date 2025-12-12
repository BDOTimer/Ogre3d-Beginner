///----------------------------------------------------------------------------|
/// Test2
///----------------------------------------------------------------------------:
#pragma warning(push, 0)
#pragma warning(disable: 4275 4251 4305)
#include <exception>
#include <iostream>

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include "OgreCameraMan.h"

///-------------------------------------------|
/// Свой путь к "ogre.h"                      |
///-------------------------------------------:
#include "OgreConfigPaths.h"
#include "OgreOverlaySystem.h"
#pragma warning(pop)


namespace mdl
{   
    using namespace Ogre;
    using namespace OgreBites;
    ///------------------------------------------------------------------------|
    /// Base.
    ///------------------------------------------------------------------- Base:
    struct  InspectorRoot;
    struct  Base
    {   inline static InspectorRoot     * pInspectorRoot{ nullptr };
        inline static ApplicationContext* ctx            { nullptr };
    };

    ///------------------------------------------------------------------------|
    /// Camera.
    ///----------------------------------------------------------------- Camera:
    struct  Camera : Base
    {   
        Ogre::SceneNode* camNode;
        Ogre::Camera*        cam;
        Ogre::Viewport*       vp;

        std::unique_ptr<OgreBites::CameraMan> man;

        Ogre::Camera* get() const { return cam; }

        void setup(Ogre::SceneManager* scnMgr)
        {   
            cam = scnMgr->createCamera("myCam");
            cam->setNearClipDistance  (5);

            camNode = scnMgr->getRootSceneNode()->createChildSceneNode();
            camNode->setPosition   (0, 300, 600);
            camNode->lookAt(Vector3(0, 100, 0), Node::TransformSpace::TS_WORLD);
            camNode->attachObject  (cam);

            vp = ctx->getRenderWindow()->addViewport(cam);
            vp->setBackgroundColour(ColourValue(0, 0, 0.1f));

            cam->setAspectRatio(Real(vp->getActualWidth ()) / 
                                Real(vp->getActualHeight()));

            ///------------------|
            /// Manager.         |
            ///------------------:
            man = std::make_unique<OgreBites::CameraMan>(camNode);
            man->setStyle(OgreBites::CS_FREELOOK); // или CS_ORBIT
            man->setTopSpeed(400);
            ctx->addInputListener(man.get());
        }
    };


    ///------------------------------------------------------------------------|
    /// Нидзя.
    ///------------------------------------------------------------------ Ninja:
    struct  Ninja : Base
    {       
        Ogre::Entity*  entity;
        SceneNode*     node  ;

        void setup(Ogre::SceneManager* scnMgr)
        {   
            entity  = scnMgr->createEntity("ninja.mesh");
            entity  ->setCastShadows  (true);

            node = scnMgr->getRootSceneNode()->createChildSceneNode();
            node->attachObject   (entity);
            node->yaw(Ogre::Degree (180));
            node->setPosition (200, 0, 0);
            node->setScale(1.5, 1.5, 1.5);
        }
    };

    ///------------------------------------------------------------------------|
    /// Sphere.
    ///----------------------------------------------------------------- Sphere:
    struct  Sphere : Base
    {       
        Ogre::Entity*  entity;
        SceneNode*     node  ;

        const char* name1{"OrangeMaterial"};

        inline static std::array<ColourValue, 5> color
        {   ColourValue{1.0 , 0.5f, 0.0},
            ColourValue{1.0 , 0.0f, 0.5},
            ColourValue{0.0 , 1.0f, 0.0},
            ColourValue{0.0 , 0.5f, 1.0},
            ColourValue{0.0 , 0.0f, 1.0},
        };

        std::array<MaterialPtr, 5> mat;


        void setup(Ogre::SceneManager* scnMgr)
        {   for(unsigned i{}; i < color.size(); ++i)
            {   setup(scnMgr, i);
            }
        }


        void setup(Ogre::SceneManager* scnMgr, const size_t n)
        {   
            std::string sn{std::to_string(n)};
            
            std::string name {"Sphere"}; name  += sn;
            std::string nameM{"matSph"}; nameM += sn;

            entity = scnMgr->createEntity(name, "sphere.mesh");
            node = scnMgr->getRootSceneNode()->createChildSceneNode();
            node->attachObject(entity);
            node->setPosition(0, 50 + (100 * float(n)), 0);
            node->setScale(0.5f, 0.5f, 0.5f);

            mat[n] = MaterialManager::getSingleton().create(
                nameM,
                ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
            );

            Ogre::Pass* const p = mat[n]->getTechnique(0)->getPass(0);
                        p->setDiffuse (color[n]);
                        p->setAmbient (ColourValue(0.3f, 0.15f, 0.0));
                        p->setSpecular(ColourValue(1.0 , 1.0  , 1.0));
                        p->setShininess(64.0);

            entity->setMaterialName(nameM);
        }
    };

    ///------------------------------------------------------------------------|
    /// Свет.
    ///----------------------------------------------------------------- Lights:
    struct  Lights : Base
    {       
        Ogre::Entity*  entity;

        void setup(Ogre::SceneManager* scnMgr)
        {   
            using namespace Ogre;

            ColourValue colourValue(0.5f, 0.5f, 0.5f);

            ///------------------|
            /// directionalLight |
            ///------------------:
            Light* directionalLight = scnMgr->createLight("DirectionalLight");
            directionalLight->setType(Light::LT_DIRECTIONAL);
            directionalLight->setDiffuseColour (colourValue);
            directionalLight->setSpecularColour(colourValue);

            SceneNode* directionalLightNode
                = scnMgr->getRootSceneNode()->createChildSceneNode();
            directionalLightNode->attachObject(directionalLight);
            directionalLightNode->setDirection(Vector3(0, -1, -1));

            ///------------------|
            /// pointLight       |
            ///------------------:
            Light* pointLight = scnMgr->createLight("PointLight");
            pointLight->setType(Light::LT_POINT);

            pointLight->setDiffuseColour (0.3f, 0.3f, 0.3f);
            pointLight->setSpecularColour(0.3f, 0.3f, 0.3f);

            SceneNode* pointLightNode
          = scnMgr->getRootSceneNode()->createChildSceneNode();
            pointLightNode->attachObject(pointLight);
            pointLightNode->setPosition(Vector3(0, 300, 600));

            //return;

            ///------------------|
            /// spotLight        |
            ///------------------:
            Light* spotLight = scnMgr->createLight("SpotLight");
            spotLight->setDiffuseColour (1, 1, 1.0);
            spotLight->setSpecularColour(1, 1, 1.0);
            spotLight->setType(Light::LT_SPOTLIGHT);

            SceneNode* spotLightNode
                = scnMgr->getRootSceneNode()->createChildSceneNode();
            spotLightNode->attachObject(spotLight);
            spotLightNode->setDirection(0, 0, 1);
            spotLightNode->setPosition(Vector3(0, 300, 600));

            spotLight->setSpotlightRange(Degree(100), Degree(100));

            spotLight->setVisible(false);
        }
    };


    ///------------------------------------------------------------------------|
    /// InspectorRoot.
    ///---------------------------------------------------------- InspectorRoot:
    struct  InspectorRoot
            :   Base
            ,   OgreBites::ApplicationContext
            ,   OgreBites::InputListener
    {       InspectorRoot( ): OgreBites::ApplicationContext("OgreTutorialApp")
            {}
        
        Ogre::Root*           root;
        Ogre::SceneManager* scnMgr;

        Camera              camera;
        Ninja                ninja;
        Sphere              sphere;
        Lights              lights;
        
        Ogre::RTShader::ShaderGenerator* shadergen;

    protected:

        ///-------------------------------------------|
        /// Свой путь к "ogre.h"                      |
        ///-------------------------------------------:
        void createRoot() override
        {
            std::cout << "RUN: createRoot() override\n\n";

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
            mRoot = OGRE_NEW Ogre::Root("");
#else
            Ogre::String pluginsPath;
#   ifndef OGRE_BITES_STATIC_PLUGINS
            pluginsPath = mFSLayer->getConfigFilePath("plugins.cfg");

        if (!Ogre::FileSystemLayer::fileExists(pluginsPath))
        {
            pluginsPath = Ogre::FileSystemLayer::resolveBundlePath(
                OGRE_CONFIG_DIR "/plugins.cfg"
            );
        }
#   endif
            Ogre::String ogreCfgPath{"ogre.cfg"};
            mRoot = OGRE_NEW Ogre::Root(
                pluginsPath,
            /// mFSLayer->getWritablePath("ogre.cfg"),
                ogreCfgPath,
                mFSLayer->getWritablePath("ogre.log")
            );
#endif

#ifdef OGRE_BITES_STATIC_PLUGINS
            mStaticPluginLoader.load();
#endif
            mOverlaySystem = OGRE_NEW Ogre::OverlaySystem();
        }


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

            shadergen
          = RTShader::ShaderGenerator::getSingletonPtr();
            shadergen->addSceneManager(scnMgr);

            camera.setup(scnMgr);
            ninja .setup(scnMgr);
            sphere.setup(scnMgr);
            lights.setup(scnMgr);

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


///----------------------------------------------------------------------------|
/// TutorialApplication.
///-------------------------------------------------------- TutorialApplication:
struct  TutorialApplication
{        TutorialApplication()
        {   setup();
        }

    virtual ~TutorialApplication() {}

    mdl::InspectorRoot inspectorRoot;

    void setup()
    {   inspectorRoot.initApp();
        inspectorRoot.getRoot()->startRendering();
        inspectorRoot.closeApp();
    }
};

///----------------------------------------------------------------------------|
/// Старт.
///----------------------------------------------------------------------------:
int main(int argc, char **argv)
{
    std::cout << "Hi, I'm Test2!\n\n";

    try
    {
        ///------------------------------------------------|
        /// Отключаем логи в MeshManager.                  |
        ///------------------------------------------------:
        Ogre::LogManager logMgr;
        logMgr.createLog("", false, false, false);  // Пустой лог

        TutorialApplication app;
    }
    catch (const std::exception& e)
    {
        std::cerr << "ERRORFATAL: " << e.what() << '\n';
        return 1;
    }

    std::cout << "\nTest2 FINISHED!\n\n"; return 0;
}
