
#pragma once
#include "Actor.h"
class Laser : public Actor
{
public:
	Laser(class Game* game);

	void UpdateActor(float dt) override;
private:
	class CircleComponent* mCircle;
	float mDeathTimer;
};
