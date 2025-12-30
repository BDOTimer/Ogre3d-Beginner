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

            cam->setAutoAspectRatio(true);

            cam->setNearClipDistance(1.0f);
            cam->setFarClipDistance(5000.0f);

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

            addEvent(set2Start);
            addEvent(setCam   );
        }

        void set2Start([[maybe_unused]]Args_t a = {})
        {   man->setYawPitchDist(Ogre::Degree(0), Ogre::Degree(10), val);
        }

        void setCam(Args_t d)
        {   val = d[0];
            man->setYawPitchDist(Ogre::Degree(0), Ogre::Degree(10), val);
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

            const ColourValue colourValue0(1.0f, 1.0f, 1.0f);
            const ColourValue colourValue1(0.5f, 0.5f, 0.5f);
            const ColourValue colourValue2(0.2f, 0.2f, 0.2f);

            ///------------|
            /// SpotLight  |
            ///------------:
            if(0)
            {
                Light* spotLight = scnMgr->createLight("SpotLight");

                spotLight->setDiffuseColour (colourValue1);
                spotLight->setSpecularColour(colourValue2);

                spotLight->setType(Light::LT_SPOTLIGHT);

                SceneNode* spotLightNode
                    = scnMgr->getRootSceneNode()->createChildSceneNode();
                spotLightNode->attachObject(spotLight);
                spotLightNode->setDirection(0, -1, -1);
                spotLightNode->setPosition (Vector3(200, 200, 0));

                spotLight->setSpotlightRange(Degree(35), Degree(50));
            }

            ///------------|
            /// DirLight   |
            ///------------:
            if(1)
            {
                Light* directionalLight = scnMgr->createLight("DirectionalLight");
                directionalLight->setType(Light::LT_DIRECTIONAL);

                directionalLight->setDiffuseColour (ColourValue(colourValue0));
                directionalLight->setSpecularColour(ColourValue(colourValue2));

                SceneNode* directionalLightNode
                    = scnMgr->getRootSceneNode()->createChildSceneNode();
                directionalLightNode->attachObject(directionalLight);
                directionalLightNode->setDirection(Vector3(0, -1, -1));
            }

            ///------------|
            /// PointLight |
            ///------------:
            if(0)
            {
                Light* pointLight = scnMgr->createLight("PointLight");
                pointLight->setType(Light::LT_POINT);

                pointLight->setDiffuseColour (colourValue1);
                pointLight->setSpecularColour(colourValue2);

                SceneNode* pointLightNode
                    = nodeUser->createChildSceneNode();
                pointLightNode->attachObject(pointLight);
                pointLightNode->setPosition(Vector3(0, 150, 250));
            }
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
            entity = scnMgr->createEntity("ninja.mesh");
            entity ->setCastShadows(true);

            node = nodeBase->createChildSceneNode("Ninja");
            node->attachObject   (entity);
            node->yaw(Ogre::Degree (180));
            node->setPosition (0, 0, 0);
            node->setScale(3.0, 3.0, 3.0);
        }
    };

    ///------------------------------------------------------------------------|
    /// Модель.
    ///------------------------------------------------------------------ Model:
    struct  Model : Glob
    {       
        Ogre::Entity*  entity;
        SceneNode*     node  {nullptr};

        void setup(const std::string& nameMesh)
        {   entity = scnMgr->createEntity(nameMesh + ".mesh");
            entity ->setCastShadows(true);

            node = nodeBase->createChildSceneNode(nameMesh);
            node->attachObject(entity);
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

            node->setPosition(-760, 0, -300);
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


    ///------------------------------------------------------------------------|
    /// Куб.
    ///------------------------------------------------------------------- Cube:
    struct  Cube : Glob
    {       
        Ogre::Entity*     entity;
        SceneNode*        node  ;
        Ogre::MaterialPtr mat   ;

        void setup(SceneNode*  par, const char* mesh = "cube")
        {   
            auto X{ConfigGame::get().getWellW() / 2 + 100};

            createMaterial();

            entity = scnMgr->createEntity(std::format("{}.mesh", mesh));
            entity->setMaterial(mat);
            entity ->setCastShadows(true);

            node = par->createChildSceneNode();
            node->attachObject        (entity);
            node->setPosition    (X,   0, 100);
            node->setScale     (1.f, 1.f, 2.f);
        }

        void createMaterial()
        {   
            auto name{std::format("cube{}", 0)};

            mat = Ogre::MaterialManager::getSingleton().getByName(name);

            if(mat != nullptr) return;

            mat = MaterialManager::getSingleton().create(
                name,
                ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
            );

            Ogre::Pass* const p = mat->getTechnique(0)->getPass(0);
                        p->setDiffuse (ColourValue(0.05f, 0.05f, 0.05f));
                        p->setAmbient (ColourValue(0.3f, 0.1f, 0.0f));
                        p->setSpecular(ColourValue(1.0f, 1.0f, 1.0f));
                        p->setShininess(64.0);
        }
    };

    ///------------------------------------------------------------------------|
    /// Две ножки под корзину.
    ///------------------------------------------------------------------ Cube2:
    struct  Cube2   : Glob
    {       Cube2() = default;
           ~Cube2()
            {   
            }

        Cube m[2];

        void setup(SceneNode*  node)
        {
            auto X{ConfigGame::get().getWellW() / 2 - 100};

            m[0].setup(node);
            m[1].setup(node);

            m[0].node->setPosition(Ogre::Vector3(-X,-50,0));
            m[1].node->setPosition(Ogre::Vector3( X,-50,0));
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

namespace mdl
{
    struct Decor : Glob
    {   Ninja    ninja;
        Tree      tree;
        Model    tree2;
        Model    tree3;
        Ground  ground;

        void setup()
        {   
            
            tree .setup();
            
        /// tree2        .setup  ("Christmas_Tree");
        /// tree2.node  ->setPosition(200, 0, -200);

            tree3        .setup     ("SnowyPineTree");
            tree3.node  ->setPosition(1000, 0, -1000);
            tree3.node  ->setScale(20,20,20);

            ninja .setup();
            ground.setup();

            //tree.entity = createTreeWithShadowsTest("Christmas_Tree.mesh");
        }

        static Entity* createTreeWithShadowsTest(const String& nameMesh)
        {
            // 1. Загрузите оригинальный меш
            Entity* entTree = Glob::scnMgr->createEntity(nameMesh);
        
            // 2. Принудительно установите большой bounding box
            MeshPtr mesh = entTree->getMesh();
            mesh->_setBounds(
                AxisAlignedBox(-60, 0, -60, 60, 60, 60),
                false
            );
        
            // 3. Создайте простой материал с тенями
            MaterialPtr shadowMat = MaterialManager::getSingleton().create(
                "TreeShadowFixMaterial",
                ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
            );
        
            Technique* tech = shadowMat->createTechnique();
            Pass* pass = tech->createPass();
            pass->setLightingEnabled(true);
            pass->setAmbient(0.5f, 0.5f, 0.5f);
            pass->setDiffuse(0.8f, 0.8f, 0.8f, 1.f);
        
            for(unsigned short i = 0; i < entTree->getNumSubEntities(); i++)
            {
                entTree->getSubEntity(i)->setMaterialName("TreeShadowFixMaterial");
            }
        
            // 5. Включите тени
            entTree->setCastShadows(true);

            auto node = nodeBase->createChildSceneNode(nameMesh);
            node->attachObject(entTree);

            node->setPosition(200, 0, -200);
        
            return entTree;
        }
    };

    inline Decor decor;
}

namespace mdl
{
    struct  Cursor : Glob
    {       Cursor()
            {   
            }

        void setup()
        {
            createCustomCursor();

            manT.oel = Ogre::OverlayManager::getSingleton()
                     .getOverlayElement("CustomCursor");
        }

        bool mouseMoved(const OgreBites::MouseMotionEvent& evt)
        {   manT.on();
            manT.oel->setPosition((float)evt.x, (float)evt.y);
            return true;
        }

        void tick(){ manT.tick(); }

    private:
        Ogre::Overlay* mCursorOverlay;
        
        void createCustomCursor();
        void toggleCursor      ();

        struct
        {   
            Ogre::OverlayElement* oel;

            ///--------------------------------|
            /// Вызывать на метрономе.         |
            ///--------------------------------:
            void tick()
            {   timeStart += 1;
                if(timeStart > timeMax)
                {   off();
                }
            }

            void off(){ if( oel->isVisible()) oel->hide(); timeStart = 0;}
            void on (){ if(!oel->isVisible()) oel->show(); }

        private:
            int timeMax  {3};
            int timeStart{0};
        }manT;
    };
}




#endif // PRIMITIVES_H
