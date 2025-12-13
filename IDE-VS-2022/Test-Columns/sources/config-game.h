///----------------------------------------------------------------------------|
/// "config-game.h"
///----------------------------------------------------------------------------:
#ifndef CONFIG_GAME_H
#define CONFIG_GAME_H
#include "box/debug.h"

struct  ConfigGame
{       ConfigGame()
        {   
        }

    const unsigned W{ 6}; /// Ширина  корзины(Well).
    const unsigned H{15}; /// Выстота корзины(Well).
    const unsigned N{15}; /// Количество (Gems) в фигуре(Column).

    const float sizeCell  {20};
    const float gemsRadius{20};


        
    static ConfigGame& get()
    {   static ConfigGame cfg; return cfg;
    }
};

#endif // CONFIG_GAME_H

