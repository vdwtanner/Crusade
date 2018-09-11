#pragma once
#include "pch.h"

class Emitter{

	friend class ParticleManager;

public:
	bool operator<(Emitter const& e);

	void update(float dt);

	Vector3 getPosition() const;
	Vector2 getVelocity() const;
	Vector2 getSize() const;
	float getInterval() const;
	float getTimer() const;
	UINT getType() const;
	UINT getEmitType() const;
	UINT getTextureIndex() const;

	void setPosition(Vector2);
	void setVelocity(Vector2);
	void setSize(Vector2);
	void setInterval(float);
	void setTimer(float);
	void setType(UINT);
	void setEmitType(UINT);
	void setTextureIndex(UINT);

private:
	Emitter(Vector3 pos, Vector2 vel, Vector2 size, float interval, UINT type, UINT etype, UINT texIndex);
	~Emitter();

	Vector3 m_pos;
	Vector2 m_vel;
	Vector2 m_size;
	float m_emitInterval;
	float m_timer;
	UINT m_type;
	UINT m_etype;
	UINT m_texIndex;

};