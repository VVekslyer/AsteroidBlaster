
#pragma once
#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void Shutdown();

	void AddActor(class Actor* actor);
	void RemoveActor(class Actor* actor);

	void AddSprite(class SpriteComponent* sprite);
	void RemoveSprite(class SpriteComponent* sprite);
	
	SDL_Texture* GetTexture(const std::string& fileName);

	// Game-specific (add/remove asteroid)
	void AddAsteroid(class Asteroid* ast);
	void AddSmallerAsteroid(class SmallerAsteroid* ast);
	void RemoveAsteroid(class Asteroid* ast);
	void RemoveSmallerAsteroid(SmallerAsteroid* ast);
	std::vector<class Asteroid*>& GetAsteroids() { return mAsteroids; }
	std::vector<class SmallerAsteroid*>& GetSmallerAsteroids() { return mSmallerAsteroids;  }
private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void SaveData();
	void LoadData();
	void UnloadData();
	
	// Map of textures loaded
	std::unordered_map<std::string, SDL_Texture*> mTextures;

	// All the actors in the game
	std::vector<class Actor*> mActors;
	// Any pending actors
	std::vector<class Actor*> mPendingActors;

	// All the sprite components drawn
	std::vector<class SpriteComponent*> mSprites;

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	SDL_Texture* textTexture;
	Uint32 mTicksCount;
	bool mIsRunning;
	// Track if we're updating actors right now
	bool mUpdatingActors;

	// Game-specific
	class Ship* mShip; // Player's ship
	std::vector<class Asteroid*> mAsteroids;
	std::vector<class SmallerAsteroid*> mSmallerAsteroids;
	int numAsteroids;
	int playerScore;
	int bestScore;
	int mHeight;
	int mWidth;
	int round;

	// Text
	TTF_Font* mFont;
	TTF_Font* mFontGM;

	// Saved score
	std::fstream scoreFile;
};
