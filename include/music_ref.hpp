#ifndef MUSIC_REF_HPP_INCLUDED
#define MUSIC_REF_HPP_INCLUDED

class Music;

namespace salmon {
    class MusicRef {
        public:
            MusicRef(Music& impl);

            bool good() const;
            void play(int loops = -1);
            void play_fadein(int fadein_ms, int loops = -1);
            void play_fadein_pos(double seconds, int fadein_ms, int loops = -1);
            void pause();
            void resume();
            void halt();
            void halt_fadeout(int fadeout_ms);
            void rewind();
            void set_position(double seconds);

            float get_volume();
            void set_volume(float factor);
            bool playing();
            bool paused();
        private:
            Music* m_impl;
    };
}

#endif // MUSIC_REF_HPP_INCLUDED
