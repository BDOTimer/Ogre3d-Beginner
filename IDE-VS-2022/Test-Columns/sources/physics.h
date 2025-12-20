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
        {   return int(val + 1000 - 50) / SZCELL - 10;
        }

        enum    EDIR
        {       ENONE,
                ELEFT,
                ERIGHT,
                EDOWN,
                EUP
        };

        bool isIntroWold(const Ogre::Vector3& vf) const
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
                    vf.x, ///+ 50.f,
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

#endif // PHYSICS_H

