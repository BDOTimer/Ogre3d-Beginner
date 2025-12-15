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

        Camera       camera;
        Ninja         ninja;
        Sphere       sphere;
        Lights       lights;
        UI               ui;
        Tree           tree;
        Well           well;
        Ground       ground;
        
        Ogre::RTShader::ShaderGenerator* shadergen;


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

            root   = getRoot();
            scnMgr = root->createSceneManager();
            scnMgr->setAmbientLight(ColourValue(0, 0, 0));
            scnMgr->setShadowTechnique(
                ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);

            nodeBase = scnMgr->getRootSceneNode()->createChildSceneNode();

            Base::scnMgr   = scnMgr;
            Base::nodeBase = nodeBase;

        /// root->loadPlugin("OgreAssimp");

            shadergen = RTShader::ShaderGenerator::getSingletonPtr();
            shadergen-> addSceneManager(scnMgr);

            camera   .setup();
            ninja    .setup();
        /// sphere   .setup();
            lights   .setup();
            tree     .setup();
            well     .setup();
            ground   .setup();

            // Добавьте ВСЕ папки с ресурсами перед инициализацией
            ResourceGroupManager& rgm = ResourceGroupManager::getSingleton();

            // Добавляем основную папку media
            rgm.addResourceLocation("media", "FileSystem", "General");

            // Добавляем папку со шрифтами отдельно (важно!)
            rgm.addResourceLocation("media/fonts", "FileSystem", "General");

            // Явно загружаем .fontdef файл
            rgm.initialiseResourceGroup("General");

            // ИЛИ инициализируем все
            //rgm.initialiseAllResourceGroups();

            Ogre::RenderWindow* mWindow = getRenderWindow();
            OgreBites::TrayManager* trayMgr
                = new OgreBites::TrayManager("UI", mWindow);
            trayMgr->createLabel(OgreBites::TL_TOP, "title",
                "Объект UI создан! Автор: ...");
        }

        ///-------------------------------------------|
        /// Обработка клавиш.                         |
        ///-------------------------------------------:
        bool keyPressed(const KeyboardEvent& evt)
        {   if (evt.keysym.sym == SDLK_ESCAPE)
            {   getRoot()->queueEndRendering();
            }
            else if (isRotWold && evt.keysym.sym == SDLK_SPACE)
            {   isRotWold = false;
                return      true;
            }
            
            return well.keyPressed(evt);
        }

        float accumulatedTime{0};  // Накопленное время
        float intervalTime   {4};  // Интервал (1 секунда)
        bool  isRotWold   {true};

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

            well.update(evt.timeSinceLastFrame);

            if(isRotWold) nodeBase->yaw  (Ogre::Degree(30 * deltaTime));
        }

    };
}

#endif // INSPECTOR_H

