///----------------------------------------------------------------------------|
/// "figure.h"
///----------------------------------------------------------------------------:
#ifndef FIGURE_H
#define FIGURE_H
#include "primitives.h"
#include "physics.h"

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

        ///---------------------------------------|
        /// Начать движение.                      |
        ///---------------------------------------:
        void start(float PosStart, float Distance)
        {   
            if(0.f == Distance) return;

            eDir = Distance < 0 ? LEFT : RIGHT;
            
            this->position =           PosStart;
            this->posStart =           PosStart;
            this->distance =           Distance;
            this->distancA =  std::abs(Distance);
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

        void startGravity(const Ogre::Vector3& PosStart)
        {   
            const unsigned w = (unsigned)ConfigGame::get().sizeCell;

            ///--------------|
            /// Коррекция.   |
            ///--------------:
            const float Y{float(w) * (unsigned(PosStart.y + 50.f) / w)};

            ///--------------|
            /// Дебаг.       |
            ///--------------:
            if(false)
            {   LN
                l(PosStart.y)
                l(Y)
            }
            
            start(Y, Step2Distance::LEFT);
        }

        void start(float PosStart, EDIR dir)
        {   
            if(isMoving) return;

            PosStart = std::ceilf(PosStart);

            ASSERT(PosStart == int(PosStart)) ///<-----------------------: TODO.

            eDir = dir;
            
            this->position = PosStart;
            this->posStart = PosStart;
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
    /// Данные юзера, которые записываются в нод.
    ///----------------------------------------------------------- UserDataNode:
    struct GemData;

    using  igm_t = std::list<GemData>::iterator;
    std::ostream& operator<<(std::ostream& o, const std::vector<igm_t>&);

    using Gm_t = std::vector<std::vector<GemData*>>;
    std::ostream& operator<<(std::ostream& o, const Gm_t&);

    struct  GemData : Base
    {   
           ~GemData()
            {   if( node != nullptr )
                { //node -> destroyAllObjects();
                }
            }
        
        size_t             id{  NPOS}; /// Масть(тип) жемчужины.
        Ogre::SceneNode* node{nullptr}; /// Нод на котором висит жемчужена.
        Ogre::Entity*  entity{nullptr}; /// Геометрия + материал жемчужины.
        igm_t             igm;          /// Место аллокации этих данных.
        float       speed{50};
        Vector2i    pos2gm   ;          /// Позиция в зеркале.

        ///------------------------------|
        /// Отвязать камень от фигуры.   |
        ///------------------------------:
        void deLink()
        {   ASSERT(nullptr != node)
            Ogre::SceneNode*  parent = node->getParentSceneNode();
            ASSERT(nullptr != parent)
            parent->removeChild(node);
        }

        ///------------------------------|
        /// Отвязать камень от фигуры.   |
        ///------------------------------:
        void reset()
        {       id = NPOS   ;
              node = nullptr;
            entity = nullptr;
        }

        ///------------------------------|
        /// Анимация жемчужины.          |
        ///------------------------------:
        void update()
        {   
            if(ConfigGame::get().isGemAnimate)
            switch(id)
            {   case   3: node->yaw  (Ogre::Degree(speed * deltaTime)); break;
                case   4: node->pitch(Ogre::Degree(speed * deltaTime)); break;
                default:;
            }
        }

        ///------------------------------|
        /// Сколько нужно для match?     |
        ///------------------------------:
        static constexpr const uint8_t AMOUNTMATCH{3};

        enum ETYPEMATCH
        {   LV, /// Линия по вертикали.
            LG, /// Линия по горизонтали.
            L1, /// Диагональ стрелкой на 1 часа.
            L5  /// Диагональ стрелкой на 5 часов.
        };

        struct Match
        {
            void addLV(Match* P) { P->nType[LV]++; pp[0] = P; }
            void addLG(Match* P) { P->nType[LG]++; pp[1] = P; }
            void addL1(Match* P) { P->nType[L1]++; pp[2] = P; }
            void addL5(Match* P) { P->nType[L5]++; pp[3] = P; }

            Match* getLV() const { return pp[0]; }
            Match* getLG() const { return pp[1]; }
            Match* getL1() const { return pp[2]; }
            Match* getL5() const { return pp[3]; }

            void reset()
            {   for(auto& p : pp   ) p = this;
                for(auto& p : nType) p = 1   ;
            }

            bool isMatch() const
            {   return  pp[0]->nType[0] >= AMOUNTMATCH ||
                        pp[1]->nType[1] >= AMOUNTMATCH ||
                        pp[2]->nType[2] >= AMOUNTMATCH ||
                        pp[3]->nType[3] >= AMOUNTMATCH ;
            }

        private:
            std::array<uint8_t, 4> nType;
            std::array<Match* , 4> pp   ;
        }match;
    };

    ///------------------------------------------------------------------------|
    /// Gem
    ///-------------------------------------------------------------------- Gem:
    struct  Gem : GemData
    {           
 
        ///---------------------------------------|
        /// Стартовая инициализация.              |
        ///---------------------------------------:
        void setup(const size_t        n     ,
                   SceneNode*          parent,
                   const size_t        Id)
        {   
            GemData::id = Id;

            const auto& SZCELL{ConfigGame::get().sizeCell};

            if(nullptr != entity) clear();

            const auto& descriptions{ConfigGame::get().descriptionGems};

            static size_t N{0};

            entity = scnMgr->createEntity(
                std::format("Gem{}", N++),
                descriptions[Id].nameMesh
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

        ///---------------------------------------|
        /// Удаление жемчужины из фигуры.         |
        ///---------------------------------------:
        void clear()
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
    };


    ///------------------------------------------------------------------------|
    /// Figure
    ///----------------------------------------------------------------- Figure:
    struct  Figure   : Base
    {       Figure() : 
                gems(ConfigGame::get().descriptionGems.size())
            ,   mat (ConfigGame::get().descriptionGems.size())
            ,   step2DistanceB(ConfigGame::get().sizeCell)
            ,   step2Gravity  (ConfigGame::get().sizeCell)
            {   
            }

        std::vector<Gem>         gems;
        std::vector<MaterialPtr> mat ;
        SceneNode*               node; /// Нод фигуры!

    private:

        myl::Step2DistanceB step2DistanceB;
        myl::Step2DistanceB step2Gravity  ;

        const ConfigGame&         cfg{ConfigGame::get()};
        const float               D2 {cfg.sizeCell  / 2};

        struct 
        {   float  get() const   { return speedMoveCurr ; }
            void start(){ speedMoveCurr = speedMoveStart; }
            void    up(){ speedMoveCurr = speedMoveFast ; }

        private:
            float speedMoveStart{100.0f}; /// единиц в секунду.
            float speedMoveFast {300.0f};
            float speedMoveCurr {speedMoveStart};
        }speedFigFall;

        ///---------------------------------------|
        /// Стартовая инициализация.              |
        ///---------------------------------------:
        void setup(SceneNode* well)
        {   
            ///------------------------|
            /// Крепим к корзине.      |
            ///------------------------:
            node = well->createChildSceneNode();

            createMaterial();
            reGenerate    ();
        }

        void reGenerate()
        {   
            Ogre::Vector3 posStart(0, cfg.get().getWellH(), 0);

            node->setPosition(posStart);
            
            for(unsigned i{}; i < gems.size( ); ++i)
            {   
                const size_t rnd = rand() % mat.size();
                
                gems[i].setup(i, node, rnd);
                gems[i].entity->setMaterialName(mat[rnd]->getName());
            }

            using T = myl::Indexer;
        /// 
            node->setVisible(T::get().fooLookWay(T::ENONE));

            speedFigFall.start();
        }

        ///---------------------------------------|
        /// Физика.                               |
        ///---------------------------------------:
        const float groundLevel {0};
        bool        isFalling{true};

        const myl::Indexer& idexer{myl::Indexer::get()};

        void doGravity()
        {   /// l(node->getPosition().y)
            step2Gravity.startGravity(node->getPosition());
        }

        bool sensorCollisionY()
        {   isFalling = node->getPosition().y - D2 > groundLevel;
            return isFalling;
        }
        
        ///---------------------------------------|
        /// Вызывается для каждого фрейма(кадра). |
        ///---------------------------------------:
        void update()
        {   
            ///------------------------|
            /// Гравитация.            |
            ///------------------------:
            if(step2Gravity.sensor(speedFigFall.get() * deltaTime))
            {   const auto& p{node->getPosition()};
                node->setPosition(p.x, step2Gravity.getPosition(), p.z);
            }
            else 
            {   if(sensorCollisionY())
                {   
                    if(myl::Indexer::get().fooLookWay(myl::Indexer::EDOWN))
                    {   doGravity();
                    }
                    else isFalling = false;
                }
            }

            /// TODO: доделать ...
            if(step2DistanceB.sensor(deltaTime * speedMoving))
            {   const auto& p{node->getPosition()};
                node->setPosition(step2DistanceB.getPosition(), p.y, p.z);
            }
            else if(!isFalling && !step2DistanceB.isUserMoving())
            {   
                onGroundCollision();
                SNDSTOP(drop1);
            }
            
            ///------------------------|
            /// Анимация.              |
            ///------------------------:
            for(auto& e : gems) e.update();

        }

        ///---------------------------------------|
        /// Обработка клавиш.                     |
        ///---------------------------------------:
        float speedMoving{100.0f};
        bool  keyPressed (const KeyboardEvent& evt)
        {   
            switch(evt.keysym.sym)
            {
            case OgreBites::SDLK_UP:
                reShuffleGems();
                break;
                
            case OgreBites::SDLK_DOWN:
                speedFigFall.up();
                SNDPLAY(drop1);
                break;
                
            case OgreBites::SDLK_LEFT: /// 122: 'Z'
            {   
                const auto& p = node->getPosition();

                if(idexer.lookL(p) && idexer.fooLookWay(myl::Indexer::ELEFT))
                {   step2DistanceB.start(p.x, myl::Step2Distance::LEFT);
                    MUSPLAY(dart);
                }
                else MUSPLAY(wow1);
                break;
            }
            case OgreBites::SDLK_RIGHT: /// 120: 'X'
            {   const auto& p = node->getPosition();
                if(idexer.lookR(p) && idexer.fooLookWay(myl::Indexer::ERIGHT))
                {   step2DistanceB.start(p.x, myl::Step2Distance::RIGHT);
                    MUSPLAY(dart);
                }
                else MUSPLAY(wow1);
                break;
            } 
            default:
            /// l(evt.keysym.sym)
                return true; // Другие клавиши не обрабатываем
            }
            return false;
        }
        
        ///---------------------------------------|
        /// Матрериалы создаются один раз.        |
        ///---------------------------------------:
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

        ///----------------------------------------|
        /// Обработка столкновения фигуры с землей.|
        ///----------------------------------------:
        void onGroundCollision()
        {    sendFigure2Well();
             reGenerate();
             isFalling = true;
             MUSPLAY(wu);
        }

        ///----------------------------------------|
        /// Циклический сдвиг по колоне вверх на 1.|
        ///----------------------------------------:
        void reShuffleGems()
        {
            Ogre::Vector3 a{gems.front().node->getPosition()};

            for(size_t i{}, N{gems.size() - 1}; i < N; ++i)
            {   gems[i  ].node->setPosition(
                gems[i+1].node->getPosition());
            }

            gems.back().node->setPosition(a);
        }

        ///---------------------------------------|
        /// Сенсор столновения: true ---> бум!    |
        ///---------------------------------------:
        bool sensorCollisions(myl::Indexer::EDIR dir)
        {   return !idexer.fooLookWay(dir);
        }

        ///---------------------------------------|
        /// Делегат от логики корзины.            |
        ///---------------------------------------:
        std::function<void(Figure*)> delegate4Well;

        ///---------------------------------------|
        /// Отправка фигуры в корзину.            |
        ///---------------------------------------:
        void sendFigure2Well()
        {   delegate4Well(this);
        }

        void setVisibleGems(bool val)
        {   for(auto& gem : gems) gem.node->setVisible(val);
        }

        friend struct Well;
    };
}

#endif // FIGURE_H


