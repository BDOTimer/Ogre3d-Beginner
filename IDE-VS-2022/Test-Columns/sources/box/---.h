///----------------------------------------------------------------------------|
/// "xxx.h"
///----------------------------------------------------------------------------:
#ifndef X_H
#define X_H
#include "config-game.h"

//namespace myl
//{
//    ///------------------------------------------------------------------------|
//    /// myl::Fps
//    /// myl::Fps::get().update(deltatime);
//    ///-------------------------------------------------------------------- Fps:
//    struct Fps
//    {
//        static Fps& get(){ static Fps fps; return fps; }
//
//        void update(const float dt)
//        {   now += dt;
//            cnt ++;
//
//            if( now >= 1.f)
//            {   now -= 1.f;
//                fps  = cnt;
//                cnt  =   0;
//                delegate();
//            }
//        }
//
//        void setDelegate(std::function<void()> foo)
//        {   delegate = foo;
//        }
//
//    private:
//        float now{};
//        int   cnt{};
//        int   fps{};
//
//        std::function<void()> test
//        {   [this]()
//            {   std::cout 
//                    << std::format("\rFPS: {}{}", fps, std::string(15, ' '));
//            }
//        };
//
//        std::function<void()> delegate{test};
//    };
//}

#endif // X_H

