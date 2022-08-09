
#include "Ship.h"
#include "SpriteComponent.h"
#include "InputComponent.h"
#include "Game.h"
#include "Laser.h"

Ship::Ship(Game* game) : Actor(game), mLaserCooldown(0.0f), mCircle(nullptr)
{
	// Position
	SetPosition(Vector2(512.0f, 384.0f));

	// Create a sprite component
	SpriteComponent* sc = new SpriteComponent(this, 150);
	sc->SetTexture(game->GetTexture("Assets/Ship.png"));

	// Create an input component and set keys/speed
	InputComponent* ic = new InputComponent(this);
	ic->SetForwardKey(SDL_SCANCODE_UP);
	//ic->SetBackKey(SDL_SCANCODE_DOWN);
	ic->SetClockwiseKey(SDL_SCANCODE_LEFT);
	ic->SetCounterClockwiseKey(SDL_SCANCODE_RIGHT);
	ic->SetMaxForwardSpeed(150.0f);
	ic->SetMaxAngularSpeed((1.5 * Math::Pi));

	// Create a circle component for collisions
	mCircle = new CircleComponent(this);
	mCircle->SetRadius(15.0f);
	dead = false;

	float dt;
}

void Ship::UpdateActor(float dt)
{
	mLaserCooldown -= dt;

	// Do we intersect with an asteroid or smaller asteroid?
	for (auto ast : GetGame()->GetAsteroids())
	{
		if (Intersect(*mCircle, *(ast->GetCircle())))
		{
			// The first asteroid we intersect with,
			// set ourselves and the asteroid to dead
			SetState(EDead);
			ast->SetState(EDead);
			dead = true;
			break;
		}
	}

	for (auto ast : GetGame()->GetSmallerAsteroids())
	{
		if (Intersect(*mCircle, *(ast->GetCircle())))
		{
			// The first asteroid we intersect with,
			// set ourselves and the asteroid to dead
			SetState(EDead);
			ast->SetState(EDead);
			dead = true;
			break;
		}
	}
	
}

void Ship::ActorInput(const uint8_t* keyState)
{
	if (keyState[SDL_SCANCODE_SPACE] && mLaserCooldown <= 0.0f)
	{
		// Create a laser and set its position/rotation to mine
		Laser* laser = new Laser(GetGame());
		laser->SetPosition(GetPosition());
		laser->SetRotation(GetRotation());

		// Reset laser cooldown (quarter second)
		mLaserCooldown = 0.25f;
	}

	
}
