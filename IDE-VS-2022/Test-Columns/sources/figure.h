///----------------------------------------------------------------------------|
/// "figure.h"
///----------------------------------------------------------------------------:
#ifndef FIGURE_H
#define FIGURE_H
#include "primitives.h"

///---------|
/// My lib  |
///---------:
namespace myl
{   

    ///------------------------------------------------------------------------|
    /// Движение точки на заданную дистанцию...
    ///---------------------------------------------------------- Step2Distance:
    struct  Step2Distance
    {
        enum  EDIR
        {     RIGHT,
              LEFT ,
              STOP
        }eDir{STOP};

        ///-------------------------------------------|
        /// Начать движение.                          |
        ///-------------------------------------------:
        void start(float posStart, float distance)
        {   
            if(0.f == distance) return;

            eDir = distance < 0 ? LEFT : RIGHT;
            
            this->position =           posStart;
            this->posStart =           posStart;
            this->distance =           distance;
            this->distancA =  std::abs(distance);
            dist           = 0;
            isMoving = true;
        }

        bool sensor(const float delta)
        {   
            if(!isMoving) return false;

            switch(eDir)
            {   case RIGHT: position += delta; break;
                case LEFT : position -= delta; break;
                default   : ;
            }

            dist += delta;
            
            if(dist > distancA)
            {
                position = posStart + distance;
                
                isMoving = false;
                eDir     = STOP ;
            }

            return isMoving;
        }

        float getPosition() const { return position; }

        ///--------------|
        /// Тест.        |
        ///--------------:
        static void test()
        {   TestInfo inf("Step2Distance");

            Step2Distance step2Distance;

            inf.Case();
            {
                step2Distance.start(20, 70);
                while(step2Distance.sensor(0.12345f));
                l(TestInfo::showResult(90.f, step2Distance.getPosition()))
            }

            inf.Case();
            {
                step2Distance.start(90, -130);
                while(step2Distance.sensor(0.12345f));
                l(TestInfo::showResult(-40.f, step2Distance.getPosition()))
            }

            inf.Case();
            {
                step2Distance.start(-20, -130);
                while(step2Distance.sensor(0.12345f));
                l(TestInfo::showResult(-150.f, step2Distance.getPosition()))
            }

            inf.Case();
            {
                step2Distance.start(-200, 40);
                while(step2Distance.sensor(0.12345f));
                l(TestInfo::showResult(-160.f, step2Distance.getPosition()))
            }
        }

    protected:
        float posStart;
        float position;
        float distance;
        float distancA;
        
        float dist;
        bool  isMoving{false};
    };

    struct  Step2DistanceB : Step2Distance
    {       Step2DistanceB(float distance)
            {   Step2Distance::distancA = distance;
                Step2Distance::distance = distance;
            }

        void start(float posStart, EDIR dir)
        {   
            if(isMoving) return;

            eDir = dir;
            
            this->position = posStart;
            this->posStart = posStart;
            if(dir == LEFT) this->distance = -std::abs(this->distance);
            dist     = 0;
            isMoving = true;
        }

        ///--------------|
        /// Тест.        |
        ///--------------:
        static void test()
        {   TestInfo inf("Step2DistanceB");

            Step2DistanceB step2Distance(30);

            inf.Case();
            {
                step2Distance.start(20, Step2DistanceB::RIGHT);
                while(step2Distance.sensor(0.12345f));

                step2Distance.start(
                    step2Distance.getPosition(), Step2DistanceB::LEFT );
                while(step2Distance.sensor(0.12345f));

                step2Distance.start(
                    step2Distance.getPosition(), Step2DistanceB::LEFT );
                while(step2Distance.sensor(0.12345f));

                l(TestInfo::showResult(-10.f, step2Distance.getPosition()))
            }
        }
    };
}

///---------|
/// Modules |
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
        
        ///-------------------------------------------|
        /// Стартовая инициализация.                  |
        ///-------------------------------------------:
        void setup(Ogre::SceneManager* scnMgr,
                   const size_t        n     ,
                   SceneNode*          parent,
                   const size_t        id)
        {   
            this->id = id;

            const auto& SZCELL{ConfigGame::get().sizeCell};

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
            node->setPosition (0, SZCELL / 2 + (SZCELL * float(n)), 0);
            node->setScale    (descriptions[id].scale);

            if(id % 2) speed = -speed;
        }

        ///-------------------------------------------|
        /// Удаление жемчужины из фигуры.             |
        ///-------------------------------------------:
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

        ///-------------------------------------------|
        /// Анимация жемчужины.                       |
        ///-------------------------------------------:
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
            ,   step2DistanceB(ConfigGame::get().sizeCell)
            {   
            }

        Ogre::SceneManager*      scnMgr;
        const ConfigGame&        cfg{ConfigGame::get()};
        std::vector<Gem>         gems;
        std::vector<MaterialPtr> mat ;
        SceneNode*               node; /// Нод фигуры!
        float        speedMove{50.0f}; /// единиц в секунду.


    private:

        myl::Step2DistanceB step2DistanceB;

        ///-------------------------------------------|
        /// Стартовая инициализация.                  |
        ///-------------------------------------------:
        void setup(Ogre::SceneManager* scnMgr, SceneNode* well)
        {   this->scnMgr = scnMgr;

            ///------------------------|
            /// Крепим к корзине.      |
            ///------------------------:
            node = well->createChildSceneNode();

            createMaterial();
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

        ///-------------------------------------------|
        /// Вызывается для каждого фрейма(кадра).     |
        ///-------------------------------------------:
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

            /// TODO: доделать ...
            if(step2DistanceB.sensor(deltaTime * speedMoving))
            {   const auto& p{node->getPosition()};
                node->setPosition(step2DistanceB.getPosition(), p.y, p.z);
            }
        }

        ///-------------------------------------------|
        /// Обработка клавиш.                         |
        ///-------------------------------------------:
        float speedMoving{100.0f};
        bool  keyPressed (const KeyboardEvent& evt)
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
            {   const auto& p = node->getPosition();
                step2DistanceB.start(p.x, myl::Step2Distance::LEFT);
                break;
            }
            case 120: /// OgreBites::SDLK_RIGHT: 'X'
            {   const auto& p = node->getPosition();
                step2DistanceB.start(p.x, myl::Step2Distance::RIGHT);
                break;
            } 
            default:
            /// l(evt.keysym.sym)
                return true; // Другие клавиши не обрабатываем
            }
            return false;
        }
        
        ///-------------------------------------------|
        /// Матрериалы создаются один раз.            |
        ///-------------------------------------------:
        void createMaterial()
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

        ///-------------------------------------------|
        /// Обработка столкновения фигуры с землей.   |
        ///-------------------------------------------:
        void onGroundCollision()
        {    reGenerate();
             isFalling = true;
        }

        ///-------------------------------------------|
        /// Циклический сдвиг по колоне вверх на 1.   |
        ///-------------------------------------------:
        void reShuffleGems()
        {
            Ogre::Vector3 a{gems.front().node->getPosition()};

            for(size_t i{}, N{gems.size() - 1}; i < N; ++i)
            {   gems[i  ].node->setPosition(
                gems[i+1].node->getPosition());
            }

            gems.back().node->setPosition(a);
        }

        ///-------------------------------------------|
        /// Сенсор столновения.                       |
        ///-------------------------------------------:
        bool sensorCollisions()
        {

        }

        friend struct Well;
    };
}

#endif // FIGURE_H


