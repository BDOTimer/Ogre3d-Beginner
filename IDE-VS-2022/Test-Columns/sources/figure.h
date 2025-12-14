///----------------------------------------------------------------------------|
/// "figure.h"
///----------------------------------------------------------------------------:
#ifndef FIGURE_H
#define FIGURE_H
#include "primitives.h"

///---------|
/// Modules.|
///---------:
namespace mdl
{   

    ///------------------------------------------------------------------------|
    /// Gem
    ///-------------------------------------------------------------------- Gem:
    struct  Gem : Base
    {       
        Ogre::Entity*  entity{nullptr};
        SceneNode*     node  {nullptr};
        size_t         id;
        
        void setup(Ogre::SceneManager* scnMgr,
                   const size_t        n     ,
                   SceneNode*          parent,
                   const size_t        id)
        {   
            this->id = id;

            if(nullptr != entity) clear(scnMgr);

            const auto& descriptions{ConfigGame::get().descriptionGems};

            entity = scnMgr->createEntity(
                std::format("Sphere{}", n),
                descriptions[id].nameMesh
            );

            if(nullptr == node)
            {   node   =  parent->createChildSceneNode();
            }
            node->resetOrientation  ();
            node->attachObject(entity);
            node->setPosition (0, 50 + (100 * float(n)), 0);
            node->setScale    (descriptions[id].scale);

            if(id % 2) speed = -speed;
        }

        void clear(Ogre::SceneManager* scnMgr)
        {   /// TODO ...
            /// node->detachAllObjects();

            if(entity->isAttached())
            {
                Ogre::SceneNode* parentNode = entity->getParentSceneNode();
                if( parentNode)
                {   parentNode->detachObject(entity);
                    scnMgr->destroyEntity   (entity->getName());
                }

            /// if(node) scnMgr->destroySceneNode(node);
            }
        }

        float speed{50};
        void update(float deltaTime)
        {   
            if(ConfigGame::get().isGemRotating)
            switch(id)
            {   case   3: node->yaw  (Ogre::Degree(speed * deltaTime)); break;
                case   4: node->pitch(Ogre::Degree(speed * deltaTime)); break;
                default:;
            }
        }
    };


    ///------------------------------------------------------------------------|
    /// Figure
    ///----------------------------------------------------------------- Figure:
    struct  Figure   : Base
    {       Figure() : 
                gems(ConfigGame::get().descriptionGems.size())
            ,   mat (ConfigGame::get().descriptionGems.size())
            {   
            }

        Ogre::SceneManager*      scnMgr;
        const ConfigGame&        cfg{ConfigGame::get()};
        std::vector<Gem>         gems;
        std::vector<MaterialPtr> mat ;
        SceneNode*               node; /// Нод фигуры!
        float        speedMove{50.0f}; /// единиц в секунду.


    private:
        void setup(Ogre::SceneManager* scnMgr, SceneNode* well)
        {   this->scnMgr = scnMgr;

            ///------------------------|
            /// TODO: взять у Well!    |
            ///------------------------:
            node = well->createChildSceneNode();

            createMatetilal();
            reGenerate     ();
        }

        void reGenerate()
        {   
            node->setPosition(0, cfg.get().getWellH(), 0);

            for(unsigned i{}; i < gems.size( ); ++i)
            {   
                const size_t rnd = rand() % mat.size();
                
                gems[i].setup(scnMgr, i, node, rnd);
                gems[i].entity->setMaterialName(mat[rnd]->getName());
            }
        }

        void Xclear()
        {   for(auto& e : gems) e.clear(scnMgr);
        }

        float groundLevel {0};
        bool  isFalling{true};

        void update(float deltaTime)
        {   ///------------------------|
            /// Движение вниз.         |
            ///------------------------:
            if(isFalling)
            {   node->translate(0, -speedMove * deltaTime, 0);
            }

            Ogre::Vector3 position = node->getPosition();

            if( position.y <= groundLevel)
            {   position.y  = groundLevel; // Не даем уйти ниже земли
                
                node->setPosition(position.x, groundLevel, position.z);
                isFalling   = false;

                onGroundCollision();
            }

            for(auto& e : gems) e.update(deltaTime);



            if(isMoveLeft)
            {   node->translate(-speedMoveLR * deltaTime, 0, 0);
            }
            if(isMoveRight)
            {   node->translate(speedMoveLR * deltaTime, 0, 0);
            }
        }

        ///-------------------------------------------|
        /// Обработка клавиш.                         |
        ///-------------------------------------------:
        float speedMoveLR{100.0f};
        bool isMoveLeft  {false };
        bool isMoveRight {false };
        bool keyPressed  (const KeyboardEvent& evt)
        {   
            if (evt.keysym.sym == SDLK_SPACE)
            {   reShuffleGems();
            }

            switch(evt.keysym.sym)
            {
            case OgreBites::SDLK_UP:
                break;
                
            case OgreBites::SDLK_DOWN:
                break;
                
            case 122: /// OgreBites::SDLK_LEFT: 'Z'
                
                if(isMoveRight) isMoveLeft  =  isMoveRight = false;
                else isMoveLeft  =  true;
                return false;
                break;
                
            case 120: /// OgreBites::SDLK_RIGHT: 'X'
                if(isMoveLeft) isMoveLeft  =  isMoveLeft = false;
                else isMoveRight  =  true;
                return false;
                break;
                
            default:
            /// l(evt.keysym.sym)
                return true; // Другие клавиши не обрабатываем
            }
            return false;
        }
        
        void createMatetilal()
        {   
            for(unsigned i{}; i < mat.size(); ++i)
            {
                mat[i] = MaterialManager::getSingleton().create(
                    std::format("matSph{}", i),
                    ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
                );

                Ogre::Pass* const p = mat[i]->getTechnique(0)->getPass(0);
                        p->setDiffuse (cfg.descriptionGems[i].color);
                        p->setAmbient (ColourValue(0.3f, 0.15f, 0.0));
                        p->setSpecular(ColourValue(1.0 , 1.0  , 1.0));
                        p->setShininess(64.0);
            }
        }

        void onGroundCollision()
        {    reGenerate();
             isFalling = true;
        }

        void reShuffleGems()
        {
            Ogre::Vector3 a{gems.front().node->getPosition()};

            for(size_t i{}, N{gems.size() - 1}; i < N; ++i)
            {   gems[i  ].node->setPosition(
                gems[i+1].node->getPosition());
            }

            gems.back().node->setPosition(a);
        }

        friend struct Well;
    };
}

#endif // FIGURE_H


