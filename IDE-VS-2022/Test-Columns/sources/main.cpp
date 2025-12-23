/// ...
///----------------------------------------------------------------------------|
/// C++20
/// Sources UTF-8
///----------------------------------------------------------------------------:
#include "inspector.h"

const char* const NAMEGAME{"Колоны-2025::Demo-rc1::ver-0.4"};

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
bool tests()
{   
    /// 
    return false;
    
    /// myl::Step2Distance ::test();
    /// myl::Step2DistanceB::test();
    
    /// test01();

    /// myl::testPhysics();

    /// phs::tests();

    return true;
}


///----------------------------------------------------------------------------|
/// Старт.
///----------------------------------------------------------------------- main:
int main([[maybe_unused]] int    argc,
         [[maybe_unused]] char** argv)
{
    std::system("chcp 65001>nul");

    unsigned int RANDSEED{false ? (unsigned int)time(NULL) : 2025}; 
    srand       (RANDSEED); l(RANDSEED)

    ConfigGame  cfgDemo {11, 15, 4};
    ConfigGame  cfgDebug{ 7, 15, 4};
    ConfigGame::get().configGame = &cfgDemo;

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
        logMgr.createLog("", false, false, false);  // Пустой лог

        Application app;
    }
    catch (const std::exception& e)
    {
        std::cerr << "ERRORFATAL: " << e.what() << '\n';
        return 1;
    }

    std::cout << "\nTest-Columns is FINISHED!\n\n"; return 0;
}
