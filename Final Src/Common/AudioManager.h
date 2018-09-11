#pragma once

#include "pch.h"
#include <Audio.h>
#include "CSoundEffectInstance.h"

using namespace DirectX;

class GameScreen;

class AudioManager{

public:
	static const float MAX_AUDIBLE_DISTANCE;
	static AudioManager* Instance();
	static void Initialize(LPWSTR path);
	~AudioManager();

	void Suspend();
	void Resume();

	void Update(float dt);

	// Gives calling class ownership of soundinstance
	CSoundEffectInstance* CreateSoundInstance(std::string soundFile, SOUND_EFFECT_INSTANCE_FLAGS flags = SoundEffectInstance_Default);
	// MUST be called to delete a CSoundEffectInstance.
	void DeleteSoundInstance(CSoundEffectInstance* instance);

	// Audio Manager will own and garbage collect the soundinstance when it finishes playing.
	CSoundEffectInstance* PlaySound(std::string soundFile, SOUND_EFFECT_INSTANCE_FLAGS flags = SoundEffectInstance_Default, bool scaleByDistance = false, float initialVolume = 1.0f, Vector2 location = Vector2(0.0f, 0.0f));

	// returns true if sound exists, false otherwise
	bool PauseSound(CSoundEffectInstance *instance);
	// returns true if sound exists, false otherwise
	bool ResumeSound(CSoundEffectInstance *instance);
	// returns true if sound exists, false otherwise
	bool StopSound(CSoundEffectInstance *instance, bool immediate = true);

	float calcVolume(float initialVolume, Vector2 location);

	void OnNewAudioDevice();

#ifndef _CRUSADE_EDITOR_
	void setGameScreen(GameScreen* screen);
#endif

private:
	static AudioManager *m_instance;

	void loadSoundData(LPWSTR folder_path);

	AudioManager();

#ifndef _CRUSADE_EDITOR_
	GameScreen *h_gamescreen;
#endif

	std::unique_ptr<AudioEngine>	m_audEngine;
	std::map<std::string, SoundEffect *>	m_sounds;
	std::vector<CSoundEffectInstance *> m_ownedInstances;
	std::vector<CSoundEffectInstance *> m_unownedInstances;

	bool m_initialized;
	bool m_silentMode;
	bool m_retryAudio;

};