/// Push::cnt::002
///----------------------------------------------------------------------------|
/// C++20
/// Sources UTF-8
///----------------------------------------------------------------------------:
#include "inspector.h"

const char* const NAMEGAME{"Колоны-2025::ver-0.1"};

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


void test01()
{   
    int   b = -210;
    float a =  0.f -100.f -110.f;
    if(   b/3 != int(a/3)) l("error")

    l(b/3)
    l(int(a/3))
}


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

    return true;
}


///----------------------------------------------------------------------------|
/// Старт.
///----------------------------------------------------------------------- main:
int main(int argc, char **argv)
{
    std::system("chcp 65001>nul");

    srand((unsigned int)time(NULL));

    ///------------------|
    /// Запуск тестов.   |
    ///------------------:
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
