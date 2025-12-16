///----------------------------------------------------------------------------|
/// "config-game.h"
///----------------------------------------------------------------------------:
#ifndef CONFIG_GAME_H
#define CONFIG_GAME_H
#include "box/debug.h"


///----------------------------------------------------------------------------|
/// Описание для набора Gems(жемчужин).
/// В конфиге описан набор(descriptionGems) из 5 штук.
///------------------------------------------------------------- DescriptionGem:
struct DescriptionGem
{   unsigned             id;
    const char*    nameMesh;
    Ogre::ColourValue color;
    Ogre::Vector3     scale;
};


///----------------------------------------------------------------------------|
/// Конфигуратор игры Columns.
///----------------------------------------------------------------- ConfigGame:
struct  ConfigGame
{       ConfigGame()
        {   W += (W+1)%2;
        }

    unsigned W{ 7}; /// Ширина (кол-во ячеек) корзины(Well).
    unsigned H{12}; /// Выстота(кол-во ячеек) корзины(Well).
    unsigned N{ 4}; /// Количество элементов(Gems) в фигуре(Column).

    float sizeCell{100}; /// Размер ячейки корзины.
    float sizeGems{ 20}; /// Если это шар, то тогда это Диаметр...

    float getWellW() const { return sizeCell * W; }
    float getWellH() const { return sizeCell * H; }

    const Ogre::Vector3 positionWell{0.0f, 0.0f,   0.0f};
    const Ogre::Vector3 positionUser{0.0f, 0.0f, 500.0f};

    ///-----------------------------------|
    /// Какие бывают жемчужины?           |
    ///-----------------------------------:
    using   DS = DescriptionGem;
    #define S Ogre::Vector3{0.5f, 0.5f, 0.5f}
    #define C Ogre::Vector3{0.7f, 0.7f, 0.7f}
    
    static constexpr std::array<DescriptionGem, 5> descriptionGems
    {   DS{0, "sphere.mesh", Ogre::ColourValue{ 1.0 , 0.5f , 0.0}, S},
        DS{4, "sphere.mesh", Ogre::ColourValue{ 0.0 , 0.0f , 1.0}, S},
        DS{2, "sphere.mesh", Ogre::ColourValue{ 0.0 , 1.0f , 0.0}, S},
        DS{3, "cube.mesh"  , Ogre::ColourValue{ 0.0 , 0.5f , 1.0}, C},
        DS{1, "cube.mesh"  , Ogre::ColourValue{ 1.0 , 0.0f , 0.5}, C}
    };
    #undef S
    #undef C
    ///-----------------------------------.

    ///-----------------------------------|
    /// Анимация(на логику не влияет).    |
    ///-----------------------------------:
    bool isGemRotating{true};
        
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


///---------|
/// Models. |
///---------:
namespace mdl
{   
    using namespace Ogre;
    using namespace OgreBites;
    ///------------------------------------------------------------------------|
    /// Base.
    ///------------------------------------------------------------------- Base:
    struct  InspectorRoot;
    struct  Base
    {   inline static InspectorRoot     * pInspectorRoot   { nullptr };
        inline static ApplicationContext* ctx              { nullptr };
        inline static OgreBites::InputListener*  pIListener{ nullptr };
        inline static Ogre::SceneManager* scnMgr           { nullptr };
        inline static Ogre::SceneNode   * nodeBase         { nullptr };
        inline static float               deltaTime;
    };
}

#endif // CONFIG_GAME_H

