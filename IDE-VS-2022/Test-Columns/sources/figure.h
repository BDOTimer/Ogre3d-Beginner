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
    struct WellLogic;
    struct GemData;

    using  igm_t = std::list<GemData>::iterator;
    std::ostream& operator<<(std::ostream& o, const std::vector<igm_t>&);

    using Gm_t = std::vector<std::vector<GemData*>>;
    std::ostream& operator<<(std::ostream& o, const Gm_t&);

    struct  GemData : Glob
    {   
           ~GemData()
            {   if( nullptr != steperGrav)
                {   delete    (steperGrav);
                }
            }
        
        WellLogic* pwellLogic   {nullptr};
        size_t             id   {  NPOS }; /// Масть(тип) жемчужины.
        Ogre::SceneNode* node   {nullptr}; /// Нод на котором висит жемчужена.
        Ogre::Entity*  entity   {nullptr}; /// Геометрия + материал жемчужины.
        igm_t             igm;             /// Место аллокации этих данных.
        float       speed{50};
        Vector2i    pos2gm   ;             /// Позиция в зеркале.

        void setupGravitate(phs::Stepper* grav, phs::Collisions* cln)
        {   ASSERT(nullptr != node)
            steperGrav = grav;
            ASSERT(nullptr != steperGrav)
            isGrav = true;

            collisions = cln;
        }

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

        bool isLive() const
        {   return node != nullptr;
        }

        ///------------------------------|
        /// Танец жемчужины.             |
        ///------------------------------:
        void animate()
        {
            const float SPEED{speed * deltaTime};

            if(ConfigGame::get().isGemAnimate)
            switch(id)
            {   case   3: node->yaw  (Ogre::Degree(SPEED)); break;
                case   4: node->pitch(Ogre::Degree(SPEED)); break;
                case   5: node->roll (Ogre::Degree(SPEED));
                          node->pitch(Ogre::Degree(SPEED));
                    break;
                default:;
            }
        }

        ///------------------------------|
        /// Анимация жемчужины.          |
        ///------------------------------:
        bool update()
        {   
            ASSERTM(node != nullptr, "Мёртвые с косами стоят...")

            animate();

            switch(match.isMatch)
            {   case  0: updateGravitate(); break;
                case  1:
                {   
                    if( float S = 1.f - 2.f * deltaTime;
                        S > 0.005f && node->getScale().x > 0.1f)
                    {   
                        ///---------------------------|
                        /// Анимация Matching.        |
                        ///---------------------------:
                        node->scale({S, S, S});
                    }
                    else
                    {   ///---------------------------|
                        /// Анимация закончилась:     |
                        /// удаление камня из колодца.|
                        ///---------------------------:
                        match.isMatch = 2;
                    
                        ///-------|
                        /// Debug.|
                        ///-------:
                        if(false)
                        {   
                            static int cntDead{};
                            LN
                            l1(std::format("Удалён Cnt: {}\n", ++cntDead))
                            l(pos2gm)
                        }

                        deLink();
                        scnMgr->destroySceneNode(node);
                        node = nullptr;
                    }
                    break;
                }
                default: ASSERT(false);
            }

            return steperGrav->isActive;
        }

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
            {   const unsigned& AM = ConfigGame::get().AMOUNTMATCH;
                isMatch
                =   pp[0]->nType[0] >= AM ||
                    pp[1]->nType[1] >= AM ||
                    pp[2]->nType[2] >= AM ||
                    pp[3]->nType[3] >= AM ;
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

    private:
        ///---------------------------------------|
        /// Гравитация.                           |
        ///---------------------------------------:
        phs::Collisions* collisions{nullptr};
        phs::Stepper*    steperGrav{nullptr};
        bool             isGrav    {false  };

        void updateGravitate()
        {   if(!isGrav) return;

            const ::ConfigGame& cfg{ ::ConfigGame::get() };
            
            const auto& posGem = node->getPosition();

            if(steperGrav->isActive)
            {   
                float y = steperGrav->update(Glob::deltaTime * 150.f);

                node->setPosition(posGem.x, y, posGem.z);

                ///---------------------|
                /// Шаг закочен!        |
                ///---------------------:
                if(!steperGrav->isActive)
                {   
                    checkAssert("Финиш");
                }
            }
            else if(collisions->isDown(
                        {posGem.x, posGem.y - 50.f, posGem.z}, false))
            {   steperGrav->start(-cfg.sizeCell);

                checkAssert("Старт", true);
                
                setGem2Well(this);
            }
        }

        void setGem2Well(GemData* gem);

        int cntStart{};

        void checkAssert(const char* mess, bool isStart = false)
        {   
            const auto& posGem = node->getPosition();

            const int x = pos2gm[0];
            const int y = pos2gm[1];

            Ogre::Vector3i _vi_{collisions->getIndex3(
                {   posGem.x,
                    posGem.y - 50.f,
                    posGem.z
                })
            };

            using namespace std;

            ///---------------------|
            /// Дебаг.              |
            ///---------------------:
            if(false)
            {   
                if(isStart) SIG(++cntStart)
                
                Ogre::Vector3i wait{x , y, (int)posGem.z};
                
                LN
                l2(format("{} {}: ", mess, "posGem: "), posGem)
                l2(format("{} {}: ", mess, "pos2gm: "), pos2gm)
                l(wait)
                l(_vi_)
            }

            static constexpr const char* sErr{"Позиция {} деградировала!"};
            ASSERTM(_vi_[0] == x, format(sErr, "X"))
            ASSERTM(_vi_[1] == y, format(sErr, "Y"))
            ASSERTM(steperGrav->pos == posGem.y, "steperGrav bad setup")
        }
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
    struct  Figure              : Glob
    {       Figure(unsigned id) : 
                id         (id)
            ,   gems(ConfigGame::get().N)
            ,   mat (ConfigGame::get().descriptionGems.size())
            {   
            }

        unsigned                 id  ;
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
        void setup(SceneNode* well, phs::Collisions* cln)
        {   
            collisions = cln;

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
                const size_t rnd{cfg.rndGen()};
                
                gems[i].setup(i, node, rnd);
                gems[i].entity->setMaterialName(mat[rnd]->getName());
            }

            speedFall.start();

            steperLR  .setup(posStart.x);
            steperGrav.setup(posStart.y);

            node->setVisible(collisions->isHereEmpty(posStart));
        }

        ///---------------------------------------|
        /// Физика.                               |
        ///---------------------------------------:
        bool  isFalling{true};
        float speedLR{200.0f};

        phs::Collisions* collisions{nullptr};
        
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
                    collisions->isDown(posFig, steperLR.isActive))
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
                Sound::get().drop1s[id]->stop();
            }
            
            ///-------------------|
            /// Анимация.         |
            ///-------------------:
            for(auto& e : gems) e.animate();
        }

        ///---------------------------------------|
        /// Карта управления.                     |
        ///---------------------------------------:
        inline static constexpr std::array<size_t, 4> userKeys[2]
        {
            {   OgreBites::SDLK_UP, 
                OgreBites::SDLK_DOWN, 
                OgreBites::SDLK_LEFT,
                OgreBites::SDLK_RIGHT
            },
            { 'w', 's', 'a', 'd'}
        };

        void  keyPressed (const KeyboardEvent& evt)
        {   
            const auto& K{evt.keysym.sym};

            if(K == userKeys[id][0])
            {   
                reShuffleGems();
                SNDPLAY(sony2);
            }
                
            else if(K == userKeys[id][1])
            {
                speedFall.up();
                Sound::get().drop1s[id]->play();
            }
                
            else if(K == userKeys[id][2])
            {
                const auto& p = node->getPosition();

                if(collisions->isLeft(p))
                {   steperLR .start (-cfg.sizeCell);
                    antiBugInjectionX();
                    MUSPLAY(dart);
                }
                else MUSPLAY(wow1);
            }

            else if(K == userKeys[id][3])
            {
                const auto& p = node->getPosition();

                if(collisions->isRight(p))
                {   steperLR .start (cfg.sizeCell);
                    antiBugInjectionX();
                    MUSPLAY(dart);
                }
                else MUSPLAY(wow1);
            }
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
                        ///p->setAmbient (ColourValue(0.4f));
                        p->setAmbient (ColourValue(0.6f, 0.5f, 0.5f));
                        p->setSpecular(ColourValue(0.9f));
                        p->setShininess(48.0f);
                    /// p->setEmissive(ColourValue(0.2f,0.1f,0.05f));
                    /// p->setSelfIllumination(ColourValue(0.1f,0.1f,0.1f,1));
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
            Sound::get().wus[id]->play();
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

        void check4Erase(std::list<GemData>& allocator) const
        {   const std::vector<igm_t>& gemsMatch{*this};
            int cntErase{};
            const size_t sz = allocator.size();
            for(const auto& it : gemsMatch)
            {   if(it->node == nullptr)
                {   allocator.erase(it); ++cntErase;
                }
            }
            if(cntErase)
            {   SIG("GemsMatch::check4Erase(.)")
                l1(std::format("Было кол-во Gem в allocator: {}\n", sz      ))
                l1(std::format("Удалено Gem из allocator   : {}\n", cntErase))
                l(allocator.size())
            } 
        }

    private:

    };
}

#endif // FIGURE_H


