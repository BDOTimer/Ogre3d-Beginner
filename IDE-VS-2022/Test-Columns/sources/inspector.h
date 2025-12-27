///----------------------------------------------------------------------------|
/// "inspector.h"
///----------------------------------------------------------------------------:
#ifndef INSPECTOR_H
#define INSPECTOR_H
#include "intro.h"
#include "game.h"
#include "sky.h"

#include "OgreTrays.h"


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

    {       InspectorRoot( ) : 
                OgreBites::ApplicationContext("")
            ,   autoCotrollerSin( camera.val,
                                  [this](){ this->camera.set2Start(); })
            {
            }

        Ogre::Root*           root;
        Ogre::SceneManager* scnMgr;
        SceneNode*        nodeBase;
        SceneNode*        nodeUser;

        Camera              camera;
        Lights              lights;

        UI                      ui;
        Effects            effects;
    /// SkyDome*           skyDome;
        Intro                intro;
        Sky*                   sky;

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
            //OgreBites::ApplicationContextSDL::setup();
            addInputListener      (this);

            //mMenu = std::make_unique<MenuStart>(getRenderWindow());
            //mMenu->show(); // сразу показываем

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

            //OgreBites::InputListener::mousePressed

            ////////////////////////////////////////////////////////////////////
            Ogre::RenderWindow* window = getRenderWindow();
            setWindowIcon      (window);
            ////////////////////////////////////////////////////////////////////

            ///-----------------------------------|
            /// Регистрация обработчиков событий. |
            ///-----------------------------------:
            addEvent(gameOver);

        /// root->loadPlugin("OgreAssimp");

            shadergen = RTShader::ShaderGenerator::getSingletonPtr();
            shadergen-> addSceneManager(scnMgr);

            camera   .setup(nodeUser);
            lights   .setup(camera.camNode);
            decor    .setup();
            ui       .setup();
            effects  .setup();
            intro    .setup();

            sky = new Sky;

            //addInputListener(ui.trayMgr);

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
                    getRoot()->queueEndRendering();
                    return true;
                case '1':
                     gameStart(); games.setup(1);
                    return true;
                case '2':
                     gameStart(); games.setup(2);
                    return true;
                default:;
            }

            switch(evt.keysym.sym)
            {
                case OgreBites::SDLK_ESCAPE:
                    gameOver({});
                    return true;
                case OgreBites::SDLK_F8:
                /// PrintNodeHierarchy(Glob::nodeBase);
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

        bool mousePressed(const OgreBites::MouseButtonEvent& evt)
        {   return ui.mousePressed(evt);
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

            accumulatedTime += evt.timeSinceLastFrame;
       
            if(accumulatedTime >= intervalTime)
            {
            /// effects.tickSecond();
                
                accumulatedTime -= intervalTime;

                seconds++;

                effects.update(seconds);
            }

            ///-----------------------------------|
            /// FPS.                              |
            ///-----------------------------------:
            /// myl::Fps::get().update(deltaTime);

            if(isPause || isGameOver)
            {
            }
            else
            {   games.update();
            }

            if(isSpeedRotWold != 0)
            {   nodeBase->yaw(Ogre::Degree(deltaTime * isSpeedRotWold));
                autoCotrollerSin.update();
            }
        }

        bool cframeRenderingQueued(const Ogre::FrameEvent&) //override
        {
        
            //if (mMenu->update())
            {   //getRoot()->queueEndRendering();
                //return false;
            }
        /*
            switch (mMenu->popAction())
            {   case MenuStart::Action::Start1Player:
                    //startGame(1);
                    break;
                case MenuStart::Action::Start2Players:
                    //startGame(2);
                    break;
                case MenuStart::Action::OpenTuning:
                    //openTuning();
                    break;
                default: break;
            }
        */
            return ApplicationContext::frameRenderingQueued({});
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
        {   std::cout << "▶ Starting game with " << players << " player(s)...\n";
            // Здесь: загрузка уровня, камера, игроки...
        }

        void openTuning() 
        {   std::cout << "⚙ Opening tuning menu...\n";
            // Можно показать другой меню-класс
        }

        Ogre::SceneManager* mSceneMgr = nullptr;
        std::unique_ptr<MenuStart> mMenu;

    }; // struct InspectorRoot
}      // namespace mdl
#endif // INSPECTOR_H

