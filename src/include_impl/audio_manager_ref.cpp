#include "audio_manager_ref.hpp"

#include "audio/audio_manager.hpp"

using salmon::AudioManagerRef;

AudioManagerRef::AudioManagerRef(AudioManager& impl) : m_impl{&impl} {}

bool AudioManagerRef::load_music(std::string path) {return m_impl->load_music(path);}
bool AudioManagerRef::load_sound(std::string path) {return m_impl->load_sound(path);}

salmon::MusicRef AudioManagerRef::get_music(std::string path) {return m_impl->get_music(path);}
salmon::SoundRef AudioManagerRef::get_sound(std::string path) {return m_impl->get_sound(path);}

void AudioManagerRef::set_music_path(std::string path) {m_impl->set_music_path(path);}
void AudioManagerRef::set_sound_path(std::string path) {m_impl->set_sound_path(path);}

void AudioManagerRef::set_master_sound_volume(float factor) {m_impl->set_master_sound_volume(factor);}
float AudioManagerRef::get_master_sound_volume() {return m_impl->get_master_sound_volume();}

void AudioManagerRef::clear_music() {m_impl->clear_music();}
void AudioManagerRef::clear_sounds() {m_impl->clear_sounds();}
void AudioManagerRef::clear() {m_impl->clear();}
