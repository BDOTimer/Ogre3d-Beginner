///----------------------------------------------------------------------------|
/// "inspector.h"
///----------------------------------------------------------------------------:
#ifndef INSPECTOR_H
#define INSPECTOR_H
#include "well.h"
#include "ui.h"


///---------|
/// Models. |
///---------:
namespace mdl
{  
    ///------------------------------------------------------------------------|
    /// InspectorRoot.("Test-Columns-2025")
    ///---------------------------------------------------------- InspectorRoot:
    struct  InspectorRoot
            :   Base
            ,   OgreBites::ApplicationContext
            ,   OgreBites::InputListener
    {       InspectorRoot( ): OgreBites::ApplicationContext("")
            {}
        
        Ogre::Root*           root;
        Ogre::SceneManager* scnMgr;
        SceneNode*        nodeBase;
        SceneNode*        nodeUser ;

        Camera       camera;
        Ninja         ninja;
        Lights       lights;
        UI               ui;
        Tree           tree;
        Well           well;
        Ground       ground;
        Effects	    effects;

        Ogre::RTShader::ShaderGenerator* shadergen;

        ConfigGame& cfg{ConfigGame::get()};

    protected:

        ///-------------------------------------------|
        /// Свой путь к "ogre.h" ---> (box.cpp)       |
        ///-------------------------------------------:
        void createRoot() override;

        void setup() override
        {   
            OgreBites::ApplicationContext::setup();

            addInputListener(this);

            Base::pInspectorRoot = this;
            Base::ctx             = this;
            Base::pIListener     = this;

            root   = getRoot();
            scnMgr = root->createSceneManager();
            scnMgr->setAmbientLight(ColourValue(0, 0, 0));
            scnMgr->setShadowTechnique(
                ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);

            nodeBase = scnMgr->getRootSceneNode()->createChildSceneNode();
            nodeUser  = scnMgr->getRootSceneNode()->createChildSceneNode();

            Base::scnMgr   = scnMgr;
            Base::nodeBase = nodeBase;

            addResourcePath();

            ////////////////////////////////////////////////////////////////////
            Ogre::RenderWindow* window = getRenderWindow();
            setWindowIcon      (window);
            ////////////////////////////////////////////////////////////////////

        /// root->loadPlugin("OgreAssimp");

            shadergen = RTShader::ShaderGenerator::getSingletonPtr();
            shadergen-> addSceneManager(scnMgr);

            camera   .setup(nodeUser);
            lights   .setup(camera.camNode);
            ninja    .setup();
            tree     .setup();
            well     .setup();
            ground   .setup();
            ui       .setup();
            effects  .setup();
        }

        ///-------------------------------------------|
        /// Обработка клавиш.                         |
        ///-------------------------------------------:
        bool keyPressed(const KeyboardEvent& evt)
        {   
            switch(evt.keysym.sym)
            {
                case OgreBites::SDLK_ESCAPE:
                    getRoot()->queueEndRendering();
                    return true;
                case OgreBites::SDLK_F5:
                    isSpeedRotWold = isSpeedRotWold ? 0 :  speedRotWold;
                    break;
                case OgreBites::SDLK_F6:
                    isSpeedRotWold = isSpeedRotWold ? 0 : -speedRotWold;
                    break;
                case OgreBites::SDLK_PAUSE:
                case 112: // 'P'--->112
                    isPause = !isPause;
                    break;
                default: // l(evt.keysym.sym)
                    ;
            }

            bool    b{false};
                    b |= ui.keyPressed  (evt);
                    b |= well.keyPressed(evt);
            return  b;
        }

        bool mousePressed(const OgreBites::MouseButtonEvent& evt)
        {   return ui.mousePressed(evt);
        }

        float accumulatedTime{ 0};  // Накопленное время
        float intervalTime   { 1};  // Интервал (1 секунда)
        int   isSpeedRotWold { 0};  // Нет вращения Мира.
        int     speedRotWold {30};  // Нет вращения Мира.
        bool  isPause     {false};

        ///-------------------------------------------|
        /// Тут крутятся фреймы.                      |
        ///-------------------------------------------:
        void frameRendered(const Ogre::FrameEvent& evt) override
        {
            Base::deltaTime = evt.timeSinceLastFrame;

            accumulatedTime += evt.timeSinceLastFrame;
       
            if(accumulatedTime >= intervalTime)
            {
            /// changeFigure();
                accumulatedTime -= intervalTime;
            }

            if(isPause) return;

            well.update(evt.timeSinceLastFrame);

            if(isSpeedRotWold)
            {   nodeBase->yaw  (Ogre::Degree(deltaTime * isSpeedRotWold));
            }
        }

        ///-------------------------------------------|
        /// Установка иконки на окно.                 |
        ///-------------------------------------------:
        void setWindowIcon(Ogre::RenderWindow* window);

        ///-------------------------------------------|
        /// Программно добавлем пути к ресурсам.      |
        ///-------------------------------------------:
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
    };
}

#endif // INSPECTOR_H

