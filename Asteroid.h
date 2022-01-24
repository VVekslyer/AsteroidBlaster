
#pragma once
#include "Actor.h"
#include "SDL/SDL_mixer.h"
class Asteroid : public Actor
{
public:
	Asteroid(class Game* game);
	~Asteroid();
	Mix_Chunk* gBoom;
	
	class CircleComponent* GetCircle() { return mCircle; }
	void UpdateActor(float dt) override;
private:
	class CircleComponent* mCircle;
};
