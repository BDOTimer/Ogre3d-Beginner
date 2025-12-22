///----------------------------------------------------------------------------|
/// "figure.h"
///----------------------------------------------------------------------------:
#ifndef FIGURE_H
#define FIGURE_H
#include "primitives.h"
#include "physics.h"

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

    struct  GemData : Glob
    {   
           ~GemData()
            {   if( node != nullptr )
                { //node -> destroyAllObjects();
                }
            }
        
        size_t             id   {  NPOS}; /// Масть(тип) жемчужины.
        Ogre::SceneNode* node   {nullptr}; /// Нод на котором висит жемчужена.
        Ogre::Entity*  entity   {nullptr}; /// Геометрия + материал жемчужины.
        igm_t             igm;             /// Место аллокации этих данных.
        float       speed{50};
        Vector2i    pos2gm   ;             /// Позиция в зеркале.

        /// not use ...
        GemData**       pcell   {nullptr}; /// Указатель на указатель 

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
        {          id = NPOS   ;
              node    = nullptr;
            entity    = nullptr;
        }

        ///------------------------------|
        /// Анимация жемчужины.          |
        ///------------------------------:
        void update()
        {   
            if(node == nullptr) return;

            if(match.isMatch == 0) updateGravitate();///-///////////////////////

            const float SPEED{speed * deltaTime};

            if(ConfigGame::get().isGemAnimate)
            switch(id)
            {   case   3: node->yaw  (Ogre::Degree(SPEED)); break;
                case   4: node->pitch(Ogre::Degree(SPEED)); break;
                default:;
            }

            if(match.isMatch == 1)
            {   float S = 1.f - 2.f * deltaTime;
                if(S > 0.005f && node->getScale().x > 0.1f)
                {   node->scale({S, S, S});
                }
                else
                {   match.isMatch = 2;
                /// node->setVisible(false);

                    ///--------------------------------|
                    /// Удаление камня из колодца.     |
                    ///--------------------------------:-/////////////////////-!
                    (*pcell) = nullptr;
                    deLink();
                    node = nullptr;
                    EraseIt(igm);
                }
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

            int isMatch{0};

            bool doIsMatch()
            {   isMatch
                =   pp[0]->nType[0] >= AMOUNTMATCH ||
                    pp[1]->nType[1] >= AMOUNTMATCH ||
                    pp[2]->nType[2] >= AMOUNTMATCH ||
                    pp[3]->nType[3] >= AMOUNTMATCH ;
                return isMatch;
            }

        private:
            std::array<uint8_t, 4> nType;
            std::array<Match* , 4> pp   ;
        }match;

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

        ///---------------------------------------|
        /// Гравитация.                           |
        ///---------------------------------------:
        phs::Stepper* steperGrav{nullptr};
        bool          isGrav    {false  };

        void setupGravitate(phs::Stepper* grav)
        {   ASSERT(nullptr != node)
            const auto& posFig = node->getPosition();
            steperGrav = grav;
            steperGrav->reset(posFig.y);
            isGrav = true;
        }

        void updateGravitate()
        {   if(!isGrav) return;

            const ConfigGame& cfg{ConfigGame::get()};
            const phs::Collisions& collisions{phs::Collisions::get()};

            const auto& posFig = node->getPosition();

            if(steperGrav->isActive)
            {   float y = steperGrav->update(Glob::deltaTime * 100.f);

                node->setPosition(posFig.x, y, posFig.z);
            }
            else if(collisions.isDown(
                        {posFig.x, posFig.y - 50.f, posFig.z}, false))
            {   steperGrav->start(-cfg.sizeCell);

                setGem2Well (this);
            }
        }

    private:
        void EraseIt    (igm_t    igm);
        void setGem2Well(GemData* gem);
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

            const auto name{std::format("Gem{}", N++)};

            entity = scnMgr->createEntity(
                name,
                descriptions[Id].nameMesh
            );

            if(nullptr == node)
            {   node   =  parent->createChildSceneNode(name);
            }
            node->resetOrientation  ();
            node->attachObject(entity);
            node->setPosition (0, SZCELL / 2 + (SZCELL * float(n)), 0);
            node->setScale    (descriptions[id].scale);

            if(id % 2) speed = -speed;
        }
    };


    ///------------------------------------------------------------------------|
    /// Figure
    ///----------------------------------------------------------------- Figure:
    struct  Figure   : Glob
    {       Figure() : 
                gems(ConfigGame::get().N)
            ,   mat (ConfigGame::get().descriptionGems.size())
            {   
            }

        std::vector<Gem>         gems;
        std::vector<MaterialPtr> mat ;
        SceneNode*               node; /// Нод фигуры!

    private:

        phs::Stepper steperLR  ;
        phs::Stepper steperGrav;

        const ConfigGame& cfg{ConfigGame::get()};
        const float       D2 {cfg.sizeCell  / 2};

        struct 
        {   float  get() const   { return speedMoveCurr ; }
            void start(){ speedMoveCurr = speedMoveStart; }
            void    up(){ speedMoveCurr = speedMoveFast ; }

        private:
            float speedMoveStart{100.0f}; /// единиц в секунду.
            float speedMoveFast {300.0f};
            float speedMoveCurr {speedMoveStart};
        }speedFall;

        ///---------------------------------------|
        /// Стартовая инициализация.              |
        ///---------------------------------------:
        void setup(SceneNode* well)
        {   
            ///------------------------|
            /// Крепим к корзине.      |
            ///------------------------:
            node = well->createChildSceneNode("Figure");

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

            speedFall.start();

            steperLR  .reset(posStart.x);
            steperGrav.reset(posStart.y);

            node->setVisible(collisions.isHereEmpty(posStart));
        }

        ///---------------------------------------|
        /// Физика.                               |
        ///---------------------------------------:
        bool  isFalling{true};
        float speedLR{200.0f};

        const phs::Collisions& collisions{phs::Collisions::get()};
        
        ///---------------------------------------|
        /// Вызывается для каждого фрейма(кадра). |
        ///---------------------------------------:
        void update()
        {   
            const auto& posFig = node->getPosition();

            ///-------------------|
            /// LR.               |
            ///-------------------:
            if(steperLR.isActive)
            {   float x = steperLR.update(Glob::deltaTime * speedLR);
                node->setPosition(x, posFig.y, posFig.z);

                isFalling = true;
            }

            ///-------------------|
            /// Гравитация.       |
            ///-------------------:
            if(steperGrav.isActive)
            {   float y = 
                    steperGrav.update(Glob::deltaTime * speedFall.get());

                node->setPosition(posFig.x, y, posFig.z);
            }
            else if(isFalling &&
                    collisions.isDown(posFig, steperLR.isActive))
            {   
                steperGrav.start(-cfg.sizeCell);
                antiBugInjectionY();
            }
            else isFalling = false;

            ///-------------------|
            /// Тишина...         |
            ///-------------------:
            if(!isFalling && !steperLR.isActive)
            {   onGroundCollision();
                SNDSTOP(drop1);
            }
            
            ///-------------------|
            /// Анимация.         |
            ///-------------------:
            for(auto& e : gems) e.update();
        }

        ///---------------------------------------|
        /// Обработка клавиш.                     |
        ///---------------------------------------:
        bool  keyPressed (const KeyboardEvent& evt)
        {   
            switch(evt.keysym.sym)
            {
            case OgreBites::SDLK_UP:
                reShuffleGems();
                break;
                
            case OgreBites::SDLK_DOWN:
                speedFall.up();
                SNDPLAY(drop1);
                break;
                
            case OgreBites::SDLK_LEFT:
            {   
                const auto& p = node->getPosition();

                if(collisions.isLeft(p))
                {   steperLR .start (-cfg.sizeCell);
                    antiBugInjectionX();
                    MUSPLAY(dart);
                }
                else MUSPLAY(wow1);
                break;
            }
            case OgreBites::SDLK_RIGHT:
            {   const auto& p = node->getPosition();

                if(collisions.isRight(p))
                {   steperLR .start (cfg.sizeCell);
                    antiBugInjectionX();
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
                auto name{std::format("matSph{}", i)};

                mat[i] = Ogre::MaterialManager::getSingleton().getByName(name);

                if(mat[i] != nullptr) continue;

                mat[i] = MaterialManager::getSingleton().create(
                    name,
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
        {    
            sendFigure2Well();
            reGenerate     ();

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

        void antiBugInjectionX()
        {   float x = steperLR.update(0.01f);
            const auto& posFig = node->getPosition();
            node->setPosition(x, posFig.y, posFig.z);
        }

        void antiBugInjectionY()
        {   float y = steperGrav.update(0.01f);
            const auto& posFig = node->getPosition();
            node->setPosition(posFig.x, y, posFig.z);
        }

        friend struct Well;
    };


    ///------------------------------------------------------------------------|
    /// Жемчужины, которые нужно удалить из колодца.
    ///-------------------------------------------------------------- GemsMatch:
    struct  GemsMatch : Glob, std::vector<igm_t>
    {       GemsMatch()
            {   reserve(128);
            }

        void debug()
        {   std::vector<igm_t>& gemsMatch{*this};

            ///----------------|
            /// Дебаг.         |
            ///----------------:
            if(!empty()) {l(gemsMatch.size())ln(gemsMatch)}
        }

    private:

    };
}

#endif // FIGURE_H


