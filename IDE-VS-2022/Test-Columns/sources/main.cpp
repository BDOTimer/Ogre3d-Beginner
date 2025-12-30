/// ...
///----------------------------------------------------------------------------|
/// C++20
/// Sources UTF-8
///----------------------------------------------------------------------------:
#include "inspector.h"

const char* const NAMEGAME{"Game-2025::Demo-rc1::ver-0.5"};

///----------------------------------------------------------------------------|
/// Application.
///---------------------------------------------------------------- Application:
struct  Application
{       Application()
        {   
            setup  ();
        }

    virtual ~Application() {}

    mdl::InspectorRoot inspectorRoot;

    void setup()
    {   inspectorRoot.initApp();
        inspectorRoot.getRoot()->startRendering();
        inspectorRoot.closeApp();
    }
};


///----------------------------------------------------------------------------|
/// Тесты.
///---------------------------------------------------------------------- tests:
#pragma warning(push, 0)
#pragma warning(disable: 4702)
bool tests()
{   
    /// 
    return false;
    
    /// myl::Step2Distance ::test();
    /// myl::Step2DistanceB::test();
    
    /// test01();

    /// myl::testPhysics();

    /// phs::tests();

    __assume(0);
    return true;
}
#pragma warning(pop)

///----------------------------------------------------------------------------|
/// Старт.
///----------------------------------------------------------------------- main:
int main([[maybe_unused]] int    argc,
         [[maybe_unused]] char** argv)
{
    std::system("chcp 65001>nul");

    //xmain();

    unsigned int RANDSEED{false ? (unsigned int)time(NULL) : 2025}; 
    srand       (RANDSEED); l(RANDSEED)

    ///-----------------|
    /// Запуск тестов.  |
    ///-----------------:
    if(tests()) return 0;

    std::cout 
        << "|----------------------------------------------|\n"
        << std::format("|            {:32}  |\n", NAMEGAME)
        << "|----------------------------------------------|\n"
        << ConfigGame::get()  << "\n\n";

    try
    {
        ///------------------------------------------------|
        /// Отключаем логи в MeshManager.                  |
        ///------------------------------------------------:
        Ogre::LogManager logMgr;
        if(const bool isLog = true)
        {   logMgr.createLog("", false, false, false);  // Пустой лог
        }

        Application app;
    }
    catch (const std::exception& e)
    {
        std::cerr << "ERRORFATAL: " << e.what() << '\n';
        return 1;
    }

    std::cout << "\nTest-Columns is FINISHED!\n\n"; return 0;
}
