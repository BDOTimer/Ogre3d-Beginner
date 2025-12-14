///----------------------------------------------------------------------------|
/// "well.h"
///----------------------------------------------------------------------------:
#ifndef WELL_H
#define WELL_H
#include "figure.h"


///---------|
/// Modules.|
///---------:
namespace mdl
{   

    struct  Well : Base
    {       Well()
            {   
            }

        
        SceneNode*  node{nullptr};
        Figure      figure;


    private:
        void setup(Ogre::SceneManager* scnMgr)
        {   
            ///------------------------|
            /// Нод корзины!           |
            ///------------------------:
            node = scnMgr->getRootSceneNode()->createChildSceneNode();

            figure.setup(scnMgr, node);
        }

        void changeFigure    ()
        {   figure.reGenerate();
        }

        ///-------------------------------------------|
        /// Обработка клавиш.                         |
        ///-------------------------------------------:
        bool keyPressed(const KeyboardEvent& evt)
        {   return figure.keyPressed(evt);
        }

        void update(float deltaTime)
        {   figure.update(deltaTime);
        }
    
        friend struct InspectorRoot;
    };
}


#endif // WELL_H

