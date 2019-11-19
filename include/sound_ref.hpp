#ifndef SOUND_REF_HPP_INCLUDED
#define SOUND_REF_HPP_INCLUDED

class SoundEffect;

namespace salmon {
    class SoundRef {
        public:
            SoundRef(SoundEffect& impl);

            bool good() const;
            void play(int loops = 0, int length_ms = -1);
            void play_fadein(int fadein_ms, int loops = 0, int length_ms = -1);
            void pause();
            void resume();
            void halt();
            void halt_fadeout(int fadeout_ms);

            void set_position(int angle, float distance = 0);
            void reset_position();

            float get_volume();
            void set_volume(float factor);
            bool playing();
            bool paused();
        private:
            SoundEffect* m_impl;
    };
}

#endif // SOUND_REF_HPP_INCLUDED
