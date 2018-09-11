#pragma once
#include "pch.h"

class TriggerVolume : public GameObject{
public:
	TriggerVolume(std::string tag, std::string name, Vector2 position, Vector2 textureSize, Vector2 hitboxSize, Vector2 hitBoxOffset, float rotation, int id, int hp);
	~TriggerVolume();

	int getId() const;
	void setId(int);

	float getDuration() const;
	void setDuration(float);

	virtual TriggerVolume* clone() = 0;
	virtual void step(float dt) = 0;
	virtual void interpolate(float alpha);

	void decay(float dt);

	bool isDead() const;
	void die();

	b2Body* getSensorBody();
	virtual void initSensorBody(b2FixtureDef);

	int getCollisionCategory();
	void setEnableCollisionWith(int, int);

	bool operator<(TriggerVolume const& e);

	virtual void BeginContact(b2Contact* contact, GameObject* other);
	virtual void EndContact(b2Contact* contact, GameObject* other);

	virtual void onTrigger(GameObject* collider) = 0;
	virtual void onExitTrigger(GameObject* collider);
	virtual void onAirTrigger(WorldTile* air) = 0;

	void setCenterPosition(Vector2 pos);
	void setPosition(Vector2 pos);

protected:
	UINT m_id;
	float m_duration;	// -1 means that this volume is immortal

	b2Body* m_body;

	std::vector<GameObject*> m_insideVolume;
};