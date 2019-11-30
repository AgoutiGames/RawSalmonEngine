#include "audio_manager_ref.hpp"

#include "audio/audio_manager.hpp"

using salmon::AudioManagerRef;

AudioManagerRef::AudioManagerRef(AudioManager& impl) : m_impl{&impl} {}

bool AudioManagerRef::load_music(std::string path, bool absolute) {return m_impl->load_music(path,absolute);}
bool AudioManagerRef::load_sound(std::string path, bool absolute) {return m_impl->load_sound(path,absolute);}

salmon::MusicRef AudioManagerRef::get_music(std::string path, bool absolute) {return m_impl->get_music(path,absolute);}
salmon::SoundRef AudioManagerRef::get_sound(std::string path, bool absolute) {return m_impl->get_sound(path,absolute);}

void AudioManagerRef::set_master_sound_volume(float factor) {m_impl->set_master_sound_volume(factor);}
float AudioManagerRef::get_master_sound_volume() {return m_impl->get_master_sound_volume();}

void AudioManagerRef::clear_music() {m_impl->clear_music();}
void AudioManagerRef::clear_sounds() {m_impl->clear_sounds();}
void AudioManagerRef::clear() {m_impl->clear();}
