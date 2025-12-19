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
    /// Play a music:
    /// Musics::get().wu.play();
    /// Musics::get().wu.stop();
    /// 
    /// https://freesoundsite.com/sound-effect-generator/
    /// https://zvukipro.com/predmet/page/2/
    /// https://vsezvuki.com/1-zvuki-shumy-i-effekty/category_1257_10/
    ///------------------------------------------------------------------------:
    struct Musics
    {
    /// const sf::SoundBuffer buffer{"resources/uw.mp3"};

        sf::Music wu   {"sound/wu.mp3"  };
        sf::Music wow1 {"sound/wow1.mp3"};
        sf::Music dart {"sound/dart.mp3"};

        static Musics& get()
        {   static Musics a; return a;
        }
    };

    ///------------------------------------------------------------------------|
    /// Sound::get().wu.play();
    ///------------------------------------------------------------------------:
    struct  Sound
    {       Sound()
            {   drop1.setLooping(true );
                drop1.setVolume (10.0f);
            }


        const sf::SoundBuffer _wu{"sound/wu.mp3"};
              sf::Sound    wu{_wu};

        const sf::SoundBuffer _drop1{"sound/drop1.mp3"};
              sf::Sound drop1{_drop1};

        static Sound& get()
        {   static Sound a; return a;
        }
    };
}

#define MUSPLAY(a) if (Musics::get().a.getStatus() != sf::Music::Status::Playing)\
                       Musics::get().a.play()
#define MUSSTOP(a)     Musics::get().a.stop()

#define SNDPLAY(a) if (Sound::get().a.getStatus() != sf::Sound::Status::Playing)\
                       Sound::get().a.play()
#define SNDSTOP(a)     Sound::get().a.stop()

#endif // SOUND_H

