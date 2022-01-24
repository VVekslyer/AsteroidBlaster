
#include "Asteroid.h"
#include "SmallerAsteroid.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "Game.h"
#include "Random.h"
#include "CircleComponent.h"

Asteroid::Asteroid(Game* game)
	:Actor(game)
	, mCircle(nullptr)
{
	// Initialize to random position/orientation
	Vector2 randPos = Random::GetVector(Vector2::Zero,
		Vector2(1024.0f, 768.0f));
	SetPosition(randPos);
	SetRotation(Random::GetFloatRange(0.0f, Math::TwoPi));

	// Create a sprite component
	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(game->GetTexture("Assets/Asteroid.png"));

	// Create a move component, and set an asteroid speed
	MoveComponent* mc = new MoveComponent(this);
	mc->SetForwardSpeed(75.0f);

	// Create a circle component (for collision)
	mCircle = new CircleComponent(this);
	mCircle->SetRadius(20.0f);

	// Add to mAsteroids in game
	game->AddAsteroid(this);

	// Intialize sound effect
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 548);
	gBoom = Mix_LoadWAV("boom.wav");
}

Asteroid::~Asteroid()
{
	Mix_PlayChannel(-1, gBoom, 0);
	GetGame()->RemoveAsteroid(this);
	
	// When an asteroid is destroyed, it will spawn two smaller asteroids.
	new SmallerAsteroid(GetGame(), this->GetPosition());
	new SmallerAsteroid(GetGame(), this->GetPosition());
}

void Asteroid::UpdateActor(float dt)
{
	// Create an asteroids vector that does not contain THIS current asteroid.
	std::vector<Asteroid*> asteroids = GetGame()->GetAsteroids();
	std::vector<Asteroid*>::iterator i = std::find(asteroids.begin(), asteroids.end(), this);
	int index;

	if (std::find(asteroids.begin(), asteroids.end(), this) != asteroids.end()) 
	{
		index = std::distance(asteroids.begin(), i);
		asteroids.erase(asteroids.begin()+index);
	}

	// Do we intersect with an asteroid?
	for (auto ast : asteroids)
	{
		if (Intersect(*mCircle, *(ast->GetCircle())))
		{
			// The first asteroid we intersect with,
			// set ourselves and the asteroid to dead
			SetState(EDead);
			ast->SetState(EDead);
			break;
		}
	}
}

