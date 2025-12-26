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

        unsigned idPlayer;
    
        void setup(SceneNode* node, unsigned id)
        {
            idPlayer = id;

            wallNode = node->createChildSceneNode();

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
                Nm(names[0]),
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                leftPlane,
                H, D+D, 1, 1, true, 1, 1.0f, 1.0f, Ogre::Vector3::UNIT_Z);
            
            Ogre::MeshManager::getSingleton().createPlane(
                Nm(names[1]),
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                rightPlane,
                H, D+D, 1, 1, true, 1, 1.0f, 1.0f, Ogre::Vector3::UNIT_Z);
            
            Ogre::MeshManager::getSingleton().createPlane(
                Nm(names[2]),
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                backPlane,
                W, H, 1, 1, true, 1, 1.0f, 1.0f, Ogre::Vector3::UNIT_Y);

            Ogre::MeshManager::getSingleton().createPlane(
                Nm(names[3]),
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                downPlane,
                W, D+D, 1, 1, true, 1, 1.f, 1.f, Ogre::Vector3::UNIT_Z);
        
            // Создаем Entity для каждой стенки
            walls[0] = scnMgr->createEntity(Nm(names[0]));
            walls[1] = scnMgr->createEntity(Nm(names[1]));
            walls[2] = scnMgr->createEntity(Nm(names[2]));
            walls[3] = scnMgr->createEntity(Nm(names[3]));
        
            for(int i = 0; i < 3; ++i)
            {
                walls[i]->setMaterialName(nameMat);
                walls[i]->setCastShadows (true);
            }

            walls[3]->setMaterialName(nameMat1);
            walls[3]->setCastShadows (true);

            // Позиционируем стенки
            Ogre::SceneNode* lNode = wallNode->createChildSceneNode(Nm(names[0]));
            lNode->attachObject(walls[0]);
            lNode->setPosition (-W2, H2, 0);
        
            Ogre::SceneNode* rNode = wallNode->createChildSceneNode(Nm(names[1]));
            rNode->attachObject(walls[1]);
            rNode->setPosition ( W2, H2, 0);
        
            Ogre::SceneNode* bNode = wallNode->createChildSceneNode(Nm(names[2]));
            bNode->attachObject(walls[2]);
            bNode->setPosition (0, H2, -D);

            Ogre::SceneNode* dNode = wallNode->createChildSceneNode(Nm(names[3]));
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
                if (meshMgr.resourceExists(Nm(name)))
                {   meshMgr.remove(Nm(name));
                }
            }
        }

    private:
        std::string Nm(std::string_view name)
        {   return std::format("{}{}", name, idPlayer);
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
            ,   repeatMatch(this)
            {   
                ///-----------------------------|
                /// Настраиваем физику.         |
                ///-----------------------------:
                collisions.fooLookWay = [this](int x, int y)
                {   return this->fooLookWay(x, y);
                };
            }

        void setup(SceneNode*  nodeB)
        {   node = nodeB->createChildSceneNode();
        }


    private:
        Figure& figure;

        const ConfigGame& cfg{ ConfigGame::get() };
        phs:: Collisions collisions;

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

        int statisticScore{};

        ///---------------------------------|
        /// true - Путь свободен!           |
        ///---------------------------------:
        bool fooLookWay(int w, int h)
        {   
            ASSERTM(h < (int)H, "Жемчуг вылез на крышу массива!")

            if( 0 > w || w >= (int)W  ||
                0 > h || h >= (int)H) return false;
            if( nullptr != gm [h][w]) return false;

            return true;
        };

        ///---------------------------------|
        /// Добавить фигуру в колодец.      |
        ///---------------------------------:
        bool add(Figure& fig)
        {   
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

                if(isGameOver = !addOne(gem, posGemI, posGemF); isGameOver) 
                {   return false;
                }
            }

            findMatchGems();

            ///----------------|
            /// Дебаг.         |
            ///----------------:
            /// l(allocator.size()) ln(gm)

            return true;
        }

        bool addOne(Gem& gem, const Ogre::Vector3i& posGemI,
                              const Ogre::Vector3 & posGemF)
        {               
            unsigned x = *(posGemI.ptr() + 0);
            unsigned y = *(posGemI.ptr() + 1);

            auto& cell = gm[y][x];

            gem.pos2gm[0] = x;
            gem.pos2gm[1] = y;

            ///------------------------------|
            /// Ячейка занята.               |
            ///------------------------------:
            if(nullptr != cell)
            {   
            /// std::cout << "ERROR-[Физика]: Ячейка занята! ---> "; l(posGemI)
                std::cout << '\n' <<
                    "|-----------------------------|\n"
                    "|      Чувак, геймовер!       |\n"
                    "|-----------------------------.\n\n";

                Sound::get().stop();

                Glob::events.call("UserOver", {(float)figure.id});
                return  false;
            }

            allocator.emplace_back(GemData());
            allocator.back()     =        gem;
            allocator.back().igm = --allocator.end();

            cell = &allocator.back();
            cell->setupGravitate(new phs::Stepper(posGemF.y), &collisions);
            cell->pwellLogic = this;

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
        /// Ищем где совпало. [box.cpp]     |
        ///---------------------------------:
        int                  findMatchGems();

        ///---------------------------------|
        /// RepeatMatch                     |
        ///---------------------------------:
        struct  RepeatMatch
        {       RepeatMatch(WellLogic* wl) : wl(wl) {}

            void tick(bool t) /// Вызвается на сигнале покоя в колодце.
            {   if  ((tt ^ t))
                {     tt = t;
                      wl->findMatchGems();

                    //l1("RepeatMatch:tick(.) run findMatchGems().\n")
                }
            }

        private:
            bool tt{false};
            WellLogic*  wl;
        }repeatMatch;

        void update()
        {   
            bool isActive{false};

            for(auto it = allocator.begin(); it !=  allocator.end(); )
            {
                ASSERT(it->isLive())

                isActive |= it->update();

                if(   !it->isLive())
                {   gm[it->pos2gm[1]][it->pos2gm[0]] = nullptr;
                       it = allocator.erase(it);
                }
                else ++it;
            }

            repeatMatch.tick(isActive);
        }

    public:
        void setGem(GemData* gem)
        {
            const int x = gem->pos2gm[0];
            const int y = gem->pos2gm[1];

            gem->pos2gm[1] -= 1;

            ASSERT(gem->pos2gm[1] >= 0)
            ASSERT(gm[gem->pos2gm[1]][x] == nullptr)

            gm[gem->pos2gm[1]][x] = gem;
            gm[y             ][x] = nullptr;
        }

        friend struct Well;
    };


    ///------------------------------------------------------------------------|
    /// Корзина.
    ///------------------------------------------------------------------- Well:
    struct  Well   : Glob
    {       Well(unsigned id) : idPlayer(id), figure(id), logic(figure)
            {   
            }
           ~Well()
            {
            }

        Cube2       cube2        ;
        unsigned    idPlayer     ;
        SceneNode*  node{nullptr};
        Figure      figure       ;
        Well3Wall   well3Wall    ;

        ///----------------------------|
        /// Содержимое корзины!        |
        ///----------------------------:
        WellLogic  logic;

    private:

        void setup(SceneNode* nodeGame)
        {
            ///------------------------|
            /// Нод корзины!           |
            ///------------------------:
            node = nodeGame->createChildSceneNode(
                std::format("Well{}", idPlayer)
            );

            figure   .setup(node, &logic.collisions); 
            well3Wall.setup(node, idPlayer);
            logic    .setup(node);

            const auto& cfgPOS{ConfigGame::get().positionWell};

            node->setPosition(cfgPOS);
            cube2.setup      (node  );

            ///------------------------|
            /// Делегируем.            |
            ///------------------------:
            figure.delegate4Well = [this](Figure* figure){ logic.add(*figure);};
        }

        ///-------------------------------------------|
        /// Обработка клавиш.                         |
        ///-------------------------------------------:
        bool keyPressed(const KeyboardEvent& evt)
        {   if(logic.isGameOver) return false;

            switch(evt.keysym.sym)
            {   case '5': ln(logic.gm) break; /// Дебаг.
                default:;
            }
            figure.keyPressed(evt);

            return true;
        }

        void update()
        {   if(logic.isGameOver) return;
            
            figure.update();
            logic .update();
        }

        friend struct InspectorRoot;
        friend struct   Game1Player;
        friend struct   Game2Player;
        friend struct         IGame;
    };
}


#endif // WELL_H

