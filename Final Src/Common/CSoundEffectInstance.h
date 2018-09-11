#pragma once

#include "pch.h"
#include <Audio.h>

class CSoundEffectInstance{

	friend class AudioManager;

public:
	void Play(bool loop = false);
	void Stop(bool immediate = true);
	void Pause();
	void Resume();
	void SetVolume(float volume);
	void SetPitch(float pitch);
	void SetPan(float pan);
	bool IsLooped() const;
	SoundState GetState();



private:
	CSoundEffectInstance(SoundEffectInstance*);
	~CSoundEffectInstance();
	SoundEffectInstance *m_instance;

};