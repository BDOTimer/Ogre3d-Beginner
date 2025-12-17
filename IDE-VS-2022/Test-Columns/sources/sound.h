///----------------------------------------------------------------------------|
/// "sound.h"
///----------------------------------------------------------------------------:
#ifndef SOUND_H
#define SOUND_H
/// #define SFML_STATIC ///<--- в настройках компилятора.
#include "SFML/Audio.hpp"


///---------|
/// Models. |
///---------:
namespace mdl
{
    ///------------------------------------------------------------------------|
    /// Play a sound
    ///------------------------------------------------------------------------:
    struct Sound
    {
        //const sf::SoundBuffer buffer{"resources/uw.mp3"};

        sf::Music music{"sound/wu.mp3"};

        void playSound()
        {   music.play();
        }

        static void test()
        {   static Sound a; a.playSound();
        }
    };
}

#endif // SOUND_H

