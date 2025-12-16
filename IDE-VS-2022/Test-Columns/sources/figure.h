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

        bool isUserMoving() const { return isMoving; }

        ///-------------------------------------------|
        /// Начать движение.                          |
        ///-------------------------------------------:
        void start(float posStart, float distance)
        {   
            if(0.f == distance) return;

            ASSERT(posStart == int(posStart))

            eDir = distance < 0 ? LEFT : RIGHT;
            
            this->position =           posStart;
            this->posStart =           posStart;
            this->distance =           distance;
            this->distancA =  std::abs(distance);
            dist           =  0;
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

                if(position != int(position))///////////////////////////////////
                {   
                    l(posStart)
                    l(distance)
                    l(position)
                    l(int(position))
                    
                }

            /// ASSERT(position == int(position)) ///<--------------------: TODO
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

    ///------------------------------------------------------------------------|
    /// Модификация Step2Distance для удобства юзания.
    ///--------------------------------------------------------- Step2DistanceB:
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

            Step2DistanceB step2Distance(100);

            inf.Case();
            {
                step2Distance.start(0, Step2DistanceB::RIGHT);
                while(step2Distance.sensor(0.12345f));

                step2Distance.start(
                    step2Distance.getPosition(), Step2DistanceB::RIGHT );
                while(step2Distance.sensor(0.12345f));

                step2Distance.start(
                    step2Distance.getPosition(), Step2DistanceB::RIGHT );
                while(step2Distance.sensor(0.12345f));

                l(TestInfo::showResult(300.f, step2Distance.getPosition()))
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
        void setup(const size_t        n     ,
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
        
    private:

        myl::Step2DistanceB step2DistanceB;

        struct 
        {   float  get() const   { return speedMoveCurr ; }
            void start(){ speedMoveCurr = speedMoveStart; }
            void    up(){ speedMoveCurr = speedMoveFast ; }

        private:
            float speedMoveStart{100.0f}; /// единиц в секунду.
            float speedMoveFast {300.0f};
            float speedMoveCurr {speedMoveStart};
        }speedFigFall;

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
            reGenerate    ();
        }

        void reGenerate()
        {   
            node->setPosition(0, cfg.get().getWellH(), 0);

            for(unsigned i{}; i < gems.size( ); ++i)
            {   
                const size_t rnd = rand() % mat.size();
                
                gems[i].setup(i, node, rnd);
                gems[i].entity->setMaterialName(mat[rnd]->getName());
            }

            speedFigFall.start();
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
            {   node->translate(0, -speedFigFall.get() * deltaTime, 0);
            }

            Ogre::Vector3 position = node->getPosition();

            if( position.y <= groundLevel)
            {   position.y  = groundLevel; // Не даем уйти ниже земли
                
                node->setPosition(position.x, groundLevel, position.z);
                isFalling   = false;
            }

            for(auto& e : gems) e.update(deltaTime);

            /// TODO: доделать ...
            if(step2DistanceB.sensor(deltaTime * speedMoving))
            {   const auto& p{node->getPosition()};
                node->setPosition(step2DistanceB.getPosition(), p.y, p.z);
            }
            else if(!isFalling && !step2DistanceB.isUserMoving())
            {   
                onGroundCollision();
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
                speedFigFall.up();
                break;
                
            case OgreBites::SDLK_LEFT: /// 122: 'Z'
            {   const auto& p = node->getPosition();
                step2DistanceB.start(p.x, myl::Step2Distance::LEFT);
                break;
            }
            case OgreBites::SDLK_RIGHT: /// 120: 'X'
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
        {    sendFigure2Well();
             reGenerate();
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

        ///-------------------------------------------|
        /// Делегат от логики корзины.                |
        ///-------------------------------------------:
        std::function<void(Figure*)> delegate4Well;

        ///-------------------------------------------|
        /// Отправка фигуры в корзину.                |
        ///-------------------------------------------:
        void sendFigure2Well()
        {   delegate4Well(this);
        }

        friend struct Well;
    };
}

#endif // FIGURE_H


