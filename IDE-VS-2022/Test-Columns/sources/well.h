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
    /// Три стенки корзины + дно.
    ///-------------------------------------------------------------- Well3Wall:
    struct  Well3Wall : Glob
    {      ~Well3Wall  ()
            {   destroy();
            }

        Ogre::SceneNode* wallNode{nullptr};
        Ogre::Entity*    walls[4];

        const char* nameMat1{"Ogre/Skin1"};
        const char* nameMat2{"drbunsen_glasses"};
        const char* nameMat {"Glass/WellWallsSimple"};
    /// const char* nameMat {"Examples/Rockwall"};

        std::array<const char*, 4> names
        {   "LeftWall" ,
            "RightWall",
            "BackWall" ,
            "DownWall"
        };
    
        void setup(SceneNode*  node)
        {
            wallNode = node->createChildSceneNode("3Walls");

            const auto& cfg{ConfigGame::get()};

            // Размеры корзины:
            float W  = cfg.get().getWellW();
            float H  = cfg.get().getWellH();
            float D  = cfg.get().sizeCell;

            const auto W2 = W/2;
            const auto H2 = H/2;

            Ogre::Plane leftPlane (Ogre::Vector3::UNIT_X, 0);
            Ogre::Plane rightPlane(Ogre::Vector3::NEGATIVE_UNIT_X, 0);
            Ogre::Plane backPlane (Ogre::Vector3::UNIT_Z, 0);
            Ogre::Plane downPlane (Ogre::Vector3::UNIT_Y, 0);

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

            // Позиционируем стенки
            Ogre::SceneNode* lNode = wallNode->createChildSceneNode(names[0]);
            lNode->attachObject(walls[0]);
            lNode->setPosition (-W2, H2, 0);
        
            Ogre::SceneNode* rNode = wallNode->createChildSceneNode(names[1]);
            rNode->attachObject(walls[1]);
            rNode->setPosition ( W2, H2, 0);
        
            Ogre::SceneNode* bNode = wallNode->createChildSceneNode(names[2]);
            bNode->attachObject(walls[2]);
            bNode->setPosition (0, H2, -D);

            Ogre::SceneNode* dNode = wallNode->createChildSceneNode(names[3]);
            dNode->attachObject(walls[3]);
            dNode->setPosition (0, 0, 0);
        }

        void destroy()
        {
            if (!wallNode) return;

            Ogre::SceneManager* ScnMgr = wallNode->getCreator();

            for (int i = 0; i < 4; ++i)
            {
                if (walls[i])
                {
                    walls[i]->detachFromParent();
                    ScnMgr  ->destroyEntity(walls[i]);
                    walls[i] = nullptr;
                }
            }
    
            Ogre::MeshManager& meshMgr = Ogre::MeshManager::getSingleton();
            for (const auto& name : names)
            {
                if (meshMgr.resourceExists(name))
                {   meshMgr.remove(name);
                }
            }
        }
    };


    ///------------------------------------------------------------------------|
    /// Корзина - ЛОГИКА.
    ///-------------------------------------------------------------- WellLogic:
    struct  WellLogic   : Glob
    {       WellLogic(Figure& f) :
                figure       (f)
            ,   cfg(ConfigGame::get())
            ,   W  (    cfg.getArrW())
            ,   H  (    cfg.getArrH())
            ,   gm(H, std::vector<GemData*>(W, nullptr))
            {   
                ///-----------------------------|
                /// Настраиваем физику.         |
                ///-----------------------------:
                phs::Collisions::get().fooLookWay = [this](int x, int y)
                {   return this->fooLookWay(x, y);
                };
            }

        std::function<void(int)> SetScore;
        void setDelegateSetScore(std::function<void(int)> dlg)
        {   SetScore = dlg;
            SetScore   (0);
        }

    private:
        Figure& figure;

        const      ConfigGame& cfg       {     ConfigGame::get()};
        const phs::Collisions& collisions{phs::Collisions::get()};

        size_t W;
        size_t H;

        SceneNode*  node;

        bool isGameOver{false};

        ///----------------------------------|
        /// Владелец данных.                 |
        ///----------------------------------:
        std::list<GemData>          allocator;

        ///----------------------------------|
        /// Зеркало корзины.                 |
        ///----------------------------------:
        std::vector<std::vector<GemData*>> gm;

        ///----------------------------------|
        /// Жемчуг для удаления.             |
        ///----------------------------------:
        GemsMatch                   gemsMatch;

        void setup(SceneNode*  nodeWell)
        {   
           node = nodeWell->createChildSceneNode("WellLogic");
        }

        ///---------------------------------|
        /// true - Путь свободен!           |
        ///---------------------------------:
        bool fooLookWay(int w, int h)
        {   
            ASSERTM(h < (int)H, "Жемчуг вылез на крышу массива!")

            if( 0 > w || w >= (int)W  ||
                0 > h || h >= (int)H) return false;
                
            if( nullptr != gm[h][w])  return false;

            return true;
        };

        bool add(Figure& fig)///-///////////////////////////////////////////////
        {   
            /// return true;

            const Ogre::Vector3& posFig{fig.node->getPosition()};

            for(auto& gem : fig.gems)
            {   
                const Ogre::Vector3& posGem{gem.node->getPosition()};

                const Ogre::Vector3 posGemF
                {   std::ceilf(posFig.x + posGem.x),
                    std::ceilf(posFig.y + posGem.y),
                    std::ceilf(posFig.z + posGem.z)
                };

                const Ogre::Vector3i     posGemI
                {   collisions.getIndex3(
                        {posGemF.x, posGemF.y - 50.f, posGemF.z})
                };

                //LN//////////////////////////////////////////////////////////-?
                //l(posGemF)
                //l(posGemI)
                

                if(posGemI[0] < 0)
                {   std::cout << "ERROR-[Физика]: Фигура за левым бортом!\n";
                    return false;
                }

                if(posGemI[0] >= W)
                {   std::cout << "ERROR-[Физика]: Фигура за правым бортом!\n";
                    return false;
                }

                if(posGemI[1] < 0)
                {   std::cout << "ERROR-[Физика]: Фигура ушла под пол!\n";
                    return false;
                }

                if(!add(gem, posGemI, posGemF)) break;
            }

            findMatchGems();

            ///----------------|
            /// Дебаг.         |
            ///----------------:
            /// l(allocator.size()) ln(gm)

            return true;
        }

        bool add(Gem& gem, const Ogre::Vector3i& posGemI,
                           const Ogre::Vector3 & posGemF)
        {               
            unsigned x = *(posGemI.ptr() + 0);
            unsigned y = *(posGemI.ptr() + 1);

            auto& cell = gm[y][x];

            ///------------------------------|
            /// Ячейка занята.               |
            ///------------------------------:
            if(nullptr != cell)
            {   std::cout << "ERROR-[Физика]: Ячейка занята! ---> "; l(posGemI)
                std::cout << '\n' <<
                    "|-----------------------------|\n"
                    "|      Чувак, геймовер!       |\n"
                    "|-----------------------------.\n\n";

            /// figure.setVisibleGems(false);
                fooGameOver();
                return  false;
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

            return true;
        }

        ///---------------------------------|
        /// Повесь сюда делегат!            |
        ///---------------------------------:
        std::function<void()> fooGameOver{[](){}};

        ///---------------------------------|
        /// Ищем где совпало. [box.cpp]     |
        ///---------------------------------:
        void                 findMatchGems();

        void update()
        {   for    (auto& r : gm)
            {   for(auto& e : r)
                {   if(nullptr != e) e->update();
                }
            }
        }

        friend struct Well;
    };


    ///------------------------------------------------------------------------|
    /// Корзина.
    ///------------------------------------------------------------------- Well:
    struct  Well   : Glob
    {       Well() : logic(figure)
            {   
                infoNewGame2Console();
            }
           ~Well()
            {
            }
        
        SceneNode*  node{nullptr};
        Figure      figure;
        Well3Wall   well3Wall;

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
            node = nodeBase->createChildSceneNode("Well");

            figure   .setup(node);
            well3Wall.setup(node);
            logic    .setup(node);

            const auto& cfgPOS{ConfigGame::get().positionWell};

            node->setPosition(cfgPOS);

            ///------------------------|
            /// Делегируем.            |
            ///------------------------:
            figure.delegate4Well = [this](Figure* figure){ logic.add(*figure);};
        }

        void setDelegateGameOver(std::function<void()> foo)
        {   logic.fooGameOver = foo;
        }

        ///-------------------------------------------|
        /// Обработка клавиш.                         |
        ///-------------------------------------------:
        bool keyPressed(const KeyboardEvent& evt)
        {   
            switch(evt.keysym.sym)
            {   case '1': ln(logic.gm) break; /// Дебаг.
                default:;
            }
            return figure.keyPressed(evt);
        }

        void update()
        {   figure.update();
            logic .update();
        }

        void destroy()
        {   
        }

        void infoNewGame2Console() const;
    
        friend struct InspectorRoot;
    };
}


#endif // WELL_H

