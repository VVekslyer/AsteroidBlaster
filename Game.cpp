
#include "Game.h"
#include "SDL/SDL_image.h"
#include <algorithm>
#include "Actor.h"
#include "SpriteComponent.h"
#include "Ship.h"
#include "Asteroid.h"
#include "Random.h"
#include "SmallerAsteroid.h"

Game::Game()
:mWindow(nullptr)
,mRenderer(nullptr)
,mIsRunning(true)
,mUpdatingActors(false)
{
	round = 1;
}

bool Game::Initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	
	mWindow = SDL_CreateWindow("Vitaliy Vekslyer - CMPT 1267 - Final Project - ASTEROID BLASTER", 100, 100, 1024, 768, 0);
	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	
	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}
	
	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
		return false;
	}

	// Intialize font
	TTF_Init();
	mFont = TTF_OpenFont("basis33.ttf", 42);
	mFontGM = TTF_OpenFont("basis33.ttf", 72);

	// Load saved score
	scoreFile.open("saved-score.txt", std::ios::in);
	if (scoreFile.is_open()) {
		std::string x;
		getline(scoreFile, x);
		bestScore = stoi(x);
	}

	Random::Init();

	LoadData();

	mTicksCount = SDL_GetTicks();
	
	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	
	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	if (keyState[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->ProcessInput(keyState);
	}
	mUpdatingActors = false;
}

void Game::UpdateGame()
{
	// Compute delta time
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	float dt = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (dt > 0.05f)
	{
		dt = 0.05f;
	}
	mTicksCount = SDL_GetTicks();

	// Update all actors
	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->Update(dt);
	}
	mUpdatingActors = false;

	// Move any pending actors to mActors
	for (auto pending : mPendingActors)
	{
		mActors.emplace_back(pending);
	}
	mPendingActors.clear();

	// Add any dead actors to a temp vector
	std::vector<Actor*> deadActors;
	for (auto actor : mActors)
	{
		if (actor->GetState() == Actor::EDead)
		{
			deadActors.emplace_back(actor);
		}
	}

	// Delete dead actors (which removes them from mActors)
	for (auto actor : deadActors)
	{
		delete actor;
	}
	

	// Generate new asteroids if no asteroids remain.
	if (mAsteroids.empty())
	{
		round++;
		numAsteroids += 10;
		for (int i = 0; i < numAsteroids; i++)
		{
			new Asteroid(this);
		}
	}
}

void Game::GenerateOutput()
{
	// Background
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
	SDL_RenderClear(mRenderer);
	
	// Draw all sprite components
	for (auto sprite : mSprites)
	{
		sprite->Draw(mRenderer);
	}

	// Text
	SDL_Color textColor = { 255, 255, 255 };
	std::string bestScoreText = "Best Score: " + std::to_string(bestScore);
	std::string scoreText = "Score: " + std::to_string(playerScore);
	std::string roundText = "Round " + std::to_string(round);


	// Hi-Score
	SDL_Surface* bestScoreSurface = TTF_RenderText_Solid(mFont, bestScoreText.c_str(), textColor);
	textTexture = SDL_CreateTextureFromSurface(mRenderer, bestScoreSurface);
	mWidth = bestScoreSurface->w;
	mHeight = bestScoreSurface->h;
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 0xFF);
	SDL_Rect renderQuad = { 700, 100, mWidth, mHeight };
	SDL_RenderCopyEx(mRenderer, textTexture, NULL, &renderQuad, 0, NULL, SDL_FLIP_NONE);

	// Score
	SDL_Surface* scoreSurface = TTF_RenderText_Solid(mFont, scoreText.c_str(), textColor);
	textTexture = SDL_CreateTextureFromSurface(mRenderer, scoreSurface);
	mWidth = scoreSurface->w;
	mHeight = scoreSurface->h;
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 0xFF);
	renderQuad = { 700, 130, mWidth, mHeight };
	SDL_RenderCopyEx(mRenderer, textTexture, NULL, &renderQuad, 0, NULL, SDL_FLIP_NONE);

	// Round
	SDL_Surface* roundSurface = TTF_RenderText_Solid(mFont, roundText.c_str(), textColor);
	textTexture = SDL_CreateTextureFromSurface(mRenderer, roundSurface);
	mWidth = roundSurface->w;
	mHeight = roundSurface->h;
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 0xFF);
	renderQuad = { 50, 130, mWidth, mHeight };
	SDL_RenderCopyEx(mRenderer, textTexture, NULL, &renderQuad, 0, NULL, SDL_FLIP_NONE);


	// If the player dies, generate a game over screen.
	if (mShip->dead)
	{
		SDL_Color textColor = { 255, 255, 255 };
		std::string gameOverText = "Game Over";

		// Game Over
		SDL_Surface* gameOverSurface = TTF_RenderText_Solid(mFontGM, gameOverText.c_str(), textColor);
		textTexture = SDL_CreateTextureFromSurface(mRenderer, gameOverSurface);
		mWidth = gameOverSurface->w;
		mHeight = gameOverSurface->h;
		SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 0xFF);
		SDL_Rect renderQuad = { 380, 360, mWidth, mHeight };
		SDL_RenderCopyEx(mRenderer, textTexture, NULL, &renderQuad, 0, NULL, SDL_FLIP_NONE);
	}

	// If all asteroids are destroyed, generate new ones.
	if (mAsteroids.empty() && mSmallerAsteroids.empty())
	{
		numAsteroids += 10;
		for (int i = 0; i < numAsteroids; i++)
			new Asteroid(this);
		
	}


	SDL_RenderPresent(mRenderer);
}

void Game::LoadData()
{
	// Create player's ship
	mShip = new Ship(this);
	mShip->SetPosition(Vector2(512.0f, 384.0f));
	mShip->SetRotation(Math::PiOver2);

	// Create asteroids
	numAsteroids = 5;
	for (int i = 0; i < numAsteroids; i++)
	{
		new Asteroid(this);
	}
}

void Game::UnloadData()
{
	// Delete actors
	// Because ~Actor calls RemoveActor, have to use a different style loop
	while (!mActors.empty())
	{
		delete mActors.back();
	}

	// Destroy textures
	for (auto i : mTextures)
	{
		SDL_DestroyTexture(i.second);
	}
	mTextures.clear();
}

SDL_Texture* Game::GetTexture(const std::string& fileName)
{
	SDL_Texture* tex = nullptr;
	// Is the texture already in the map?
	auto iter = mTextures.find(fileName);
	if (iter != mTextures.end())
	{
		tex = iter->second;
	}
	else
	{
		// Load from file
		SDL_Surface* surf = IMG_Load(fileName.c_str());
		if (!surf)
		{
			SDL_Log("Failed to load texture file %s", fileName.c_str());
			return nullptr;
		}

		// Create texture from surface
		tex = SDL_CreateTextureFromSurface(mRenderer, surf);
		SDL_FreeSurface(surf);
		if (!tex)
		{
			SDL_Log("Failed to convert surface to texture for %s", fileName.c_str());
			return nullptr;
		}

		mTextures.emplace(fileName.c_str(), tex);
	}
	return tex;
}

void Game::AddAsteroid(Asteroid* ast)
{
	mAsteroids.emplace_back(ast);
}

void Game::AddSmallerAsteroid(SmallerAsteroid* ast)
{
	mSmallerAsteroids.emplace_back(ast);
}

void Game::RemoveAsteroid(Asteroid* ast)
{
	auto iter = std::find(mAsteroids.begin(),
		mAsteroids.end(), ast);
	if (iter != mAsteroids.end())
	{
		mAsteroids.erase(iter);
		playerScore++;
	}
}

void Game::RemoveSmallerAsteroid(SmallerAsteroid* ast)
{
	auto iter = std::find(mSmallerAsteroids.begin(),
		mSmallerAsteroids.end(), ast);
	if (iter != mSmallerAsteroids.end())
	{
		mSmallerAsteroids.erase(iter);
		playerScore++;
	}
}

void Game::SaveData() 
{
	// Save the player score if it's greater than the hi-score.
	scoreFile.close();
	scoreFile.open("saved-score.txt", std::ios::out);
	if (scoreFile.is_open())
		if (playerScore >= bestScore)
			scoreFile << std::to_string(playerScore);
		else
			scoreFile << std::to_string(bestScore);

	scoreFile.close();
}

void Game::Shutdown()
{
	SaveData();
	UnloadData();
	IMG_Quit();
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

void Game::AddActor(Actor* actor)
{
	// If we're updating actors, need to add to pending
	if (mUpdatingActors)
	{
		mPendingActors.emplace_back(actor);
	}
	else
	{
		mActors.emplace_back(actor);
	}
}

void Game::RemoveActor(Actor* actor)
{
	// Is it in pending actors?
	auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
	if (iter != mPendingActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mPendingActors.end() - 1);
		mPendingActors.pop_back();
	}

	// Is it in actors?
	iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mActors.end() - 1);
		mActors.pop_back();
	}
}

void Game::AddSprite(SpriteComponent* sprite)
{
	// Find the insertion point in the sorted vector
	// (The first element with a higher draw order than me)
	int myDrawOrder = sprite->GetDrawOrder();
	auto iter = mSprites.begin();
	for ( ;
		iter != mSprites.end();
		++iter)
	{
		if (myDrawOrder < (*iter)->GetDrawOrder())
		{
			break;
		}
	}

	// Inserts element before position of iterator
	mSprites.insert(iter, sprite);
}

void Game::RemoveSprite(SpriteComponent* sprite)
{
	// (We can't swap because it ruins ordering)
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	mSprites.erase(iter);
}
