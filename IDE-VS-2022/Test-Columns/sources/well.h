///----------------------------------------------------------------------------|
/// "well.h"
///----------------------------------------------------------------------------:
#ifndef WELL_H
#define WELL_H
#include "figure.h"


///---------|
/// Modules.|
///---------:
namespace mdl
{   
    ///------------------------------------------------------------------------|
    /// Три стенки корзины.
    ///-------------------------------------------------------------- Well3Wall:
    struct Well3Wall : Base
    {
        Ogre::SceneNode* wallNode;
        Ogre::Entity*    walls[3];

    /// const char* nameMat{"glass/WellWalls"};
    /// const char* nameMat{"glass/WellWallsAdvanced"};
        const char* nameMat{"Glass/WellWallsSimple"};
    /// const char* nameMat{"Examples/Rockwall"};
    
        void setup(SceneNode*  node)
        {
            wallNode = node->createChildSceneNode("3Walls");

            Ogre::Plane leftPlane (Ogre::Vector3::UNIT_X, 0);
            Ogre::Plane rightPlane(Ogre::Vector3::NEGATIVE_UNIT_X, 0);
            Ogre::Plane backPlane (Ogre::Vector3::UNIT_Z, 0);

            const char* names[]
            {   "LeftWall" ,
                "RightWall",
                "BackWall"
            };

            const auto& cfg{ConfigGame::get()};

            // Размеры корзины:
            float W  = cfg.get().getWellW();
            float H  = cfg.get().getWellH();
            float D  = cfg.get().sizeCell;
            float T  = 1.0f;
        
            Ogre::MeshManager::getSingleton().createPlane(
                names[0],
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                leftPlane,
                H, D+D, 1, 1, true, 1, 1.0f, 1.0f, Ogre::Vector3::UNIT_Z);
            
            Ogre::MeshManager::getSingleton().createPlane(
                names[1],
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                rightPlane,
                H, D+D, 1, 1, true, 1, 1.0f, 1.0f, Ogre::Vector3::UNIT_Z);
            
            Ogre::MeshManager::getSingleton().createPlane(
                names[2],
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                backPlane,
                W, H, 1, 1, true, 1, 1.0f, 1.0f, Ogre::Vector3::UNIT_Y);
        
            // Создаем Entity для каждой стенки
            walls[0] = scnMgr->createEntity(names[0]);
            walls[1] = scnMgr->createEntity(names[1]);
            walls[2] = scnMgr->createEntity(names[2]);
        
            for(int i = 0; i < 3; ++i)
            {
                walls[i]->setMaterialName(nameMat);
                walls[i]->setCastShadows (true);
            }

            const auto W2 = W/2;
            const auto H2 = H/2;
        
            // Позиционируем стенки
            Ogre::SceneNode* leftNode = wallNode->createChildSceneNode();
            leftNode->attachObject(walls[0]);
            leftNode->setPosition (-W2, H2, 0);
        
            Ogre::SceneNode* rightNode = wallNode->createChildSceneNode();
            rightNode->attachObject(walls[1]);
            rightNode->setPosition ( W2, H2, 0);
        
            Ogre::SceneNode* backNode = wallNode->createChildSceneNode();
            backNode->attachObject(walls[2]);
            backNode->setPosition (0, H2, -D);
            //backNode->pitch       (Ogre::Degree(90));
        }
    };

    ///------------------------------------------------------------------------|
    /// Корзина.
    ///------------------------------------------------------------------- Well:
    struct  Well : Base
    {       Well()
            {   
            }
        
        SceneNode*  node;
        Well3Wall   well3Wall;
        Figure      figure;

    private:
        void setup()
        {   
            ///------------------------|
            /// Нод корзины!           |
            ///------------------------:
            node = nodeBase->createChildSceneNode();
            figure.setup(scnMgr, node);

            well3Wall.setup(node);
        }

        void changeFigure    ()
        {   figure.reGenerate();
        }

        ///-------------------------------------------|
        /// Обработка клавиш.                         |
        ///-------------------------------------------:
        bool keyPressed(const KeyboardEvent& evt)
        {   return figure.keyPressed(evt);
        }

        void update(float deltaTime)
        {   figure.update(deltaTime);
        }
    
        friend struct InspectorRoot;
    };
}


#endif // WELL_H

