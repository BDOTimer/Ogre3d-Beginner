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
    /// Sound::get().wu();
    /// https://freesoundsite.com/sound-effect-generator/
    ///------------------------------------------------------------------------:
    struct Sound
    {
    /// const sf::SoundBuffer buffer{"resources/uw.mp3"};

        sf::Music _wu   {"sound/wu.mp3"  };
        sf::Music _wow1 {"sound/wow1.mp3"};
        sf::Music _dart {"sound/dart.mp3"};

        static Sound& get()
        {   static Sound a; return a;
        }

        void wu  (){ _wu  .play(); }
        void wow1(){ _wow1.play(); }
        void dart(){ _dart.play(); }
    };
}

#endif // SOUND_H

