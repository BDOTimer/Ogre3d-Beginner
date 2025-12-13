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

    unsigned W{ 6}; /// Ширина (кол-во ячеек) корзины(Well).
    unsigned H{15}; /// Выстота(кол-во ячеек) корзины(Well).
    unsigned N{ 4}; /// Количество элементов(Gems) в фигуре(Column).

    unsigned amountGems{7}; /// Количество вариантов Gems.

    float sizeCell{20}; /// Размер ячейки корзины.
    float sizeGems{20}; /// Если это шар, то тогда это Диаметр...

    float getWellW() const { return sizeCell * W; }
    float getWellH() const { return sizeCell * H; }

    const Ogre::Vector3 positionWell{0.0f, 0.0f,   0.0f};
    const Ogre::Vector3 positionUser{0.0f, 0.0f, 500.0f};
        
    static ConfigGame& get()
    {   static ConfigGame cfg; return cfg;
    }
};

inline std::ostream& operator<<(std::ostream& o, const ConfigGame& c)
{   o   << "Ширина корзины : " << c.getWellW() << '\n'
        << "Высота корзины : " << c.getWellH() << '\n'
        << "Размер элемента: " << c.sizeGems   << '\n'
        << "Размер ячейки  : " << c.sizeCell   << '\n';
    return o;
}

#endif // CONFIG_GAME_H

