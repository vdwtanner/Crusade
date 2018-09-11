#include "pch.h"
#include "CSoundEffectInstance.h"

CSoundEffectInstance::CSoundEffectInstance(SoundEffectInstance *instance){
	m_instance = instance;
}

CSoundEffectInstance::~CSoundEffectInstance(){
	delete m_instance;
}

void CSoundEffectInstance::Play(bool loop){
	m_instance->Play(loop);
}

void CSoundEffectInstance::Stop(bool immediate){
	m_instance->Stop(immediate);
}

void CSoundEffectInstance::Pause(){
	m_instance->Pause();
}

void CSoundEffectInstance::Resume(){
	m_instance->Resume();
}

void CSoundEffectInstance::SetVolume(float volume){
	m_instance->SetVolume(volume);
}

void CSoundEffectInstance::SetPitch(float pitch){
	m_instance->SetPitch(pitch);
}

void CSoundEffectInstance::SetPan(float pan){
	m_instance->SetPan(pan);
}

bool CSoundEffectInstance::IsLooped() const{
	return m_instance->IsLooped();
}

SoundState CSoundEffectInstance::GetState(){
	return m_instance->GetState();
}