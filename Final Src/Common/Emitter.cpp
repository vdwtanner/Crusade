#include "pch.h"
#include "Emitter.h"

Emitter::Emitter(Vector3 pos, Vector2 vel, Vector2 size, float interval, UINT type, UINT etype, UINT texIndex){
	m_pos = pos;
	m_vel = vel;
	m_size = size;
	m_emitInterval = interval;
	m_timer = interval;
	m_type = type;
	m_etype = etype;
	m_texIndex = texIndex;
}

Emitter::~Emitter(){

}

bool Emitter::operator<(Emitter const& e){
	return m_etype < e.getEmitType();
}

void Emitter::update(float dt){
	if (m_timer < 0)
		m_timer = m_emitInterval;
	m_timer -= dt;
}

Vector3 Emitter::getPosition() const{
	return m_pos;
}
Vector2 Emitter::getVelocity() const{
	return m_vel;
}
Vector2 Emitter::getSize() const{
	return m_size;
}
float Emitter::getInterval() const{
	return m_emitInterval;
}
float Emitter::getTimer() const{
	return m_timer;
}
UINT Emitter::getType() const{
	return m_type;
}
UINT Emitter::getEmitType() const{
	return m_etype;
}
UINT Emitter::getTextureIndex() const{
	return m_texIndex;
}

void Emitter::setPosition(Vector2 pos){
	m_pos = Vector3(pos.x, pos.y, m_pos.z);
}
void Emitter::setVelocity(Vector2 vel){
	m_vel = vel;
}
void Emitter::setSize(Vector2 size){
	m_size = size;
}
void Emitter::setInterval(float interval){
	float f = 0.001f;
	if (interval > 0.001f)
		f = interval;
	m_emitInterval = f;
}
void Emitter::setTimer(float timer){
	m_timer = timer;
}
void Emitter::setType(UINT type){
	m_type = type;
}
void Emitter::setEmitType(UINT etype){
	m_etype = etype;
}
void Emitter::setTextureIndex(UINT index){
	m_texIndex = index;
}