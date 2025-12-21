///----------------------------------------------------------------------------|
/// "primitives.h"
///----------------------------------------------------------------------------:
#ifndef PRIMITIVES_H
#define PRIMITIVES_H
#include "config-game.h"
#include "effects.h"

///---------|
/// Models. |
///---------:
namespace mdl
{   

    ///------------------------------------------------------------------------|
    /// ExtendedCameraMan(управляем дистанцией).
    ///------------------------------------------------------- ExtendedCameraMan:
    struct  xExtendedCameraMan : public OgreBites::CameraMan
    {       xExtendedCameraMan(Ogre::SceneNode* cam) : OgreBites::CameraMan(cam)
            {   mOrbitDistance = 100.0f;
            }

        void setOrbitDistance(float distance)
        {   mOrbitDistance = distance;
            updateOrbitCamera();
        }

        void changeOrbitDistance(float delta)
        {   mOrbitDistance += delta;
            mOrbitDistance = Ogre::Math::Clamp(mOrbitDistance, 10.0f, 500.0f);
            updateOrbitCamera();
        }

        void setOrbitTarget(const Ogre::Vector3& target)
        {   mOrbitTarget = target;
            updateOrbitCamera();
        }

    private:
        float mOrbitDistance;
        Ogre::Vector3 mOrbitTarget;

        void updateOrbitCamera()
        {   // Получаем текущее направление от камеры к цели
            Ogre::Vector3 currentPos = mCamera->getPosition();
            Ogre::Vector3 direction = currentPos - mOrbitTarget;
            direction.normalise();

            // Новая позиция на нужном расстоянии
            Ogre::Vector3 newPos = mOrbitTarget + direction * mOrbitDistance;
            mCamera->setPosition(newPos);
        ///mCamera->lookAt(mOrbitTarget);
        }
    };

    ///------------------------------------------------------------------------|
    /// Camera.
    ///----------------------------------------------------------------- Camera:
    struct  Camera : Glob
    {   
        Ogre::SceneNode* camNode;
        Ogre::SceneNode* camGoal;
        Ogre::Camera*        cam;
        Ogre::Viewport*       vp;

        std::unique_ptr<OgreBites::CameraMan> man;

        Ogre::Camera* get() const { return cam; }

        void setup(SceneNode* nodeUser)
        {   
            cam = scnMgr->createCamera("myCam");
            cam->setNearClipDistance  (5);

            camGoal = scnMgr->getRootSceneNode()->createChildSceneNode();
            camGoal->setPosition(0, 900, 0);

            camNode = nodeUser->createChildSceneNode();
            camNode->setPosition   (0, 999, 1900);
            camNode->lookAt(Vector3(0, 1200, 0), Node::TransformSpace::TS_WORLD);
            camNode->attachObject  (cam);

            vp = ctx->getRenderWindow()->addViewport(cam);
            vp->setBackgroundColour(ColourValue(0, 0, 0.02f));

            cam->setAspectRatio(Real(vp->getActualWidth ()) / 
                                Real(vp->getActualHeight()));

            cam->setAutoAspectRatio(true);

            ///------------------|
            /// Manager.         |
            ///------------------:
            man = std::make_unique<OgreBites::CameraMan>(camNode);

            man->setStyle(OgreBites::CS_ORBIT); // или  CS_FREELOOK
            man->setTarget(camGoal);
            man->setTopSpeed(100);
            man->setFixedYaw(true);
            ctx->addInputListener(man.get());

            set2Start();
        }

        void set2Start()
        {   man->setYawPitchDist(Ogre::Degree(0), Ogre::Degree(10), 2000.0f);
        }
    };

    ///------------------------------------------------------------------------|
    /// Текст.
    ///--------------------------------------------------------------- TextTest:
    struct  TextTest : Glob
    {
        void setup()
        {   
            
        }
    };

    ///------------------------------------------------------------------------|
    /// Грунт.
    ///----------------------------------------------------------------- Ground:
    struct  Ground : Glob
    {       
        Ogre::Entity*  entity;
        SceneNode*     node  ;

        void setup()
        {   
            const char* name{"ground"};
            
            Plane plane(Vector3::UNIT_Y, 0);

            MeshManager::getSingleton().createPlane(
                name, RGN_DEFAULT,
                plane,
                2500, 2500, 20, 20,
                true,
                1, 5, 5,
                Vector3::UNIT_Z
            );

            Entity* groundEntity = scnMgr->createEntity(name);
            node = nodeBase->createChildSceneNode(name);
            node->attachObject(groundEntity);

            groundEntity->setCastShadows (false);
            groundEntity->setMaterialName("Examples/Rockwall");
        }
    };

    ///------------------------------------------------------------------------|
    /// Нидзя.
    ///------------------------------------------------------------------ Ninja:
    struct  Ninja : Glob
    {       
        Ogre::Entity*  entity;
        SceneNode*     node  ;

        void setup()
        {   
            auto X{ConfigGame::get().getWellW() / 2 + 100};

            entity = scnMgr->createEntity("ninja.mesh");
            entity ->setCastShadows(true);

            node = nodeBase->createChildSceneNode("Ninja");
            node->attachObject   (entity);
            node->yaw(Ogre::Degree (160));
            node->setPosition (X, 0, 100);
            node->setScale(1.5, 1.5, 1.5);
        }
    };

    ///------------------------------------------------------------------------|
    /// Sphere.
    ///----------------------------------------------------------------- Sphere:
    struct  Sphere : Glob
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

        void setup()
        {   for(unsigned i{}; i < color.size(); ++i)
            {   setup(i);
            }
        }

        void setup(const size_t n)
        {   
            std::string sn{std::to_string(n)};
            
            std::string name {"Sphere"}; name  += sn;
            std::string nameM{"matSph"}; nameM += sn;

            entity = scnMgr->createEntity(name, "sphere.mesh");
            node = nodeBase->createChildSceneNode();
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
    struct  Lights : Glob
    {       
        Ogre::Entity*  entity;

        void setup(SceneNode*  nodeUser)
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
                = nodeUser->createChildSceneNode();
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
                = nodeUser->createChildSceneNode();
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
                = nodeUser->createChildSceneNode();
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
    struct  Obj5File : Glob
    {       
        Ogre::Entity*  entity;
        SceneNode*     node  ;
        Ogre::MeshPtr  mesh  ;

        const char* name1{"myModel"};

        void setup(Ogre::String filename="model.obj")
        {   
            mesh = Ogre::MeshManager::getSingleton().load(
                filename,
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                Ogre::HardwareBuffer::HBU_STATIC,
                Ogre::HardwareBuffer::HBU_STATIC
            );

            entity = scnMgr->createEntity("myModel", filename);
            node   = nodeBase->createChildSceneNode();
            node->attachObject(entity);
            node->setPosition (0, 0, 0);
            node->setScale    (0.1f, 0.1f, 0.1f);
        }
    };

    ///------------------------------------------------------------------------|
    /// Ёлка.
    ///------------------------------------------------------------------ Ninja:
    struct  Tree : Glob
    {       
        Ogre::Entity*  entity;
        SceneNode*     node  ;

        void setup()
        {   
            entity = scnMgr->createEntity("12150_Christmas_Tree_V2_L2.mesh");
            entity ->setCastShadows(true);

            node = nodeBase->createChildSceneNode("Tree");
            node->attachObject(entity);

            //node->pitch(Ogre::Degree( -90));
            node->setOrientation(Ogre::Quaternion(
                Ogre::Degree(-90), Ogre::Vector3::UNIT_X));

            node->setPosition(-600, 0, -400);
            node->setScale   (7, 7, 7);
        }
    };

    ///------------------------------------------------------------------------|
    /// Цилиндр.
    ///---------------------------------------------------- SimpleBlackCylinder:
    struct  SimpleBlackCylinder : Glob
    {       SimpleBlackCylinder(
                const std::string name
            ,   SceneNode*  nodeParent
            ,   float radius =  50.0f
            ,   float height = 100.0f
            ,   int segments = 16)
            {   
                createCylinderMesh(name + "_Mesh", radius, height, segments);

                mEntity = scnMgr->createEntity(name, name + "_Mesh");
                mEntity-> setMaterialName("Examples/Black");

                mNode = nodeParent->createChildSceneNode(name + "_Node");
                mNode-> attachObject(mEntity);
            }
           ~SimpleBlackCylinder()
            {   if(mNode)
                {   Ogre::SceneManager* sceneMgr = mNode->getCreator();
                    sceneMgr->destroyEntity (mEntity);
                    sceneMgr->destroySceneNode(mNode);
                }
            }

        void setPosition(const Ogre::Vector3& pos)
        {   mNode->setPosition(pos);
        }
        Ogre::SceneNode* getNode()
        {   return mNode;
        }

    private:

        Ogre::SceneNode* mNode;
        Ogre::Entity*  mEntity;

        void createCylinderMesh(const Ogre::String& meshName,
                                float radius, float height, int segments)
        {   
            Ogre::ManualObject manual("TempCylinderCreator");

            manual.begin(
                "BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);

            float halfHeight = height / 2.0f;
            float angleStep = (2 * Ogre::Math::PI) / segments;

            // Боковая поверхность
            for(int i = 0; i <= segments; ++i)
            {   float angle = i * angleStep;
                float x = radius * cos(angle);
                float z = radius * sin(angle);

                manual.position(x,  halfHeight, z);
                manual.position(x, -halfHeight, z);

                if(i < segments)
                {   int base = i * 2;
                    manual.triangle(base,     base + 1, base + 2);
                    manual.triangle(base + 1, base + 3, base + 2);
                }
            }
            manual.end();

            manual.convertToMesh(
                meshName,
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        }
    };

    ///------------------------------------------------------------------------|
    /// Две ножки под корзину.
    ///--------------------------------------------------------- BlackCylinder2:
    struct  BlackCylinder2   : Glob
    {       BlackCylinder2() = default;
           ~BlackCylinder2()
            {   
            }

        SimpleBlackCylinder* m[2];

        void setup(SceneNode* nodeParent)
        {   
            m[0] = new SimpleBlackCylinder("Cylinder1", nodeParent);
            m[1] = new SimpleBlackCylinder("Cylinder2", nodeParent);

            m[0]->setPosition(Ogre::Vector3(-300,50,0));
            m[1]->setPosition(Ogre::Vector3( 300,50,0));
        }
    };
}


inline void PrintNodeHierarchy2(Ogre::SceneNode* root, int depth = 1)
{
    if (!root) return;
    
    std::string indent(depth * 3, '.');

    Ogre::Node::ChildNodeMap list = root->getChildren();

 /// if(!list.empty()) l(list.size())

    for(const auto& e : list)
    {   std::cout << indent << '|' << e->getName() << '\n';
        PrintNodeHierarchy2((Ogre::SceneNode*)e, ++depth);
    }
}

inline void PrintNodeHierarchy(Ogre::SceneNode* root)
{
    std::cout << "\n--- Scene Hierarchy ---\n";
    std::cout << root->getName() << '\n';
    PrintNodeHierarchy2(root);
    std::cout << std::endl;
}

#endif // PRIMITIVES_H
