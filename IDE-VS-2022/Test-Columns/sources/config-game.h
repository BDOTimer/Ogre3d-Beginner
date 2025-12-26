extern const char* const NAMEGAME;
///----------------------------------------------------------------------------|
/// "config-game.h"
///----------------------------------------------------------------------------:
#ifndef CONFIG_GAME_H
#define CONFIG_GAME_H
#include "box/debug.h"
#include "sound.h"


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
{       //ConfigGame()
        //{   W += (W+1)%2;
        //}

    const bool isDebug{false};

    const unsigned W{ 7}; /// Ширина (кол-во ячеек) корзины(Well).
    const unsigned H{11}; /// Выстота(кол-во ячеек) корзины(Well).
    const unsigned N{ 5}; /// Количество элементов(Gems) в фигуре(Column).
    const unsigned T{ 5}; /// Количество типов элементов(Gems) в фигуре(Column).

    ///------------------------------|
    /// Сколько нужно для match?     |
    ///------------------------------:
    const uint8_t AMOUNTMATCH{3};

    float sizeCell{100}; /// Размер ячейки корзины.

    float getWellW() const { return sizeCell * W; }
    float getWellH() const { return sizeCell * H; }
    int   getArrW () const { return W           ; } 
    int   getArrH () const { return H + N + 3   ; } 

    const Ogre::Vector3 positionWell{0.0f,100.0f,   0.0f};
    const Ogre::Vector3 positionUser{0.0f,  0.0f, 500.0f};

    ///-----------------------------------|
    /// Какие бывают жемчужины?           |
    ///-----------------------------------:
    using   DS = DescriptionGem;
    #define S Ogre::Vector3{0.5f, 0.5f, 0.5f} /// Скейлинг для шаров.
    #define C Ogre::Vector3{0.7f, 0.7f, 0.7f} /// Скейлинг для кубов.
    
    static constexpr std::array<DescriptionGem, 6> descriptionGems
    {   DS{0, "sphere.mesh", Ogre::ColourValue{ 1.0f , 0.5f , 0.0f}, S},
        DS{1, "sphere.mesh", Ogre::ColourValue{ 0.0f , 0.0f , 1.0f}, S},
        DS{2, "sphere.mesh", Ogre::ColourValue{ 0.0f , 1.0f , 0.0f}, S},
        DS{3, "cube.mesh"  , Ogre::ColourValue{ 0.0f , 0.5f , 1.0f}, C},
        DS{4, "cube.mesh"  , Ogre::ColourValue{ 1.0f , 0.0f , 0.5f}, C},
        DS{5, "cube.mesh"  , Ogre::ColourValue{ 0.5f , 0.1f , 0.1f}, C}
    };

    const char* decode4DescriptionGems(size_t id)
    {   ASSERT(id < descriptionGems.size())
        static const char* const m[]
        {   "Лимон",
            "Синий Мяч",
            "Зелёное яблоко",
            "Куб небесный",
            "Куб розовый"
        };
        return m[id];
    }

    #undef S
    #undef C
    ///-----------------------------------.

    ///-----------------------------------|
    /// Анимация(на логику не влияет).    |
    ///-----------------------------------:
    bool isGemAnimate{true};
    
    inline static ConfigGame* pConfigGame {nullptr};

    size_t rndGen() const
    {   const size_t ND{descriptionGems.size()};
        const int rndMax = T > ND ? ND : T;

        if(!isDebug) return rand() % rndMax;

        static constexpr const size_t R{6};
            
        ///-------------------------------|
        /// Детерминированный ряд id'ов.  |
        ///-------------------------------:
        std::array<size_t, R> r
        {   0, 1, 0,
            0, 0, 1
        };

        static size_t i{};
        return r[ i++%R ];
    }

    static ConfigGame& get()
    {   static ConfigGame cfgHard {0, 21, 15, 5, 6   };
        static ConfigGame cfgDemo {0, 11, 15, 5, 5   };
        static ConfigGame cfgDebg {1,  7,  7, 3, 3, 3};

        return pConfigGame ? *pConfigGame : cfgHard;
    }
};

inline std::ostream& operator<<(std::ostream& o, const ConfigGame& c)
{   o   << "Ширина корзины : " << c.getWellW() << '\n'
        << "Высота корзины : " << c.getWellH() << '\n'
    /// << "Размер элемента: " << c.sizeGems   << '\n'
        << "Размер ячейки  : " << c.sizeCell   << '\n';
    return o;
}


///----------------------------------------------------------------------------|
/// Events.
/// events.add("GameOver", [this](Args_t){ this->GameOver(); });
///--------------------------------------------------------------------- Events:
using  Args_t = const std::vector<float>&;
using  Foo_t  = void(Args_t);
struct Events
{   
    void add(const std::string& name, std::function<Foo_t> foo)
    {   m[name] = foo;
    }

    void call(const std::string& nameEvent, Args_t args={})
    {   
        if(auto it = m.find(nameEvent); it != m.end())
        {   auto&[name, foo] = *it;
            foo(args);
        }
        else ASSERTM(false,
             std::format("Незарегистрированное собыитие: \"{}\"", nameEvent))
    }

private:
    
    std::map<std::string, std::function<Foo_t>> m;
};

///---------|
/// Объект. |
///---------:
#define addEvent(n)    mdl::Glob::events.add(#n,[this](Args_t a){ this->n(a);});


///---------|
/// Models. |
///---------:
namespace mdl
{   
    using namespace Ogre;
    using namespace OgreBites;
    ///------------------------------------------------------------------------|
    /// Glob.
    ///------------------------------------------------------------------- Glob:
    struct  InspectorRoot;
    struct  UI;
    struct  Glob
    {   inline static InspectorRoot     * pInspectorRoot;
        inline static ApplicationContext* ctx           ;
        inline static InputListener     * pIListener    ;
        inline static Ogre::SceneManager* scnMgr        ;
        inline static Ogre::SceneNode   * nodeBase      ;
        inline static Ogre::SceneNode   * ninja         ;
        inline static                 UI* pUI           ;
        inline static float               deltaTime     ;
        inline static unsigned            cntGame{0}    ;
        inline static Events              events        ;
    };
}



#endif // CONFIG_GAME_H

