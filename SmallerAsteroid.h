
#pragma once
#include "Actor.h"
#include "SDL/SDL_mixer.h"

class SmallerAsteroid : public Actor
{
	public:
		SmallerAsteroid(class Game* game, Vector2 pos);
		~SmallerAsteroid();
		Mix_Chunk* gBoom;
		class CircleComponent* GetCircle() { return mCircle; }
		void UpdateActor(float dt) override;
	private:
		class CircleComponent* mCircle;
};