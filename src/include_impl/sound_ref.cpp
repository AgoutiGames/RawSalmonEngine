#include "sound_ref.hpp"

#include "audio/sound_effect.hpp"

using salmon::SoundRef;

SoundRef::SoundRef() : m_impl{SoundEffect::get_null_sound()} {}
SoundRef::SoundRef(SoundEffect& impl) : m_impl{&impl} {}

bool SoundRef::good() const {return m_impl->good();}
void SoundRef::play(int loops, int length_ms) {m_impl->play(loops,length_ms);}
void SoundRef::play_fadein(int fadein_ms, int loops, int length_ms) {m_impl->play_fadein(fadein_ms,loops,length_ms);}
void SoundRef::pause() {m_impl->pause();}
void SoundRef::resume() {m_impl->resume();}
void SoundRef::halt() {m_impl->halt();}
void SoundRef::halt_fadeout(int fadeout_ms) {m_impl->halt_fadeout(fadeout_ms);}

void SoundRef::set_position(int angle, float distance) {m_impl->set_position(angle,distance);}
void SoundRef::reset_position() {m_impl->reset_position();}

float SoundRef::get_volume() {return m_impl->get_volume();}
void SoundRef::set_volume(float factor) {m_impl->set_volume(factor);}
bool SoundRef::playing() {return m_impl->playing();}
bool SoundRef::paused() {return m_impl->paused();}
