///----------------------------------------------------------------------------|
/// Test-Columns-2025
///----------------------------------------------------------------------------:
#include "inspector.h"


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
    return false;
    
    /// myl::Step2Distance ::test();
    /// myl::Step2DistanceB::test();

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

    std::cout << "Привет, Я Test-Columns!\n\n"
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
