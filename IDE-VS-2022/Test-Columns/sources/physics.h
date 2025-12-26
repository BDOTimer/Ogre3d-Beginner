///----------------------------------------------------------------------------|
/// "physics.h"
///----------------------------------------------------------------------------:
#ifndef PHYSICS_H
#define PHYSICS_H
#include "config-game.h"

///---------|
/// Physics.|
///---------:
namespace phs
{
    ///------------------------------------------------------------------------|
    /// Стенд для тестирования преобразования float в индексы вокселей.
    /// Позиции Gems приподняты по Y на +sizeCell / 2
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

        Ogre::Vector3i getIndex3(const Ogre::Vector3& v) const
        {   return 
            {   xf2i(v.x),
                yf2i(v.y),
                int (v.z)
            };
        }

        ///-------------------------------------|
        /// Путь свободен?                      |
        ///-------------------------------------:
        bool isLeft(const Ogre::Vector3& pos) const
        {   
            const int x = xf2i(pos.x) - 1;

            ///------------|
            /// Sensor - 0 |
            ///------------:
            {   const int y = yf2i(pos.y);

                if(!fooLookWay(x, y)) return false;
            }
            ///------------|
            /// Sensor - 1 |
            ///------------:
            {   const int y = yf2i(pos.y) + 1;

                if(!fooLookWay(x, y)) return false;
            }
            return true;
        }

        bool isRight(const Ogre::Vector3& pos) const
        {   
            const int x = xf2i(pos.x)  + 1;

            ///------------|
            /// Sensor - 0 |
            ///------------:
            {   const int y = yf2i(pos.y);

                if(!fooLookWay(x, y)) return false;
            }
            ///------------|
            /// Sensor - 1 |
            ///------------:
            {   const int y = yf2i(pos.y) + 1;

                if(!fooLookWay(x, y)) return false;
            }
            return true;
        }

        bool isDown(const Ogre::Vector3& pos, bool isActive) const
        {   
            const int y = yf2i(pos.y) - 1;

            ///------------|
            /// Sensor - 0 |
            ///------------:
            {   const int x = xf2i(pos.x);

                if(!fooLookWay(x, y)) return false;
            }

            if(!isActive) return true;

            ///------------|
            /// Sensor - 3 |
            ///------------:
            {   const int x = xf2i(pos.x) + 1;

                if(!fooLookWay(x, y)) return false;
            }
            return true;
        }

        bool isHereEmpty(const Ogre::Vector3& pos) const
        {   return fooLookWay(xf2i(pos.x), yf2i(pos.y));
        }

        static void test()
        {
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
    {       Stepper() : Distance(ConfigGame::get().sizeCell)
            {   
            }
            Stepper(float val) : Distance(ConfigGame::get().sizeCell), pos(val)
            {   
            }

        ///-------------------------------------|
        /// Координата фигуры: x или y          |
        /// Полный контроль!                    |
        /// Вызвать на старте фигуры!           |
        ///-------------------------------------:
        void setup(float xy)
        {   pos = xy;
            tmp =  0;
        }

        void start(const float val)
        {   if(isActive) return;
            
            isActive = true;

            dir = val < 0.f ? -1.f : 1.f;
        }

        void endMoved()
        {   tmp =  0;
        }

        float update(float   dt)
        {   tmp += getSpeed (dt);

            if(isDebug)
            {   l(Distance)
                l(tmp)
            }

            ///--------------|
            /// СТОП!        |
            ///--------------:
            if(tmp > Distance)
            {   isActive = false;
                pos += dir * Distance;
                tmp -= Distance;
                return pos;
            } 
            return dir * tmp + pos;
        }

        float getSpeed(float dt) const
        {   return dt <= Distance  ? dt : Distance + 1;
        }

        const float Distance   ; /// Шаг движения.
              float      pos   ; /// Только операции(+-) с Distance!
              float      tmp{0}; /// Всегда > 0 !
              float      dir   ; /// Направление движения.
        
        bool isActive{false};

        void doDebug() { isDebug = true; }

        static void test()
        {   Stepper stepper;
        }

    private:
        void initStep()
        {   
        }

        bool isDebug{false};
    };

    void inline tests()
    {
        //Test$f2i::testX();
        //Test$f2i::testY();
    }
}

#endif // PHYSICS_H

