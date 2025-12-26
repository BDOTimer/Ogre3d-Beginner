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
        
    protected:

        Well* createWell(Ogre::SceneNode* nodeGame,
                         unsigned         idPlayer, 
                         Well*      well = nullptr)
        {   
            if (well)
            {   safeRemoveNode(well->node);
                delete(well);
            }
            
            well = new Well(idPlayer);

            well->logic.setDelegateSetScore(
                [this](int score)
                {   Glob::pUI->score->set(score);
                }
            );
            well->setup(nodeGame);
            return   well;
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

        void infoNewGame2Console(std::string_view) const;
    };


    ///------------------------------------------------------------------------|
    /// Заглушка.
    ///---------------------------------------------------------------- GameNul:
    struct  GameNul : IGame
    {   void setup     (){}
        void update    (){}
        bool keyPressed(const KeyboardEvent&){ return false; }
    };


    ///------------------------------------------------------------------------|
    /// Игра с одним игроком.
    ///------------------------------------------------------------ Game1Player:
    struct  Game1Player : IGame
    {       Game1Player()
            {   setup  ();
                infoNewGame2Console("Game1Player::setup()\n");

                auto X{ConfigGame::get().getWellW() / 2 + 100};
                Glob::ninja->setPosition(X, 0, 100);

                Ogre::Degree     angle(160);
                Ogre::Quaternion rot(angle, Ogre::Vector3::UNIT_Y);
                Glob::ninja->setOrientation(rot);
            }
           ~Game1Player()
            {   
            }

        Ogre::SceneNode*   nodePl;
        mdl::Well*  well{nullptr};

        ///---------------------------------------|
        /// Инициализация.                        |
        ///---------------------------------------:
        void setup()
        {   nodePl = nodeBase->createChildSceneNode("Game1Player");
            well = createWell(nodePl, 0, well); ASSERT(well != nullptr)
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
        
    private:
        
        friend struct GameMain;
    };


    ///------------------------------------------------------------------------|
    /// Игра с двумя игроками.
    ///------------------------------------------------------------ Game2Player:
    struct  Game2Player : IGame
    {       Game2Player()
            {   setup  ();
                infoNewGame2Console("Game2Player::setup()\n");
                Glob::ninja->setPosition(0, 0, 100);

                Ogre::Degree     angle(180);
                Ogre::Quaternion rot(angle, Ogre::Vector3::UNIT_Y);
                Glob::ninja->setOrientation(rot);
            }
           ~Game2Player()
            {   
            }

        std::array<Ogre::SceneNode*, 2>          nodePl;
        std::array<mdl::Well*, 2> wells{nullptr, nullptr};

        ///---------------------------------------|
        /// Инициализация.                        |
        ///---------------------------------------:
        void setup()
        {   nodePl[0] = nodeBase->createChildSceneNode("Game2Player0");
            nodePl[1] = nodeBase->createChildSceneNode("Game2Player1");
            unsigned id{};
            for(auto& w : wells)
            {   w = createWell(nodePl[id], id, w); id++;
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
        
    private:

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

        unsigned chtGameOver{};
        void UserOver(Args_t)
        {   if(++chtGameOver == amountPlayers)
            {   events.call("gameOver");
            }
        }
    };

}

#endif // GAME_H

