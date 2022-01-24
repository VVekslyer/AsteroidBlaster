
#include "SmallerAsteroid.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "CircleComponent.h"
#include "Game.h"
#include "Random.h"

SmallerAsteroid::SmallerAsteroid(Game* game, Vector2 pos)
	:Actor(game)
	, mCircle(nullptr)
{
	// These smaller asteroids will be spawned where their parent asteroid was.
	SetPosition(pos);
	SetRotation(Random::GetFloatRange(0.0f, Math::TwoPi));

	// Create a sprite component
	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(game->GetTexture("Assets/SmallerAsteroid.png"));

	// Create a move component, and set an asteroid speed
	MoveComponent* mc = new MoveComponent(this);
	mc->SetForwardSpeed(60.0f);

	// Create a circle component (for collision)
	mCircle = new CircleComponent(this);
	mCircle->SetRadius(10.0f);

	// Add to mSmallerAsteroids in game
	game->AddSmallerAsteroid(this);

	// Intialize sound effect
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 548);
	gBoom = Mix_LoadWAV("boom.wav");
};

SmallerAsteroid::~SmallerAsteroid()
{
	Mix_PlayChannel(-1, gBoom, 0);
	GetGame()->RemoveSmallerAsteroid(this);
}

void SmallerAsteroid::UpdateActor(float dt)
{

}