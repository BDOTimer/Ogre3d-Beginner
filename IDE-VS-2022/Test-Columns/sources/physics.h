///----------------------------------------------------------------------------|
/// "physics.h"
///----------------------------------------------------------------------------:
#ifndef PHYSICS_H
#define PHYSICS_H
#include "config-game.h"

///---------|
/// My Lib. |
///---------:
namespace myl
{   
    ///------------------------------------------------------------------------|
    /// 
    ///------------------------------------------------------------------------:
    struct  physics
    {       physics()
            {   
            }


        
    private:

    };

    ///------------------------------------------------------------------------|
    /// Индекстор точек для локальных координат фигуры.
    /// const myl::Indexer& idexer{myl::Indexer::get()};
    ///------------------------------------------------------------------------:
    struct  Indexer
    {       Indexer() : 
                cfg(ConfigGame::get())
            ,   SZCELL(int(cfg.sizeCell))
            ,   Wd2   (float(cfg.W/2)*cfg.sizeCell)
            ,   D     (100)
            ,   DDD   (float(SZCELL) * D)
            ,   WW    (Wd2 + DDD)
            ,   W     (cfg.W)
            ,   H     (cfg.H + cfg.getArrH())
            {   
            }

        const ConfigGame& cfg;
        const int      SZCELL; //
        const float       Wd2;
        const int           D; //
        const float       DDD; 
        const float        WW; //
        const int           W; /// Ширина  массива.
        const int           H; /// Выстота массива.
        
        static Indexer& get()
        {   static Indexer idexer; return idexer;
        }

        const Ogre::Vector3i getIndex3(const Ogre::Vector3& v) const
        {   return Ogre::Vector3i
            {       getIndexX(v.x),
                    getIndexY(v.y),
                0///getIndexZ(v.z)
            };
        }

        int getIndexX(const float val) const
        {   return int(val + WW) / SZCELL - D;
        }

        int getIndexY(const float val) const ///-/////////////////////////////-?
        {   return int(val + 2000 - 50) / SZCELL - 20;
        }

        enum    EDIR
        {       ENONE,
                ELEFT,
                ERIGHT,
                EDOWN,
                EUP
        };

        bool XisIntroWold(const Ogre::Vector3& vf) const
        {   const Ogre::Vector3i&& vi{getIndex3(vf)};
            return 0 <= vi[0] && vi[0] < W && 0 <= vi[1] && vi[1] < H;
        }

        ///-------------------------------------|
        /// Путь свободен?                      |
        ///-------------------------------------:
        bool lookL(const Ogre::Vector3& vf) const
        {   const Ogre::Vector3i&& vi{getIndex3(vf)};
            return 0 <= vi[0] - 1;
        }

        bool lookR(const Ogre::Vector3& vf) const
        {   const Ogre::Vector3i&& vi
            {   
                getIndex3({
                    vf.x,// + 50.f,
                    vf.y,
                    vf.z
                })
            };
            return vi[0] + 1 < W;
        }

        bool lookD(const Ogre::Vector3& vf) const
        {   const Ogre::Vector3i&& vi{getIndex3(vf)};
            return 0 <= vi[1] - 1;
        }

        ///-------------------------------------|
        /// Путь свободен?                      |
        /// Нужна отдельная инициализация!      |
        ///-------------------------------------:
        std::function<     bool(Indexer::EDIR)> fooLookWay
        {   []([[maybe_unused]] Indexer::EDIR a)->bool
            {   ASSERTM(false, "myl::Indexer::fooLookWay not init!\n")
                return  false;
            }
        };

        ///-------------------------------------|
        /// Тест настроен для W = 7             |
        /// Размер вокселя: 100.0f              |
        /// Для координаты X                    |          
        ///-------------------------------------:
        static void test()
        {   TestInfo testInfo("Indexer");
            
            std::vector<float> in
            {     99.9f,  100.0f,  100.1f,
                   0.1f,    0.0f, -000.1f,
                - 99.9f, -100.0f, -100.1f,
                -199.0f, -200.0f, -200.1f,
                -299.0f, -300.0f, -300.1f,
                -399.0f, -400.0f, -400.1f,
                -499.0f, -500.0f, -500.1f
            };

            std::vector<int> ver
            {   0,  1,  1,
                0,  0, -1,
               -1, -1, -2,
               -2, -2, -3,
               -3, -3, -4,
               -4, -4, -5,
               -5, -5, -6
            };

            for(size_t i{}; i < in.size(); ++i)
            {   
                int r{Indexer::get().getIndexX(in[i])};
                auto mess = r == ver[i] + 3 ? "GOOD!" : "Bad...";
                if(i%3 == 0) LN
                std::cout
                    << std::format("{:6} ---> {:2}   {}\n", in[i], r, mess);
            }
        }
        
    private:

    };
    
    inline void testPhysics()
    {
        Indexer::test();
    }
}


///---------|
/// Physics.|
///---------:
namespace phs
{
    ///------------------------------------------------------------------------|
    /// Стенд для тестирования преобразования float в индексы вокселей.
    ///------------------------------------------------------------------------:
    struct  Test$f2i
    {       Test$f2i() :
                cfg(ConfigGame::get())
            ,   W ((int)cfg.W)
            ,   H ((int)cfg.H)
            , CellSizeFloat(cfg.sizeCell )
            , CellSizeInt  ((int)CellSizeFloat)
            {   
            }

        const ConfigGame cfg;

        int   W;
        int   H;
        float CellSizeFloat;
        int   CellSizeInt  ;

        int xf2i(float x) const
        {   return int(x + CellSizeFloat * W) / CellSizeInt - W + W / 2;
        }

        int yf2i(float y) const
        {   return int(y + CellSizeFloat * H) / CellSizeInt - H;
        }

        static void testX()
        {   Test$f2i tt;

            for(float xf; true; )
            {   std::cout << "float x: ";
                std::cin  >> xf;
                std::cout << std::format(
                              "   xf: {:6} ---> xi {:4}\n", xf, tt.xf2i(xf));
            }
        }

        static void testY()
        {   Test$f2i tt;

            for(float yf; true; )
            {   std::cout << "float y: ";
                std::cin  >> yf;
                std::cout << std::format(
                              "   yf: {:6} ---> yi: {:4}\n", yf, tt.yf2i(yf));
            }
        }
    };


    ///------------------------------------------------------------------------|
    /// Collisions.
    /// Базовое соглашение: Сенсоры ВСЕГДА выровнены по сетке.
    /// 
    ///  sensor1         sensor2  
    ///     q---------------p
    ///     |               |
    ///     |               |
    ///     |               |
    ///     |               |
    ///     d---------------b
    ///  sensor0         sensor3
    ///------------------------------------------------------------- Collisions:
    struct Collisions : Test$f2i
    {
        static const Collisions& get(){ static Collisions cln; return cln; }

        ///-------------------------------------|
        /// Путь свободен?                      |
        /// Нужна отдельная инициализация!      |
        ///-------------------------------------:
        std::function<     bool(int w, int h)> fooLookWay
        {   []([[maybe_unused]] int w, 
               [[maybe_unused]] int h)->bool
            {   ASSERTM(false, "phs::Collisions::fooLookWay not init!\n")
                return  false;
            }
        };

        bool isLeft(const Ogre::Vector3& pos) const
        {   
            ///------------|
            /// Sensor - 0 |
            ///------------:
            {   const int x = xf2i(pos.x);
                const int y = yf2i(pos.y);

                if(!fooLookWay(x, y)) return false;
            }
            ///------------|
            /// Sensor - 1 |
            ///------------:
            {   const int x = xf2i(pos.x);
                const int y = yf2i(pos.y + CellSizeFloat);

                if(!fooLookWay(x, y)) return false;
            }
            return true;
        }

        bool isRight(const Ogre::Vector3& pos) const
        {   
            /// x, y - индексы текущего размещения фигуры.

            ///------------|
            /// Sensor - 0 |
            ///------------:
            {   const int x = xf2i(pos.x);
                const int y = yf2i(pos.y);

                if(!fooLookWay(x, y)) return false;
            }
            ///------------|
            /// Sensor - 1 |
            ///------------:
            {   const int x = xf2i(pos.x);
                const int y = yf2i(pos.y + CellSizeFloat);

                if(!fooLookWay(x, y)) return false;
            }
            return true;
        }

        bool isDown(const Ogre::Vector3& pos) const
        {   ///------------|
            /// Sensor - 0 |
            ///------------:
            {   const int x = xf2i(pos.x);
                const int y = yf2i(pos.y);

                if(!fooLookWay(x, y)) return false;
            }
            ///------------|
            /// Sensor - 3 |
            ///------------:
            {   const int x = xf2i(pos.x + CellSizeFloat);
                const int y = yf2i(pos.y);

                if(!fooLookWay(x, y)) return false;
            }
            return true;
        }
    };


    ///------------------------------------------------------------------------|
    /// Stepper. Включение только после проверки свободного пути!
    /// 
    /// Контроль одного значения.
    /// Использование в update(float dt):
    ///     if(stepper.isActive)
    ///     {   float x = stepper.update(dt);
    ///         const auto& p = figure.getPosition();
    ///         figure.setPosition(x, p.y, p.z);
    /// 
    ///         ...?
    ///     }
    ///---------------------------------------------------------------- Stepper:
    struct  Stepper
    {       Stepper() : Distance(cln.CellSizeFloat)
            {   
            }

        ///-------------------------------------|
        /// Координата фигуры: x или y          |
        /// Полный контроль!                    |
        /// Вызвать на старте фигуры!           |
        ///-------------------------------------:
        void reset(float xy)
        {   pos = xy;
        }

        const Collisions cln{ Collisions::get() };

        float speed{100.f};

        void start(const float val)
        {   if(isActive) return;
            
            isActive = true;
            pos      = val ;
            tmp      =   0 ;

            dir = val < 0.f ? -1.f : 1.f;
        }

        float update(float   dt)
        {   tmp += getSpeed (dt);
            
            ///--------------|
            /// СТОП!        |
            ///--------------:
            if(tmp > Distance)
            {   isActive = false;
                pos += dir * Distance;
                return pos;
            } 
            return dir * tmp + pos;
        }

        float getSpeed(float dt)
        {   float  spd     { dt * speed };
            return spd <= cln.CellSizeFloat  ? spd : cln.CellSizeFloat + 1;
        }

              float      dir; /// Направление движения.
              float      tmp; /// Всегда > 0 !
              float      pos; /// Только операции(+-) с Distance!
        const float Distance; /// Шаг движения.

        bool isActive{false};

        static void test()
        {   Stepper stepper;
        }

    private:
        void initStep()
        {   
        }
    };

    void inline tests()
    {
        //Test$f2i::testX();
        //Test$f2i::testY();

        Stepper stepper;
    }
}

#endif // PHYSICS_H

