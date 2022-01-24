
#pragma once
#include "Actor.h"
#include "CircleComponent.h"
#include "SmallerAsteroid.h"
#include "Asteroid.h"
class Ship : public Actor
{
public:
	Ship(class Game* game);

	void UpdateActor(float dt) override;
	void ActorInput(const uint8_t* keyState) override;
	bool dead;
	
private:
	class CircleComponent* mCircle;
	float mLaserCooldown;

};