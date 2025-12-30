///----------------------------------------------------------------------------|
/// "inspector.h"
///----------------------------------------------------------------------------:
#ifndef INSPECTOR_H
#define INSPECTOR_H
#include "intro.h"
#include "game.h"
#include "sky.h"

#include <windows.h>


///---------|
/// Models. |
///---------:
namespace mdl
{  
    ///------------------------------------------------------------------------|
    /// InspectorRoot.("Test-Columns-2025")
    ///---------------------------------------------------------- InspectorRoot:
    struct  InspectorRoot
            :   Glob
            ,   OgreBites::ApplicationContext
            ,   OgreBites::InputListener
            ,   OgreBites::TrayListener

    {       InspectorRoot( ) : 
                OgreBites::ApplicationContext("")
            ,   autoCotrollerSin( camera.val,
                                  [this](){ this->camera.set2Start(); })
            {
            }

        OgreBites::TrayManager* trayMgr{nullptr};

        Ogre::Root*           root;
        Ogre::SceneManager* scnMgr;
        SceneNode*        nodeBase;
        SceneNode*        nodeUser;

        Cursor              cursor;
        Camera              camera;
        Lights              lights;

        UI                      ui;
        Effects            effects;
    /// SkyDome*           skyDome;
        Intro                intro;
        Sky                    sky;

        ///---------------------------------------|
        /// Игра...                               |
        ///---------------------------------------:
        GameMain games;

        Ogre::RTShader::ShaderGenerator* shadergen;

        ConfigGame& cfg{ConfigGame::get()};

        AutoCotrollerSin  autoCotrollerSin;

    protected:

        ///---------------------------------------|
        /// Свой путь к "ogre.h" ---> (box.cpp)   |
        ///---------------------------------------:
        void createRoot() override;

        Ogre::Quaternion orientationWorldStart;

        void setup() override
        {   
            OgreBites::ApplicationContext::setup();
            addInputListener      (this);

            trayMgr = new OgreBites::TrayManager(
                "UI", getRenderWindow(), this
            );

            Glob::pInspectorRoot = this;
            Glob::ctx            = this;
            Glob::pIListener     = this;

            root   = getRoot();
            scnMgr = root->createSceneManager();
            scnMgr->setAmbientLight(ColourValue(0, 0, 0));
            scnMgr->setShadowTechnique(
               ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);

            nodeBase = scnMgr->getRootSceneNode()->createChildSceneNode("Glob");
            nodeUser = scnMgr->getRootSceneNode()->createChildSceneNode();

            Glob::scnMgr   = scnMgr;
            Glob::nodeBase = nodeBase;
            Glob::pUI      = &ui;

            addResourcePath();

            ////////////////////////////////////////////////////////////////////
            Ogre::RenderWindow* window = getRenderWindow();
            setWindowIcon      (window);
            ////////////////////////////////////////////////////////////////////

            #if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            ShowCursor(FALSE);
            #endif

            ///-----------------------------------|
            /// Регистрация обработчиков событий. |
            ///-----------------------------------:
            addEvent(gameOver);

        /// root->loadPlugin("OgreAssimp");

            shadergen = RTShader::ShaderGenerator::getSingletonPtr();
            shadergen-> addSceneManager(scnMgr);

           //scnMgr->setSkyBox(true, "Examples/StormySkyBox8");

            cursor   .setup();
            camera   .setup(nodeUser);
            lights   .setup(camera.camNode);
            decor    .setup();
            ui       .setup(trayMgr);
            effects  .setup();
        /// intro    .setup();
            sky      .setup();

            ///-----------------------------------|
            /// Сохраняем ориентацию мира.        |
            ///-----------------------------------:
            orientationWorldStart = nodeBase->getInitialOrientation();
        }

        ///---------------------------------------|
        /// Обработка клавиш.                     |
        ///---------------------------------------:
        bool keyPressed(const KeyboardEvent& evt)
        {   
            if(this->isGameOver)
            switch(evt.keysym.sym)
            {   case OgreBites::SDLK_ESCAPE:
                    trayMgr->showOkDialog("!!!", "Exit");
                    return true;
                case '1': startGame(1); return true;
                case '2': startGame(2); return true;
                default:;
            }

            static unsigned iSky{};

            switch(evt.keysym.sym)
            {
                case OgreBites::SDLK_ESCAPE:
                    gameOver({});
                    return true;
                case OgreBites::SDLK_F8:
                /// PrintNodeHierarchy(Glob::nodeBase);
                    break;
                case OgreBites::SDLK_F4:
                    sky.toggle();
                    break;
                case OgreBites::SDLK_F5:
                    isSpeedRotWold = isSpeedRotWold ? 0 :  speedRotWold;
                    break;
                case OgreBites::SDLK_F6:
                    isSpeedRotWold = isSpeedRotWold ? 0 : -speedRotWold;
                    break;
                case OgreBites::SDLK_SPACE:
                case OgreBites::SDLK_PAUSE:
                case 'p':
                    isPause = !isPause;
                    break;
                case '0': camera2StartGame(); 
                    break;
                case OgreBites::SDLK_F12:
    				ui.trayMgr->areFrameStatsVisible()
                  ? ui.trayMgr->hideFrameStats()
    			  : ui.trayMgr->showFrameStats(TrayLocation::TL_BOTTOMLEFT);
    				break;
                default: // l(evt.keysym.sym)
                    ;
            }

            bool    b{false};
                    b |= ui   .keyPressed(evt);
            if(!isPause || !isGameOver)
            {       b |= games.keyPressed(evt);
            }
            return  b;
        }

        bool mousePressed(
            [[maybe_unused]]
            const OgreBites::MouseButtonEvent& evt)
        {   return true;
        }

        float    accumulatedTime { 0};  // Накопленное время.
        float    intervalTime    { 1};  // Интервал (1 секунда)
        float    isSpeedRotWold  { 0};  // Нет вращения Мира.
        const float speedRotWold {30};  // Нет вращения Мира.
        bool     isPause      {false};
        bool     isGameOver   {true };
        float    seconds          {0};

        ///---------------------------------------|
        /// Тут крутятся фреймы.                  |
        ///---------------------------------------:
        void frameRendered(const Ogre::FrameEvent& evt) override
        {
            Glob::deltaTime = evt.timeSinceLastFrame;

            ///-----------------------------------|
            /// Метроном.                         |
            ///-----------------------------------:
            accumulatedTime += evt.timeSinceLastFrame;
       
            if(accumulatedTime >= intervalTime)
            {
            /// effects.tickSecond();
                
                accumulatedTime -= intervalTime;

                seconds++;

                effects.update(seconds);
                cursor .tick();
            }

            ///-----------------------------------|
            /// FPS.                              |
            ///-----------------------------------:
            /// myl::Fps::get().update(deltaTime);

            if(isPause || isGameOver)
            {   sky.update();
            }
            else
            {   games.update();
            }

            if(isSpeedRotWold != 0)
            {   nodeBase->yaw(Ogre::Degree(deltaTime * isSpeedRotWold));
                autoCotrollerSin.update();
            }
        }

        void buttonHit(OgreBites::Button* button) override
        {
            if (button->getName() == "btStart1")
            {   startGame(1);
            }
            else if (button->getName() == "btStart2")
            {   startGame(2);
            }
            else if (button->getName() == "btTuning")
            {   trayMgr->showOkDialog(NAMEGAME,"...");
                openTuning();
            }
            else if (button->getName() == "btExit")
            {   getRoot()->queueEndRendering();
            }

            ui.buttonHit(button);
        }

        ///---------------------------------------|
        /// Диалог выхода из игры.                |
        ///---------------------------------------:
        void okDialogClosed(const Ogre::DisplayString& message) override
        {   std::cout << "OK-диалог закрыт: " + Ogre::String(message);

            if(message == "Exit") getRoot()->queueEndRendering();

        }

        ///---------------------------------------|
        /// Установка иконки на окно.             |
        ///---------------------------------------:
        void setWindowIcon(Ogre::RenderWindow* window);

        ///---------------------------------------|
        /// Программно добавлем пути к ресурсам.  |
        ///---------------------------------------:
        void addResourcePath()
        {
        // Добавьте ВСЕ папки с ресурсами перед инициализацией
            ResourceGroupManager& rgm = ResourceGroupManager::getSingleton();

            // Добавляем основную папку media
            rgm.addResourceLocation("media", "FileSystem", "General");
            rgm.addResourceLocation("media/fonts", "FileSystem", "General");

            // Явно загружаем .fontdef файл
            rgm.initialiseResourceGroup("General");

            // ИЛИ инициализируем все
            //rgm.initialiseAllResourceGroups();
        }

        void gameOver(Args_t)
        {   isGameOver     = true;
            isSpeedRotWold = 20.f * (1 - (rand()%2)*2);

            this->trayMgr
                ->getTrayContainer(TrayLocation::TL_CENTER)
                ->setVisible(true);
        }

        void gameStart()
        {   isSpeedRotWold = 0;
            isPause    = false;
            isGameOver = false;
            camera2StartGame();
        }

        void camera2StartGame()
        {   camera.set2Start ();
            nodeBase->setOrientation(orientationWorldStart);
        }


    private:
        void startGame(int players)
        {   
            std::cout << std::format(
                "▶ Starting game with {} player(s)...\n", players);

            gameStart();
            games.setup(players);

            this->trayMgr
                ->getTrayContainer(TrayLocation::TL_CENTER)
                ->setVisible(false);
        }

        void openTuning() 
        {   std::cout << "⚙ Opening tuning menu...\n";
        }

        bool mouseMoved(const OgreBites::MouseMotionEvent& evt) override
        {   return cursor.mouseMoved(evt);
        }

        std::unique_ptr<MenuStart> mMenu;

    }; // struct InspectorRoot
}      // namespace mdl
#endif // INSPECTOR_H

