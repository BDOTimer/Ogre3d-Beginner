///----------------------------------------------------------------------------|
/// "primitives.h"
///----------------------------------------------------------------------------:
#ifndef PRIMITIVES_H
#define PRIMITIVES_H
#include "config-game.h"

///---------|
/// Models. |
///---------:
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
            entity = scnMgr->createEntity("ninja.mesh");
            entity ->setCastShadows(true);

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
    /// Нидзя.
    ///------------------------------------------------------------------ Ninja:
    struct  Obj5File : Base
    {       
        Ogre::Entity*  entity;
        SceneNode*     node  ;
        Ogre::MeshPtr  mesh  ;

        const char* name1{"myModel"};

        void setup(Ogre::SceneManager* scnMgr, Ogre::String filename)
        {   
            mesh = Ogre::MeshManager::getSingleton().load(
                "model.obj",            // имя файла
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                Ogre::HardwareBuffer::HBU_STATIC,
                Ogre::HardwareBuffer::HBU_STATIC
            );

            entity = scnMgr->createEntity("myModel", "model.obj");
            node   = scnMgr->getRootSceneNode()->createChildSceneNode();
            node->attachObject(entity);
            node->setPosition (0, 0, 0);
            node->setScale    (0.1f, 0.1f, 0.1f);
        }
    };

    ///------------------------------------------------------------------------|
    /// Ёлка.
    ///------------------------------------------------------------------ Ninja:
    struct  Tree : Base
    {       
        Ogre::Entity*  entity;
        SceneNode*     node  ;

        void setup(Ogre::SceneManager* scnMgr)
        {   
            entity = scnMgr->createEntity("12150_Christmas_Tree_V2_L2.mesh");
            entity ->setCastShadows(true);

            node = scnMgr->getRootSceneNode()->createChildSceneNode();
            node->attachObject       (entity);

            //node->pitch(Ogre::Degree   (-90));
            node->setOrientation(Ogre::Quaternion(
                Ogre::Degree(-90), Ogre::Vector3::UNIT_X));

            node->setPosition    (-200, 0, 0);
            node->setScale    (2.5, 2.5, 2.5);
        }
    };
}

#endif // PRIMITIVES_H