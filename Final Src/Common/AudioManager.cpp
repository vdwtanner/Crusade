#include "pch.h"
#include "AudioManager.h"
#ifndef _CRUSADE_EDITOR_
#include "GameScreen.h"
#endif


const float AudioManager::MAX_AUDIBLE_DISTANCE = 30.0f;
AudioManager* AudioManager::m_instance = nullptr;

AudioManager* AudioManager::Instance(){
	return m_instance;
}

// Call this before getting an instance of this.
void AudioManager::Initialize(LPWSTR path){
	m_instance = new AudioManager();

	// Create the audio engine
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
	eflags = eflags | AudioEngine_Debug;
#endif
	m_instance->m_audEngine.reset(new AudioEngine(eflags));

	m_instance->m_retryAudio = false;

	if (!m_instance->m_audEngine->IsAudioDevicePresent()){
		m_instance->m_silentMode = true;
	}
	else
	{
		m_instance->m_silentMode = false;
	}

	// populate our sound effect vector
	m_instance->loadSoundData(path);

	m_instance->m_initialized = true;
}

AudioManager::~AudioManager(){
	if (m_audEngine.get())
		m_audEngine->Suspend();
	for (UINT i = 0; i < m_ownedInstances.size(); i++){
		delete m_ownedInstances[i];
	}
	for (UINT i = 0; i < m_unownedInstances.size(); i++){
		delete m_unownedInstances[i];
	}
	for (auto it = m_sounds.begin(); it != m_sounds.end(); it++){
		delete it->second;
	}
}

AudioManager::AudioManager(){

}

void AudioManager::loadSoundData(LPWSTR folder_path){
	using namespace std;
	using namespace std::tr2::sys;
	path scriptDir = CW2A(folder_path);

	USES_CONVERSION;

	for (directory_iterator i(scriptDir), end; i != end; i++){
		path extension = i->path().extension();
		if (i->path().extension().compare(".adpcm") == 0 || i->path().extension().compare(".wav") == 0)
		{
			wchar_t *wfilePath = A2W(i->path().file_string().c_str());
			m_sounds[i->path().filename().c_str()] = new SoundEffect(m_audEngine.get(), wfilePath);

		}
		else{
			CrusadeUtil::debug("\nNot a adpcm or wav file: ");

			CrusadeUtil::debug(i->path());
		}
	}
}

void AudioManager::Suspend(){
	m_audEngine->Suspend();
}

void AudioManager::Resume(){
	m_audEngine->Resume();
}

void AudioManager::Update(float dt){
	if (m_retryAudio)
	{
		m_retryAudio = false;
		if (m_audEngine->Reset())
		{
			//Restart looped sounds
			for (auto it = m_ownedInstances.begin(); it != m_ownedInstances.end(); it++){
				if ((*it)->IsLooped())
					(*it)->Play(true);
			}
			for (auto it = m_unownedInstances.begin(); it != m_unownedInstances.end(); it++){
				if ((*it)->IsLooped())
					(*it)->Play(true);
			}
		}
	}
	else if (!m_audEngine->Update())
	{
		if (m_audEngine->IsCriticalError())
		{
			m_retryAudio = true;
		}
		else
		{
			for (auto it = m_ownedInstances.begin(); it != m_ownedInstances.end();){
				if ((*it)->GetState() == STOPPED){
					delete (*it);
					it = m_ownedInstances.erase(it);
				}
				else
				{
					it++;
				}
			}
		}
	}
}

// if you create a sound with this function, you must delete it with DeleteSoundInstance
CSoundEffectInstance* AudioManager::CreateSoundInstance(std::string soundFile, SOUND_EFFECT_INSTANCE_FLAGS flags){
	CSoundEffectInstance *sound = new CSoundEffectInstance(m_sounds[soundFile]->CreateInstance(flags).release());
	m_unownedInstances.push_back(sound);
	return sound;
	//sound->Play(looped);
}

void AudioManager::DeleteSoundInstance(CSoundEffectInstance* instance){
	for (auto it = m_ownedInstances.begin(); it != m_ownedInstances.end(); it++){
		if ((*it) == instance){
			delete (*it);
			m_ownedInstances.erase(it);
			return;
		}
	}
	for (auto it = m_unownedInstances.begin(); it != m_unownedInstances.end(); it++){
		if ((*it) == instance){
			delete (*it);
			m_unownedInstances.erase(it);
			return;
		}
	}
}

CSoundEffectInstance* AudioManager::PlaySound(std::string soundFile, SOUND_EFFECT_INSTANCE_FLAGS flags, bool scaleByDistance, float initialVolume, Vector2 location){
	CSoundEffectInstance *sound = new CSoundEffectInstance(m_sounds[soundFile]->CreateInstance(flags).release());
	m_ownedInstances.push_back(sound);
	sound->Play();
	if (scaleByDistance){
		sound->SetVolume(calcVolume(initialVolume, location));
	}
	return sound;
}

float AudioManager::calcVolume(float initialVolume, Vector2 location){
#ifndef _CRUSADE_EDITOR_
	Vector2 camPos = h_gamescreen->getCamera()->getPosition();
	float disX = fabs(location.x - camPos.x);
	float disY = fabs(location.y - camPos.y);

	float hyp = sqrtf(disX*disX + disY*disY);

	float vol = initialVolume;
	if (hyp > 10.0f){
		float d = hyp - 10.0f;
		float percent = cosf(d / MAX_AUDIBLE_DISTANCE);
		vol = initialVolume * percent;
	}

	return vol;
#else
	return 1;
#endif
}


bool AudioManager::PauseSound(CSoundEffectInstance *instance){
	bool exists = false;
	for (auto it = m_ownedInstances.begin(); it != m_ownedInstances.end(); it++){
		if ((*it) == instance){
			exists = true;
			break;
		}
	}
	if (!exists){
		for (auto it = m_unownedInstances.begin(); it != m_unownedInstances.end(); it++){
			if ((*it) == instance){
				exists = true;
				break;
			}
		}
	}
	if (exists)
		instance->Pause();
	return exists;
}

bool AudioManager::ResumeSound(CSoundEffectInstance *instance){
	bool exists = false;
	for (auto it = m_ownedInstances.begin(); it != m_ownedInstances.end(); it++){
		if ((*it) == instance){
			exists = true;
			break;
		}
	}
	if (!exists){
		for (auto it = m_unownedInstances.begin(); it != m_unownedInstances.end(); it++){
			if ((*it) == instance){
				exists = true;
				break;
			}
		}
	}
	if (exists)
		instance->Resume();
	return exists;
}

bool AudioManager::StopSound(CSoundEffectInstance *instance, bool immediate){
	bool exists = false;
	for (auto it = m_ownedInstances.begin(); it != m_ownedInstances.end(); it++){
		if ((*it) == instance){
			exists = true;
			break;
		}
	}
	if (!exists){
		for (auto it = m_unownedInstances.begin(); it != m_unownedInstances.end(); it++){
			if ((*it) == instance){
				exists = true;
				break;
			}
		}
	}
	if (exists)
		instance->Stop(immediate);
	return exists;
}

void AudioManager::OnNewAudioDevice(){
	m_retryAudio = true;
}

#ifndef _CRUSADE_EDITOR_
void AudioManager::setGameScreen(GameScreen *screen){
	h_gamescreen = screen;
}
#endif