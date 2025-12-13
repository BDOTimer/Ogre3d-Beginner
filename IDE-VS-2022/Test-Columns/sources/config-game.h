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
    const unsigned N{ 4}; /// Количество элементов(Gems) в фигуре(Column).

    const float sizeCell{20}; /// Размер ячейки корзины.
    const float sizeGems{20}; /// Если это шар, то тогда это Диаметр...


        
    static ConfigGame& get()
    {   static ConfigGame cfg; return cfg;
    }
};

#endif // CONFIG_GAME_H

