#pragma once

#include "Enemy.h"

class MeleeEnemy : public Enemy {
public:
	MeleeEnemy(std::string tag, std::string name, Vector2 position, Vector2 textureSize, Vector2 hitBoxSize, Vector2 hitBoxOffset, float rotation, int id, int hp, float speed, float sight, float hearing, float massDensity, float attackMultiplier, float strength, float jumpStrength, int courage, int exp, std::string type, UINT aiFlags, std::string abilities, SoundDescription sounds);
	MeleeEnemy* clone();
	void step(float dt) override;
protected:
	float walkTimer = 3;
	float dtWalkTimer = 0;
	void stepAI(float dt) override;
private:

};