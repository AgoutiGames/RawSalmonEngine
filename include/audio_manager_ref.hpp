#ifndef AUDIO_MANAGER_REF_HPP_INCLUDED
#define AUDIO_MANAGER_REF_HPP_INCLUDED

#include <string>

#include "music_ref.hpp"
#include "sound_ref.hpp"

class AudioManager;

namespace salmon {
    class AudioManagerRef {
        public:
            AudioManagerRef(AudioManager& impl);

            bool load_music(std::string path);
            bool load_sound(std::string path);

            MusicRef get_music(std::string path);
            SoundRef get_sound(std::string path);

            void set_music_path(std::string path);
            void set_sound_path(std::string path);

            void set_master_sound_volume(float factor);
            float get_master_sound_volume();

            void clear_music();
            void clear_sounds();
            void clear();
        private:
            AudioManager* m_impl;
    };
}


#endif // AUDIO_MANAGER_REF_HPP_INCLUDED
