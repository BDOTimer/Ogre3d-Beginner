///----------------------------------------------------------------------------|
/// "game.h"
///----------------------------------------------------------------------------:
#ifndef GAME_H
#define GAME_H
#include "well.h"
#include "ui.h"

///---------|
/// Models. |
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
        Well* createWell(unsigned idPlayer, Well* well)
        {   
            if (well)
            {   safeRemoveNode(well->node);
                delete(well);
            }
            
            well = new Well();
            well-> idPlayer = idPlayer;

            well->setDelegateGameOver(
                [this]()
                {   WARNING(false, "TODO: not init...")
                }
            );

            well->logic.setDelegateSetScore(
                [this](int score)
                {   Glob::pUI->score->set(score);
                }
            );
            well->setup();
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
                std::cout << "Game1Player::setup()\n";
            }
           ~Game1Player()
            {   
            }

        mdl::Well* well{nullptr};

        ///---------------------------------------|
        /// Инициализация.                        |
        ///---------------------------------------:
        void setup()
        {   well = createWell(0, well); ASSERT(well != nullptr)
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

    };


    ///------------------------------------------------------------------------|
    /// Игра с двумя игроками.
    ///------------------------------------------------------------ Game2Player:
    struct  Game2Player : IGame
    {       Game2Player()
            {   setup  ();
                std::cout << "Game2Player::setup()\n";
            }

        std::array<mdl::Well*, 2> wells{nullptr, nullptr};

        ///---------------------------------------|
        /// Инициализация.                        |
        ///---------------------------------------:
        void setup()
        {   
            unsigned id{};
            for(auto& w : wells)
            {   w = createWell(id++, w);

                w->setDelegateGameOver(
                    [this]()
                    {   this->fooGameOver(); 
                    }
                );
            }
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
        void fooGameOver(unsigned idPlayer = 0)
        {   std::cout << std::format(
                "Игрок {} закончил игру!\n", idPlayer + 1);
        }
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
            this->amountPlayers = nPlayers;

            if(&gameNul != game) delete game;

            switch    (nPlayers)
            {   case  1: game = new Game1Player; break;
                case  2: game = new Game2Player; break;
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
    };

}

#endif // GAME_H

