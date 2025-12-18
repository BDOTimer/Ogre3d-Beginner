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
        Ogre::Entity*    walls[4];

        const char* nameMat1{"Ogre/Skin1"};
        const char* nameMat2{"drbunsen_glasses"};
        const char* nameMat {"Glass/WellWallsSimple"};
    /// const char* nameMat {"Examples/Rockwall"};
    
        void setup(SceneNode*  node)
        {
            wallNode = node->createChildSceneNode("3Walls");

            Ogre::Plane leftPlane (Ogre::Vector3::UNIT_X, 0);
            Ogre::Plane rightPlane(Ogre::Vector3::NEGATIVE_UNIT_X, 0);
            Ogre::Plane backPlane (Ogre::Vector3::UNIT_Z, 0);
            Ogre::Plane downPlane (Ogre::Vector3::UNIT_Y, 0);

            const char* names[]
            {   "LeftWall" ,
                "RightWall",
                "BackWall" ,
                "DownWall"
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

            Ogre::MeshManager::getSingleton().createPlane(
                names[3],
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                downPlane,
                W, D+D, 1, 1, true, 1, 1.f, 1.f, Ogre::Vector3::UNIT_Z);
        
            // Создаем Entity для каждой стенки
            walls[0] = scnMgr->createEntity(names[0]);
            walls[1] = scnMgr->createEntity(names[1]);
            walls[2] = scnMgr->createEntity(names[2]);
            walls[3] = scnMgr->createEntity(names[3]);
        
            for(int i = 0; i < 3; ++i)
            {
                walls[i]->setMaterialName(nameMat);
                walls[i]->setCastShadows (true);
            }

            walls[3]->setMaterialName(nameMat1);
            walls[3]->setCastShadows (true);

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

            Ogre::SceneNode* downNode = wallNode->createChildSceneNode();
            downNode->attachObject(walls[3]);
            downNode->setPosition (0, 0, 0);
        }
    };


    ///------------------------------------------------------------------------|
    /// Корзина - ЛОГИКА.
    ///-------------------------------------------------------------- WellLogic:
    struct  WellLogic   : Base
    {       WellLogic() : 
                W(ConfigGame::get().W)
            ,   H(ConfigGame::get().H + ConfigGame::get().N)
            ,   gm(H, std::vector<GemData*>(W, nullptr))
            {   
            }

    private:
        SceneNode*  node;
        const ConfigGame& cfg{ConfigGame::get()};

        size_t W;
        size_t H;

        const myl::Indexer& indexer{myl::Indexer::get()};

        ///----------------------------------|
        /// Владелец данных.                 |
        ///----------------------------------:
        std::list<GemData>          allocator;

        ///----------------------------------|
        /// Зеркало корзины.                 |
        ///----------------------------------:
        std::vector<std::vector<GemData*>> gm;

        void setup(SceneNode*  nodeWell)
        {   node = nodeWell->createChildSceneNode();
        }

        void add(Figure& figure)///-////////////////////////////////////////////
        {   
            /// return;
            
            std::vector<Gem>& gems = figure.gems;

            const Ogre::Vector3& posFig{figure.node->getPosition()};

            for(auto& gem : gems)
            {   
                const Ogre::Vector3& posGem{gem.node->getPosition()};

                const Ogre::Vector3 posGemF
                {   std::ceilf(posFig.x + posGem.x),
                    std::ceilf(posFig.y + posGem.y),
                    std::ceilf(posFig.z + posGem.z)
                };

                const Ogre::Vector3i&& posGemI
                {   indexer.getIndex3(posGemF)
                };

                /// LN
                /// l(posGemI)

                if(posGemI[0] < 0)
                {   std::cout << "ERROR-[Физика]: Фигура за левым бортом!\n";
                    return;
                }

                if(posGemI[0] >= W)
                {   std::cout << "ERROR-[Физика]: Фигура за правым бортом!\n";
                    return;
                }

                add(gem, posGemI, posGemF);
            }
        }

        void add(Gem& gem, const Ogre::Vector3i& posGemI,
                           const Ogre::Vector3 & posGemF)
        {               
            unsigned x = *(posGemI.ptr() + 0);
            unsigned y = *(posGemI.ptr() + 1);

            auto& cell = gm[y][x];

            ///------------------------------|
            /// Ячейка занята.               |
            ///------------------------------:
            if(nullptr != cell)
            {   std::cout << "ERROR-[Физика]: Ячейка занята! ---> ";
                l_(posGemI) l(posGemF)
                return;
            }

            allocator.emplace_back(GemData());
            allocator.back() = gem;
            allocator.back().igm = --allocator.end();

            cell = &allocator.back();

            gem.reset();
            
            /// Debug:
            ///------------------------------|
            /// Что лежит в этой ячейке.     |
            ///------------------------------:
            /// l(cell->igm->id)
            /// l(cell->id)

            ///------------------------------|
            /// Отвязать камень от фигуры.   |
            ///------------------------------:
            cell->deLink();

            ///------------------------------|
            /// Присоединить камень к well.  |
            ///------------------------------:
            node->addChild(cell->node);

            cell->node->setPosition(posGemF);
        }

        void update(float deltaTime)
        {   for    (auto& r : gm)
            {   for(auto& e : r)
                {   if(nullptr != e) e->update(deltaTime);
                }
            }
        }

        friend struct Well;
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

        ///----------------------------|
        /// Содержимое корзины!        |
        ///----------------------------:
        WellLogic  logic;

    private:
        void setup()
        {   
            ///------------------------|
            /// Нод корзины!           |
            ///------------------------:
            node = nodeBase->createChildSceneNode();
            figure.setup(scnMgr, node);

            well3Wall.setup(node);
            logic    .setup(node);

            const auto& cfgPOS{ConfigGame::get().positionWell};

            node->setPosition(cfgPOS);

            ///------------------------|
            /// Делегируем.            |
            ///------------------------:
            figure.delegate4Well = [this](Figure* figure){ logic.add(*figure);};
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
            logic .update(deltaTime);
        }
    
        friend struct InspectorRoot;
    };
}


#endif // WELL_H

