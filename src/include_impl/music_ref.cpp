#include "music_ref.hpp"

#include "audio/music.hpp"

using salmon::MusicRef;

MusicRef::MusicRef(Music& impl) : m_impl{&impl} {}

bool MusicRef::good() const {return m_impl->good();}
void MusicRef::play(int loops) {m_impl->play(loops);}
void MusicRef::play_fadein(int fadein_ms, int loops) {m_impl->play_fadein(fadein_ms,loops);}
void MusicRef::play_fadein_pos(double seconds, int fadein_ms, int loops) {m_impl->play_fadein_pos(seconds,fadein_ms,loops);}
void MusicRef::pause() {m_impl->pause();}
void MusicRef::resume() {m_impl->resume();}
void MusicRef::halt() {m_impl->halt();}
void MusicRef::halt_fadeout(int fadeout_ms) {m_impl->halt_fadeout(fadeout_ms);}
void MusicRef::rewind() {m_impl->rewind();}
void MusicRef::set_position(double seconds) {m_impl->set_position(seconds);}

float MusicRef::get_volume() {return m_impl->get_volume();}
void MusicRef::set_volume(float factor) {m_impl->set_volume(factor);}
bool MusicRef::playing() {return m_impl->playing();}
bool MusicRef::paused() {return m_impl->paused();}
