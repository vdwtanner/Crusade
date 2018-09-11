#pragma once

#include "pch.h"
#include "PrimitiveBatch.h"
#include "VertexTypes.h"
#include "Effects.h"
#include "SimpleMath.h"
#include "Box2DSingleton.h"
#include "GameObject.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class WorldTileMatrix;

class WorldTile : public GameObject{

public:
	enum Sides{
		NONE = 0x0000,
		UP = 0x0001,
		RIGHT = 0x0002,
		DOWN = 0x0004,
		LEFT = 0x0008,
		ALL = 0xFFFF
	};
	// z member in vector3 pos is the layer
	//WorldTile(Vector3 pos, UINT id, int health, bool isActiveLayer);
	WorldTile();
	~WorldTile();

	//void Draw(PrimitiveBatch<VertexPositionColorTexture> *batch);
	void Update(float dt);
	UINT getId();
	void setId(UINT id);

	void setLayer(int layer);
	int getLayer();

	b2Body* getRigidbody();
	void destroyTile();
	void initRigidbody(b2BodyDef def);
	void step(float dt);
	GameObject* clone();

	virtual void damage(int);
	virtual void heal(int);
	void damageThisOnly(int);

	void setSideEnableCollisionWith(int side, int collideWith);

	void BeginContact(b2Contact* contact, GameObject* other);
	void EndContact(b2Contact* contact, GameObject* other);
private:
	int m_layer;
	UINT m_id;
	b2Body* m_rigidbody;
};