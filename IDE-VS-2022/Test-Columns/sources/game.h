///----------------------------------------------------------------------------|
/// "game.h"
///----------------------------------------------------------------------------:
#ifndef GAME_H
#define GAME_H
#include "well.h"
#include "ui.h"

///---------|
/// Modules.|
///---------:
namespace mdl
{   
    using namespace Ogre;
    using namespace OgreBites;

    ///------------------------------------------------------------------------|
    /// Интерфейс игры.
    ///------------------------------------------------------------------ IGame:
    struct  IGame : Glob
    {      
        virtual ~IGame(){}

        ///---------------------------------------|
        /// Инициализация.                        |
        ///---------------------------------------:
        virtual void setup() = 0;

        ///---------------------------------------|
        /// Динамика.                             |
        ///---------------------------------------:
        virtual void update() = 0;

        ///---------------------------------------|
        /// Обработка клавиш.                     |
        ///---------------------------------------:
        virtual bool keyPressed(const KeyboardEvent& evt) = 0;

        ///---------------------------------------|
        /// Получить имя.                         |
        ///---------------------------------------:
        virtual const std::string& getNameUse(unsigned) const = 0;
        
    protected:
        Well* createWell(Ogre::SceneNode* nodeGame,
                         unsigned         idPlayer, 
                         Well*      well = nullptr)
        {   
            well = new Well(idPlayer);
            well->setup    (nodeGame);
            return well;
        }

        static void safeRemoveNode(Ogre::SceneNode* node)
        {
            while (node->numAttachedObjects())
            {   node->detachObject(node->getAttachedObject(0));
            }
            while (node->numChildren())
            {   safeRemoveNode(static_cast<Ogre::SceneNode*>(node->getChild(0)));
            }
            node->getCreator()->destroySceneNode(node);
        }

        void destroyNode(const std::string& name)
        {   if (Glob::scnMgr->hasSceneNode(name)) 
            {   
               //nodePl->removeAndDestroyAllChildren();
               //scnMgr->destroySceneNode(nodePl);

                Ogre::SceneNode* node = scnMgr->getSceneNode(name);
                scnMgr->destroySceneNode(node);
            }
            else ASSERT(false)
        }

        void infoNewGame2Console(std::string_view) const;
    };


    ///------------------------------------------------------------------------|
    /// Заглушка.
    ///---------------------------------------------------------------- GameNul:
    struct  GameNul : IGame
    {   void setup     (){}
        void update    (){}
        bool keyPressed(const KeyboardEvent&){ return false; }
        const std::string& getNameUse(unsigned) const
        {   return z;
        }
        const std::string z{""};
    };


    ///------------------------------------------------------------------------|
    /// Игра с одним игроком.
    ///------------------------------------------------------------ Game1Player:
    struct  Game1Player : IGame
    {       Game1Player()
            {   
                infoNewGame2Console("Game1Player::setup()\n");
                setup     ();
                setupDecor();
            }
           ~Game1Player()
            {   
                safeRemoveNode(well->node);
                delete(well);

                destroyNode(nameUser);
            }

        std::string             nameUser{"Game1Player"};
        Ogre::SceneNode* nodePl{nullptr};
        mdl ::Well*      well  {nullptr};

        ///---------------------------------------|
        /// Инициализация.                        |
        ///---------------------------------------:
        void setup()
        {   
            nodePl = nodeBase->createChildSceneNode(nameUser);
            well   = createWell(nodePl, 0, well); ASSERT(well != nullptr)
        }

        ///---------------------------------------|
        /// Динамика.                             |
        ///---------------------------------------:
        void update()
        {   well->update();
        }

        ///---------------------------------------|
        /// Обработка клавиш.                     |
        ///---------------------------------------:
        bool keyPressed(const KeyboardEvent& evt)
        {   return well->keyPressed(evt);
        }

        const std::string& getNameUse(unsigned) const
        {   return nameUser;
        }
        
    private:
        void setupDecor()
        {   
            const ConfigGame& cfg{ConfigGame::get()};

            auto X{cfg.getWellW() / 2 + 100};
            decor.ninja.node->setPosition(X, 0, 100);

            Ogre::Degree     angle(160);
            Ogre::Quaternion rot(angle, Ogre::Vector3::UNIT_Y);
            decor.ninja.node->setOrientation(rot);

            decor.ground.node->setScale(3.0, 3.0, 3.0);
            decor.tree.node->setPosition(-cfg.getWellW()/2 -200, 0, -600);

            events.call("setCam", {2200});
        }

        friend struct GameMain;
    };


    ///------------------------------------------------------------------------|
    /// Игра с двумя игроками.
    ///------------------------------------------------------------ Game2Player:
    struct  Game2Player : IGame
    {       Game2Player()
            {   
                infoNewGame2Console("Game2Player::setup()\n");
                setup     ();
                setupDecor();
            }
           ~Game2Player()
            {   
                unsigned id{};
                for(auto& w : wells)
                {   safeRemoveNode(w->node);
                    delete(w);
                    destroyNode(names[id++]);
                }
            }

        std::array<std::string, 2> names
        {   "Game2Player0",
            "Game2Player1"
        };
        std::array<Ogre::SceneNode*, 2>          nodePl;
        std::array<mdl::Well*, 2> wells{nullptr, nullptr};

        ///---------------------------------------|
        /// Инициализация.                        |
        ///---------------------------------------:
        void setup()
        {   
            unsigned   id{};
            for(auto&  w : wells)
            {   nodePl[id] = nodeBase->createChildSceneNode(names[id]);
                       w   = createWell(nodePl[id], id, w);
                       id++;
            }

            const auto x{ConfigGame::get().getWellW() / 2 + 100};

            nodePl[0]->translate( x, 0, 0, Ogre::Node::TS_LOCAL);
            nodePl[1]->translate(-x, 0, 0, Ogre::Node::TS_LOCAL);

        }

        ///---------------------------------------|
        /// Динамика.                             |
        ///---------------------------------------:
        void update()
        {   for(auto w : wells) w->update();
        }

        ///---------------------------------------|
        /// Обработка клавиш.                     |
        ///---------------------------------------:
        bool keyPressed(const KeyboardEvent& evt)
        {
            bool                b{false};
            for(auto w : wells) b |= w->keyPressed(evt);
            return              b;
        }

        const std::string& getNameUse(unsigned i) const
        {   return names[i];
        }
        
    private:
        void setupDecor()
        {   
            decor.ninja.node->setPosition(0, 0, 100);

            Ogre::Degree     angle(180);
            Ogre::Quaternion rot(angle, Ogre::Vector3::UNIT_Y);
            decor.ninja.node->setOrientation(rot);

            decor.ground.node->setScale(3.0, 1.0, 3.0);

            nodePl[0]->yaw(Ogre::Degree(-10));
            nodePl[1]->yaw(Ogre::Degree( 10));

            decor.tree.node->setPosition(0, 0, -600);
            decor.tree.node->setScale(8, 12, 8);

            events.call("setCam", {3300});
        }

        friend struct GameMain;
    };

    ///------------------------------------------------------------------------|
    /// Фабрика игор...
    ///--------------------------------------------------------------- GameMain:
    struct  GameMain   : Glob
    {       GameMain() = default;
           ~GameMain()
            {   if(&gameNul != game) delete game;
            }

        GameMain           (const GameMain&) = delete;
        GameMain& operator=(const GameMain&) = delete;

        IGame* game{&gameNul};

        ///---------------------------------------|
        /// Инициализация.                        |
        ///---------------------------------------:
        void setup(unsigned nPlayers)
        {   
            ++Glob::cntGame;
            chtGameOver = 0;

            static ConfigGame cfgHard {0, 21, 15, 5, 6   };
            static ConfigGame cfgDemo {0, 11, 15, 5, 5   };

            switch(nPlayers)
            {   case 1: ConfigGame::get().pConfigGame = &cfgDemo; break;
                case 2: ConfigGame::get().pConfigGame = &cfgHard; break;
            }

            ///-----------------------------------|
            /// Регистрация обработчиков событий. |
            ///-----------------------------------:
            addEvent(UserOver);

            this->amountPlayers = nPlayers;

            if(&gameNul != game) delete game;

            switch    (nPlayers)
            {   case  1: game = new Game1Player(); break;
                case  2: game = new Game2Player(); break;
                default:;
            }
        }

        ///---------------------------------------|
        /// Динамика.                             |
        ///---------------------------------------:
        void update()
        {   game->update();
        }

        ///---------------------------------------|
        /// Обработка клавиш.                     |
        ///---------------------------------------:
        bool keyPressed(const KeyboardEvent& evt)
        {   return game->keyPressed(evt);
        }
        
        private:
            unsigned amountPlayers{};
            GameNul  gameNul;

        unsigned chtGameOver;
        void UserOver(Args_t a)
        {   
            unsigned i{unsigned(a[0])};
            std::cout << std::format(
                "Игрок {} закочил ...\n", game->getNameUse(i));
            
            if(++chtGameOver == amountPlayers)
            {   events.call("gameOver");
            }
        }
    };

}

#endif // GAME_H

