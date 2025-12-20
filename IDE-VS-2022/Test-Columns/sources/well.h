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
    struct  Well3Wall : Base
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
    struct  WellLogic   : Base
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
                myl::Indexer::get().fooLookWay = [this](myl::Indexer::EDIR d)
                {   return this->fooLookWay(d);
                };
            }

        std::function<void(int)> SetScore;
        void setDelegateSetScore(std::function<void(int)> dlg)
        {   SetScore = dlg;
        }

    private:
        Figure& figure;

        const ConfigGame& cfg{ConfigGame::get()};

        size_t W;
        size_t H;

        SceneNode*  node;

        const myl::Indexer& indexer{myl::Indexer::get()};

        bool isGameOver{false};

        ///----------------------------------|
        /// Владелец данных.                 |
        ///----------------------------------:
        std::list<GemData>          allocator;

        ///----------------------------------|
        /// Зеркало корзины.                 |
        ///----------------------------------:
        std::vector<std::vector<GemData*>> gm;

        void setup(SceneNode*  nodeWell)
        {   
           node = nodeWell->createChildSceneNode("WellLogic");
        }

        ///---------------------------------|
        /// Путь свободен от gem?           |
        /// Сначала проверить на стенки!    |
        ///---------------------------------:
        bool fooLookWay(const myl::Indexer::EDIR dir)
        {   
            const Ogre::Vector3& posFig{figure.node->getPosition()};

            for(auto& gem : figure.gems)
            {   
                const Ogre::Vector3& posGem{gem.node->getPosition()};
                const Ogre::Vector3& posGemF
                {   std::ceilf(posFig.x + posGem.x),
                    std::ceilf(posFig.y + posGem.y),
                    std::ceilf(posFig.z + posGem.z)
                };

                const Vector3i vi{ indexer.getIndex3(posGemF) };

                switch(dir)
                {   case myl::Indexer::ELEFT:
                    {   if( int x = vi[0]-1; x < 0 ||
                            nullptr != gm[vi[1]][x]) return false;
                        break;
                    }
                    case myl::Indexer::ERIGHT:
                    {   if( int x = vi[0]+1; x >= W ||
                            nullptr != gm[vi[1]][x]) return false;
                        break;
                    }
                    case myl::Indexer::EDOWN:
                    {   if( int y = vi[1]-1; y < 0 ||
                            nullptr != gm[y][vi[0]]) return false;
                        break;
                    }
                    case myl::Indexer::EUP:
                    {   if( int y = vi[1]+1; y >= H ||
                            nullptr != gm[y][vi[0]]) return false;
                        break;
                    }
                    default:
                        if( vi[0] < 0 || vi[1] >= H ||
                            nullptr != gm[vi[1]][vi[0]]) return false;
                        ;
                }
            }
            return true;
        };

        bool add(Figure& fig)///-////////////////////////////////////////////
        {   
            /// return;

            const Ogre::Vector3& posFig{fig.node->getPosition()};

            for(auto& gem : fig.gems)
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
                    return false;
                }

                if(posGemI[0] >= W)
                {   std::cout << "ERROR-[Физика]: Фигура за правым бортом!\n";
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

        void update()
        {   for    (auto& r : gm)
            {   for(auto& e : r)
                {   if(nullptr != e) e->update();
                }
            }
        }

        ///---------------------------------|
        /// Повесь сюда делегат!            |
        ///---------------------------------:
        std::function<void()> fooGameOver{[](){}};

        ///---------------------------------|
        /// Ищем где совпало.               |
        ///---------------------------------:
        std::vector<igm_t> findMatchGems()
        {   
            std::vector<igm_t> matchGems; matchGems.reserve(128);

            for       (auto& r : gm)
            {   for   (auto& e : r )
                {   if(nullptr != e)
                    {   e->match.reset();
                    }
                }
            }

            for(    size_t h{}; h < H; ++h)
            {   for(size_t w{}; w < W; ++w)
                {   
                    ///------------------------|
                    /// В фокусе только 1 раз! |
                    ///------------------------:
                    const auto& a{gm[h][w]};

                    if(nullptr == a)
                    {   continue;
                    }
                    
                    ///------------------------|
                    /// Горизонталь.           |
                    ///------------------------:
                    if(size_t i = w + 1; i < W )
                    {   const auto& b{gm[h][i]};
                        
                        if(nullptr != b && a->id == b->id)
                        {   b->match.addLG(a->match.getLG());
                        }
                    }

                    ///------------------------|
                    /// Вертикаль.             |
                    ///------------------------:
                    if(size_t j = h + 1; j < H )
                    {   const auto& b{gm[j][w]};
                        
                        if(nullptr != b && a->id == b->id)
                        {   b->match.addLV(a->match.getLV());
                        }
                    }

                    ///------------------------|
                    /// Диагональ "Плюс".      |
                    ///------------------------:
                    if(size_t j = h + 1, i = w + 1; j < H && i < W )
                    {   const auto& b{gm[j][i]};
                        
                        if(nullptr != b && a->id == b->id)
                        {   b->match.addL1(a->match.getL1());
                        }
                    }

                    ///------------------------|
                    /// Диагональ "Минус".     |
                    ///------------------------:
                    if(size_t j = h - 1, i = w - 1; j < H && i < W )
                    {   const auto& b{gm[j][i]};
                        
                        if(nullptr != b && a->id == b->id)
                        {   b->match.addL5(a->match.getL5());
                        }
                    }
                }
            }

            for       (const auto& r : gm)
            {   for   (const auto& e : r )
                {   if(nullptr != e)
                    {   if(e->match.isMatch())
                        {
                            matchGems.push_back(e->igm);
                        }
                    }
                }
            }

            ///----------------|
            /// Дебаг.         |
            ///----------------:
            if(!matchGems.empty()) {l(matchGems.size())ln(matchGems)}

            SetScore(int(matchGems.size()));

            return matchGems;
        }

        friend struct Well;
    };


    ///------------------------------------------------------------------------|
    /// Корзина.
    ///------------------------------------------------------------------- Well:
    struct  Well   : Base
    {       Well() : logic(figure)
            {   std::cout << '\n' << std::format("{}\n{}{:2}{}\n{}\n",
                "|----------------------------------|",
                "|     Новая игра  - ", Base::cntGame," создана!    |",
                "|----------------------------------|");
            }
           ~Well()
            {   //destroyAll();
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

        void changeFigure    ()
        {   figure.reGenerate();
        }

        void setDelegate(std::function<void()> foo)
        {   logic.fooGameOver = foo;
        }

        ///-------------------------------------------|
        /// Обработка клавиш.                         |
        ///-------------------------------------------:
        bool keyPressed(const KeyboardEvent& evt)
        {   return figure.keyPressed(evt);
        }

        void update()
        {   figure.update();
            logic .update();
        }

        void destroyAll()
        {   if( node != nullptr )
            {   node -> destroyAllChildrenAndObjects();
                node -> destroyAllObjects();
                
                Ogre::Node* parent = node->getParent();
                parent->removeChild(node);
            }
        }
    
        friend struct InspectorRoot;
    };
}


#endif // WELL_H

