///----------------------------------------------------------------------------|
/// "figure.h"
///----------------------------------------------------------------------------:
#ifndef FIGURE_H
#define FIGURE_H
#include "primitives.h"

///---------|
/// Models. |
///---------:
namespace mdl
{   

    ///------------------------------------------------------------------------|
    /// Gems
    ///------------------------------------------------------------------- Gems:
    struct  Gems : Base
    {       
        Ogre::Entity*  entity{nullptr};
        SceneNode*     node  {nullptr};
        
        void setup(Ogre::SceneManager* scnMgr,
                   const size_t        n,
                   SceneNode*          parent)
        {   
            std::string sn   {std::to_string(n)};
            std::string name {"Sphere"}; name  += sn;

            if(nullptr != entity) clear(scnMgr);

            static std::array<const char*, 2> nameMesh
            {   "sphere.mesh",
                "cube.mesh"
            };

            const unsigned rnd = rand() % 2;

            entity = scnMgr->createEntity(name, nameMesh[rnd]);

            if(nullptr == node)
            {   node   =  parent->createChildSceneNode();
            }
            node->attachObject(entity);
            node->setPosition(0, 50 + (100 * float(n)), 0);

            switch(rnd)
            {   case  0: node->setScale(0.5f, 0.5f, 0.5f); break;
                case  1: node->setScale(0.9f, 0.9f, 0.9f);break;
                default: ;
            }
        }

        void clear(Ogre::SceneManager* scnMgr)
        {   /// TODO ...
            /// node->detachAllObjects();

            if(entity->isAttached())
            {
                Ogre::SceneNode* parentNode = entity->getParentSceneNode();
                if(parentNode)
                {   parentNode->detachObject(entity);
                    scnMgr->destroyEntity   (entity->getName());
                }

            /// if(node) scnMgr->destroySceneNode(node);
            }
        }
    };


    ///------------------------------------------------------------------------|
    /// Figure
    ///----------------------------------------------------------------- Figure:
    struct  Figure   : Base
    {       Figure() : 
                gems(ConfigGame::get().colors.size())
            ,   mat (ConfigGame::get().colors.size())
            {   
            }

        Ogre::SceneManager*      scnMgr;
        const ConfigGame&        cfg{ConfigGame::get()};
        std::vector<Gems>        gems;
        std::vector<MaterialPtr> mat ;
        SceneNode*               node; /// Нод фигуры!
        float        speedMove{50.0f}; /// единиц в секунду.

        void setup(Ogre::SceneManager* scnMgr)
        {   this->scnMgr = scnMgr;

            ///------------------------|
            /// TODO: взять у Well!    |
            ///------------------------:
            node = scnMgr->getRootSceneNode()->createChildSceneNode();

            createMatetilal();
            reGenerate     ();
        }

        void reGenerate()
        {   
            node->setPosition(0, cfg.get().getWellH(), 0);

            for(unsigned i{}; i < gems.size( ); ++i)
            {   gems[i].setup(scnMgr, i, node);
                const int rnd = rand() % mat.size();
                gems[i].entity->setMaterialName(mat[size_t(rnd)]->getName());
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
        
            // Или с указанием пространства
            // mNode->translate(Ogre::Vector3(0, 0, -mMoveSpeed * deltaTime), 
            //                  Ogre::Node::TS_LOCAL);

            Ogre::Vector3 position = node->getPosition();

            if( position.y <= groundLevel)
            {   position.y  = groundLevel; // Не даем уйти ниже земли
                
                node->setPosition(position.x, groundLevel, position.z);
                isFalling   = false;

                onGroundCollision();
            }
        }

        void keyPressed(const KeyboardEvent& evt)
        {   
            if (evt.keysym.sym == SDLK_SPACE)
            {   reShuffleGems();
            }
        }
        

    private:
        void createMatetilal()
        {   
            for(unsigned i{}; i < mat.size(); ++i)
            {
                mat[i] = MaterialManager::getSingleton().create(
                    std::format("matSph{}", i),
                    ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
                );

                Ogre::Pass* const p = mat[i]->getTechnique(0)->getPass(0);
                        p->setDiffuse (cfg.colors[i]);
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
            std::vector<Ogre::Vector3> pos;

            for(const auto& e : gems)
            {   pos.emplace_back(e.node->getPosition());
            }   pos.emplace_back(Ogre::Vector3());

            pos.back() = pos.front();

            size_t i{1};
            for(const auto& e : gems)
            {   e.node->setPosition(pos[i++]);
            }
        }
    };
}

#endif // FIGURE_H


