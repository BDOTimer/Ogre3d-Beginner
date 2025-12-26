///----------------------------------------------------------------------------|
/// "inspector.h"
///----------------------------------------------------------------------------:
#ifndef INSPECTOR_H
#define INSPECTOR_H
#include "intro.h"
#include "game.h"
#include "sky.h"


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
                                  [this](){this->camera.set2Start();})
            {}

        Ogre::Root*           root;
        Ogre::SceneManager* scnMgr;
        SceneNode*        nodeBase;
        SceneNode*        nodeUser;

        Camera              camera;
        Lights              lights;
        Ninja                ninja;
        UI                      ui;
        Tree                  tree;
        Ground              ground;
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

            addInputListener(this);

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

            ///-----------------------------------|
            /// Регистрация обработчиков событий. |
            ///-----------------------------------:
            addEvent(gameOver);

        /// root->loadPlugin("OgreAssimp");

            shadergen = RTShader::ShaderGenerator::getSingletonPtr();
            shadergen-> addSceneManager(scnMgr);

            camera   .setup(nodeUser);
            lights   .setup(camera.camNode);
            ninja    .setup();
            tree     .setup();
            ground   .setup();
            ui       .setup();
            effects  .setup();
            intro    .setup();

            sky = new Sky;

            Glob::ninja = ninja.node;

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
                    games.setup(1); gameStart();
                    return true;
                case '2':
                    games.setup(2); gameStart();
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

        float    accumulatedTime { 0};  // Накопленное время
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

        void setScore(int score)
        {   ui.score->set(score);
        }

        void camera2StartGame()
        {   camera.set2Start ();
            nodeBase->setOrientation(orientationWorldStart);
        }

    }; // struct InspectorRoot
}      // namespace mdl
#endif // INSPECTOR_H

