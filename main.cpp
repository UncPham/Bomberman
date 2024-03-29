//Bomberman Game 
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <sstream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <iostream>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480 + 32;

//The dimensions of the level
const int LEVEL_WIDTH = 1248;
const int LEVEL_HEIGHT = 480;

//Tile constants
const int TILE_WIDTH = 32;
const int TILE_HEIGHT = 32;
const int TOTAL_TILES = 585;
const int TOTAL_TILE_SPRITES = 19;

const int BUTTON_WIDTH = 200;
const int BUTTON_HEIGHT = 75;
const int TOTAL_BUTTONS = 5;

enum LButtonSprite
{
	BUTTON_SPRITE_MOUSE_OUT = 0,
	BUTTON_SPRITE_MOUSE_OVER_MOTION = 1,
	BUTTON_SPRITE_TOTAL = 2
};

//The different tile sprites
const int Land = 0;
const int Spon = 1;
const int BombIteam = 3;
const int LengthIteam = 4;
const int SpeedIteam = 5;
const int Door = 6;
const int Wall = 7;
const int Box = 8;
const int BoxHasBombIteam = 9;
const int BoxHasLengthIteam = 10;
const int BoxHasSpeedIteam = 11;
const int BoxHasDoor = 12;
const int BoxExploding = 13;
const int BoxHasBombIteamExploding = 14;
const int BoxHasLengthIteamExploding = 15;
const int BoxHasSpeedIteamExploding = 16;
const int BoxHasDoorExploding = 18;
const int HardWall = 17;

const int Menu = 1;
const int ChooseMode = 7;
const int Pause = 2;
const int MultiPlayer = 3;
const int NormalGame = 4;
const int Playing = 5;
const int EndGame = 6;
const int WinGame = 8;
const int Player1Win = 9;
const int Player2Win = 10;

const int LevelNormal = 5;
const int LevelMultiPlayer = 3;

const int MaxBomb = 5;
const int MaxBot = 40;


int count = 0;
int currentLevel = 0;
int score = 0;


int game = Menu;
int mode;

bool UpLevel = false;

//Texture wrapper class
class LTexture
{
public:
	//Initializes variables
	LTexture();

	//Deallocates memory
	~LTexture();

	//Loads image at specified path
	bool loadFromFile(std::string path);

#if defined(SDL_TTF_MAJOR_VERSION)
	//Creates image from font string
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
#endif

	//Deallocates texture
	void free();

	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	//Set blending
	void setBlendMode(SDL_BlendMode blending);

	//Set alpha modulation
	void setAlpha(Uint8 alpha);

	//Renders texture at given point
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	//Gets image dimensions
	int getWidth();
	int getHeight();

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};

//The tile
class Tile
{
public:
	//Initializes position and type
	Tile(int x, int y, int tileType);

	//Shows the tile
	void render(SDL_Rect& camera);

	//Get the tile type
	int getType();

	void setType(int mType);

	//Get the collision box
	SDL_Rect getBox();

private:
	//The attributes of the tile
	SDL_Rect mBox;

	//The tile type
	int mType;
};

class LButton
{
public:
	//Initializes internal variables
	LButton();

	//Sets top left position
	void setPosition(int x, int y);

	//Handles mouse event
	bool handleEvent(SDL_Event* e);

	//Shows button sprite
	void render(int i);

private:
	//Top left position
	SDL_Point mPosition;

	//Currently used global sprite
	LButtonSprite mCurrentSprite;
};

class LTimer
{
public:
	//Initializes variables
	LTimer();

	//The various clock actions
	void start();
	void stop();
	void pause();
	void unpause();

	//Gets the timer's time
	Uint32 getTicks();

	//Checks the status of the timer
	bool isStarted();
	bool isPaused();

private:
	//The clock time when the timer started
	Uint32 mStartTicks;

	//The ticks stored when the timer was paused
	Uint32 mPausedTicks;

	//The timer status
	bool mPaused;
	bool mStarted;
};

class Bomb
{
public:
	//The dimensions of the dot
	static const int Bomb_WIDTH = 32;
	static const int Bomb_HEIGHT = 32;

	//Initializes the variables
	Bomb();

	bool handleEvent1(SDL_Event& e, SDL_Rect dot);

	bool handleEvent2(SDL_Event& e, SDL_Rect dot);

	//Shows the dot on the screen
	void render(SDL_Rect& camera, Tile* tiles[]);

	bool boom();

	void explode(Tile* tiles[], SDL_Rect& camera);

	SDL_Rect getRect();

	bool getThrough();
	void setThrough(bool _through);
	bool getIsExploding();

	void incLength();
	int getLength();
	void resetLength();

	void setLength(int _length);

	int getX();
	int getY();
	bool getUsed();

	void setRect();
private:
	//Collision box of the dot
	SDL_Rect mBomb;

	Uint32 wait;

	bool through;

	bool beUsed;

	int length;

	bool isExploding;

	bool check;

	int step;
};


class Bot
{
public:
	//The dimensions of the dot
	static const int DOT_WIDTH = 32;
	static const int DOT_HEIGHT = 32;

	//Maximum axis velocity of the dot

	//Initializes the variables
	Bot(int x, int y, bool k);

	//Moves the dot and check collision against tiles
	void move(Tile* tiles[], Bomb* bomb[]);

	void findDirection(Tile* tiles[], SDL_Rect dot);

	void death(Bomb* bomb[], Tile* tile[]);

	//Shows the dot on the screen
	void render(SDL_Rect& camera);

	void changeDirection();

	int getRect_x();
	int getRect_y();

	bool getDead();


	SDL_Rect getRect();

private:
	//Collision box of the dot
	SDL_Rect mBox;

	//The velocity of the dot
	int mVelX, mVelY;

	Uint32 time;

	bool change;

	bool dead;

	bool intelligent;
};

//The dot that will move around on the screen
class Dot
{
public:

	//The dimensions of the dot
	static const int DOT_WIDTH = 32;
	static const int DOT_HEIGHT = 32;

	//Maximum axis velocity of the dot
	int DOT_VEL = 2;

	//Initializes the variables
	Dot(int x, int y);

	//Takes key presses and adjusts the dot's velocity
	void handleEvent1(SDL_Event& e);
	void handleEvent2(SDL_Event& e);

	//Moves the dot and check collision against tiles
	void move(Tile* tiles[], Bomb* bomb[], Bot* bot[], Dot* otherDot = NULL, Bomb* bomb2[] = NULL);

	//Centers the camera over the dot
	void setCamera(SDL_Rect& camera);

	void death(Bomb* bomb[], Bot* bot[], Tile* tile[]);

	void getIteam(Tile* tiles[], Bomb* bomb[], Bot* bot[]);

	void recovery();

	//Shows the dot on the screen
	void render(SDL_Rect& camera, int t);

	int getRect_x();
	int getRect_y();

	int getTotalBomb();

	bool getDead();
	void deadRender(SDL_Rect camera);

	int getLife();
	void setLife(int _life);

	void resetVel();
	void stop();

	SDL_Rect getRect();
	void setRect(int x, int y);

	void resetTotalBomb();

private:
	//Collision box of the dot
	SDL_Rect mBox;

	//The velocity of the dot
	int mVelX, mVelY;

	int totalBomb;

	int step;
	int fade;
	bool right;
	bool left;

	int life;
	bool immortal;

	int alpha;

	Uint32 time;
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close(Tile* tiles[], Bomb* bomb1[], Bomb* bomb2[], Bot* bot[]);

//Box collision detector
bool checkCollision(SDL_Rect a, SDL_Rect b);

//Checks collision box against set of tiles
bool touchesWall(SDL_Rect box, Tile* tiles[]);

SDL_Rect touchesEgdeOfWall(SDL_Rect box, Tile* tiles[], int velX, int velY);

bool touchesBomb(SDL_Rect box, Bomb* bomb[]);

bool touchBomb(SDL_Rect box, Bomb* bomb[]);

bool checkCoordinate(SDL_Rect a, Bomb& bomb);

//Sets tiles from tile map
bool setTiles(Tile* tiles[], Bot* bot[], int gameLevel);

void NextLevel(Tile* tiles[], Dot& dot, Bot* bot[], Bomb* bomb[]);

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene textures
LTexture gDotTexture;
LTexture gDotTexture2;
LTexture gTileTexture;
SDL_Rect gTileClips[TOTAL_TILE_SPRITES];
LTexture BombTexture;
LTexture BoomTexture;
LTexture BotTexture1;
LTexture BotTexture2;
LTexture EndGameTexture;
LTexture MenuTexture;
LTexture PauseTexture;

LTexture NextLevelText;
LTexture gTimeTextTexture;
LTexture LifeTextTexture;

//Globally used font
TTF_Font* gFont = NULL;

Mix_Music* Music = NULL;

//Mouse button sprites
SDL_Rect gSpriteClips[BUTTON_SPRITE_TOTAL];
LTexture gButtonSpriteSheetTexture;

//Buttons objects
LButton gButtons[TOTAL_BUTTONS];

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile(std::string path)
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	if (textSurface != NULL)
	{
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}
	else
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}


	//Return success
	return mTexture != NULL;
}
#endif

void LTexture::free()
{
	//Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	//Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}


LButton::LButton()
{
	mPosition.x = 0;
	mPosition.y = 0;

	mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
}

void LButton::setPosition(int x, int y)
{
	mPosition.x = x;
	mPosition.y = y;
}

bool LButton::handleEvent(SDL_Event* e)
{
	//If mouse event happened
	if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN)
	{
		//Get mouse position
		int x, y;
		SDL_GetMouseState(&x, &y);

		//Check if mouse is in button
		bool inside = true;

		//Mouse is left of the button
		if (x < mPosition.x)
		{
			inside = false;
		}
		//Mouse is right of the button
		else if (x > mPosition.x + BUTTON_WIDTH)
		{
			inside = false;
		}
		//Mouse above the button
		else if (y < mPosition.y)
		{
			inside = false;
		}
		//Mouse below the button
		else if (y > mPosition.y + BUTTON_HEIGHT)
		{
			inside = false;
		}

		//Mouse is outside button
		if (!inside)
		{
			mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
		}
		//Mouse is inside button
		else
		{
			//Set mouse over sprite
			switch (e->type)
			{
			case SDL_MOUSEMOTION:
				mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
				break;
			case SDL_MOUSEBUTTONDOWN:
				return true;
				break;
			}
		}
	}
	return false;
}

void LButton::render(int i)
{
	//Show current button sprite
	gSpriteClips[mCurrentSprite].x = i * 200;
	gButtonSpriteSheetTexture.render(mPosition.x, mPosition.y, &gSpriteClips[mCurrentSprite]);
}

Tile::Tile(int x, int y, int tileType)
{
	//Get the offsets
	mBox.x = x;
	mBox.y = y;

	//Set the collision box
	mBox.w = TILE_WIDTH;
	mBox.h = TILE_HEIGHT;

	//Get the tile type
	mType = tileType;
}

void Tile::render(SDL_Rect& camera)
{
	//If the tile is on screen
	if (checkCollision(camera, mBox))
	{
		//Show the tile
		gTileTexture.render(mBox.x - camera.x, mBox.y - camera.y, &gTileClips[mType]);
	}
}

int Tile::getType()
{
	return mType;
}

void Tile::setType(int Type)
{
	mType = Type;
}

SDL_Rect Tile::getBox()
{
	return mBox;
}



Dot::Dot(int x, int y)
{
	life = 3;
	//Initialize the collision box
	mBox.x = x;
	mBox.y = y;
	mBox.w = DOT_WIDTH;
	mBox.h = DOT_HEIGHT;

	//Initialize the velocity
	mVelX = 0;
	mVelY = 0;

	totalBomb = 1;

	step = 0;
	fade = 0;
	right = false;
	left = false;

	alpha = 1;

	immortal = false;
}

void Dot::handleEvent1(SDL_Event& e)
{
	if (!getDead())
	{
		//If a key was pressed
		if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
		{
			if (step == 0) step++;
			//Adjust the velocity
			switch (e.key.keysym.sym)
			{
			case SDLK_w:
				mVelY -= DOT_VEL;
				left = true;
				break;
			case SDLK_s:
				mVelY += DOT_VEL;
				right = true;
				break;
			case SDLK_a:
				mVelX -= DOT_VEL;
				left = true;
				break;
			case SDLK_d:
				mVelX += DOT_VEL;
				right = true;
				break;
			}
		}
		//If a key was released
		else if (e.type == SDL_KEYUP && e.key.repeat == 0)
		{
			//Adjust the velocity
			switch (e.key.keysym.sym)
			{
			case SDLK_w:
				mVelY += DOT_VEL;
				left = false;
				break;
			case SDLK_s:
				mVelY -= DOT_VEL;
				right = false;
				break;
			case SDLK_a:
				mVelX += DOT_VEL;
				left = false;
				break;
			case SDLK_d:
				mVelX -= DOT_VEL;
				right = false;
				break;
			}
		}
	}
}

void Dot::handleEvent2(SDL_Event& e)
{
	if (!getDead())
	{
		//If a key was pressed
		if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
		{
			if (step == 0) step++;
			//Adjust the velocity
			switch (e.key.keysym.sym)
			{
			case SDLK_UP:
				mVelY -= DOT_VEL;
				left = true;
				break;
			case SDLK_DOWN:
				mVelY += DOT_VEL;
				right = true;
				break;
			case SDLK_LEFT:
				mVelX -= DOT_VEL;
				left = true;
				break;
			case SDLK_RIGHT:
				mVelX += DOT_VEL;
				right = true;
				break;
			}
		}
		//If a key was released
		else if (e.type == SDL_KEYUP && e.key.repeat == 0)
		{
			//Adjust the velocity
			switch (e.key.keysym.sym)
			{
			case SDLK_UP:
				mVelY += DOT_VEL;
				left = false;
				break;
			case SDLK_DOWN:
				mVelY -= DOT_VEL;
				right = false;
				break;
			case SDLK_LEFT:
				mVelX += DOT_VEL;
				left = false;
				break;
			case SDLK_RIGHT:
				mVelX -= DOT_VEL;
				right = false;
				break;
			}
		}
	}
}

void Dot::move(Tile* tiles[], Bomb* bomb[], Bot* bot[], Dot* otherDot, Bomb* bomb2[])
{
	//Move the dot left or right 
	mBox.x += mVelX;
	getIteam(tiles, bomb, bot);

	//If the dot went too far to the left or right or touched a wall
	if (otherDot != NULL || bomb2 != NULL)
	{
		if (touchesWall(mBox, tiles) || touchBomb(mBox, bomb) || checkCollision(mBox, otherDot->getRect()) || touchesBomb(mBox, bomb2))
		{
			mBox = touchesEgdeOfWall(mBox, tiles, mVelX, 0);
			//move back
			mBox.x -= mVelX;
		}
	}
	else
	{
		if (touchesWall(mBox, tiles) || touchBomb(mBox, bomb))
		{
			mBox = touchesEgdeOfWall(mBox, tiles, mVelX, 0);
			//move back
			mBox.x -= mVelX;
		}
	}


	mBox.y += mVelY;
	getIteam(tiles, bomb, bot);

	//If the dot went too far up or down or touched a wall
	if (otherDot != NULL || bomb2 != NULL)
	{
		if (touchesWall(mBox, tiles) || touchBomb(mBox, bomb) || checkCollision(mBox, otherDot->getRect()) || touchesBomb(mBox, bomb2))
		{
			mBox = touchesEgdeOfWall(mBox, tiles, 0, mVelY);
			//move back
			mBox.y -= mVelY;
		}
	}
	else
	{
		if (touchesWall(mBox, tiles) || touchBomb(mBox, bomb))
		{
			mBox = touchesEgdeOfWall(mBox, tiles, 0, mVelY);
			//move back
			mBox.y -= mVelY;
		}
	}
}



void Dot::getIteam(Tile* tiles[], Bomb* bomb[], Bot* bot[])
{
	SDL_Rect iBox = mBox;
	iBox.x += 10;
	iBox.y += 10;
	iBox.w -= 20;
	iBox.h -= 20;
	for (int i = 0; i < TOTAL_TILES; ++i)
	{
		//If the tile is a wall type tile
		if ((tiles[i]->getType() >= BombIteam) && (tiles[i]->getType() <= Door))
		{
			//If the collision box touches the wall tile
			if (checkCollision(iBox, tiles[i]->getBox()))
			{
				if (tiles[i]->getType() == BombIteam)
				{
					if (totalBomb < MaxBomb - 1)
					{
						totalBomb++;
					}
					bomb[totalBomb - 1]->setLength(bomb[0]->getLength());
					tiles[i]->setType(0);
				}
				if (tiles[i]->getType() == LengthIteam)
				{
					for (int j = 0; j < totalBomb; j++)
					{
						bomb[j]->incLength();
					}
					tiles[i]->setType(0);
				}
				if (tiles[i]->getType() == SpeedIteam)
				{
					if (mVelX > 0)
					{
						mVelX += DOT_VEL - 1;
					}
					if (mVelY > 0)
					{
						mVelY += DOT_VEL - 1;
					}
					if (mVelX < 0)
					{
						mVelX -= DOT_VEL - 1;
					}
					if (mVelY < 0)
					{
						mVelY -= DOT_VEL - 1;
					}
					DOT_VEL++;
					tiles[i]->setType(0);
				}
				if (tiles[i]->getType() == Door)
				{
					bool nextLevel = true;
					for (int i = 0; i < count; i++)
					{
						if (!bot[i]->getDead()) nextLevel = false;
					}
					if (nextLevel)
					{
						EndGameTexture.render(0, 0);
						NextLevelText.render(SCREEN_WIDTH / 2 - NextLevelText.getWidth(), SCREEN_HEIGHT / 2 - NextLevelText.getWidth());
						SDL_RenderPresent(gRenderer);
						SDL_Delay(1500);
						UpLevel = true;
					}
				}
			}
		}
	}
}

void Dot::setCamera(SDL_Rect& camera)
{
	//Center the camera over the dot
	camera.x = (mBox.x + DOT_WIDTH / 2) - SCREEN_WIDTH / 2;
	camera.y = (mBox.y + DOT_HEIGHT / 2) - SCREEN_HEIGHT / 2;

	//Keep the camera in bounds
	if (camera.x < 0)
	{
		camera.x = 0;
	}
	if (camera.y < 0)
	{
		camera.y = 0;
	}
	if (camera.x > LEVEL_WIDTH - camera.w)
	{
		camera.x = LEVEL_WIDTH - camera.w;
	}
	if (camera.y > LEVEL_HEIGHT - camera.h)
	{
		camera.y = LEVEL_HEIGHT - camera.h;
	}
}

void Dot::death(Bomb* bomb[], Bot* bot[], Tile* tiles[])
{
	SDL_Rect iBox = mBox;
	iBox.x += 10;
	iBox.y += 10;
	iBox.w -= 20;
	iBox.h -= 20;
	for (int j = 0; j < totalBomb; j++)
	{
		if (bomb[j]->getIsExploding())
		{
			int x = bomb[j]->getRect().x / 32;
			int y = bomb[j]->getRect().y / 32;
			for (int i = 1; i <= bomb[j]->getLength(); i++)
			{
				if (tiles[(y - i) * 39 + x]->getType() == 0)
				{
					SDL_Rect up = bomb[j]->getRect();
					up.y -= 32 * i;
					if (checkCollision(iBox, up))
					{
						if (!immortal)
						{
							life--;
							immortal = true;
							time = SDL_GetTicks();
						}
					}
				}
				else break;
			}
			for (int i = 1; i <= bomb[j]->getLength(); i++)
			{
				if (tiles[(y + i) * 39 + x]->getType() == 0)
				{
					SDL_Rect down = bomb[j]->getRect();
					down.y += 32 * i;
					if (checkCollision(iBox, down))
					{
						if (!immortal)
						{
							life--;
							immortal = true;
							time = SDL_GetTicks();
						}
					}
				}
				else break;
			}
			for (int i = 1; i <= bomb[j]->getLength(); i++)
			{
				if (tiles[y * 39 + x - i]->getType() == 0)
				{
					SDL_Rect left = bomb[j]->getRect();
					left.x -= 32 * i;
					if (checkCollision(iBox, left))
					{
						if (!immortal)
						{
							life--;
							immortal = true;
							time = SDL_GetTicks();
						}
					}
				}
				else break;
			}
			for (int i = 1; i <= bomb[j]->getLength(); i++)
			{
				if (tiles[y * 39 + x + i]->getType() == 0)
				{
					SDL_Rect right = bomb[j]->getRect();
					right.x += 32 * i;
					if (checkCollision(iBox, right))
					{
						if (!immortal)
						{
							life--;
							immortal = true;
							time = SDL_GetTicks();
						}
					}
				}
				else break;
			}
			if (checkCollision(iBox, bomb[j]->getRect()))
			{
				if (!immortal)
				{
					life--;
					immortal = true;
					time = SDL_GetTicks();
				}
			}
		}
	}
	for (int i = 0; i < count; i++)
	{
		if (checkCollision(mBox, bot[i]->getRect()))
		{
			if (!immortal)
			{
				life--;
				immortal = true;
				time = SDL_GetTicks();
			}
			break;
		}
	}
}

void Dot::recovery()
{
	if (SDL_GetTicks() - time >= 2000)
	{
		immortal = false;
	}
}

void Dot::render(SDL_Rect& camera, int t)
{
	if (!getDead())
	{
		if (immortal)
		{
			alpha++;
			if (alpha / 6 > 6) alpha = 1;
			if (alpha / 4 >= 0 && alpha / 4 <= 2)
			{
				gDotTexture.setAlpha(32);
			}
			else gDotTexture.setAlpha(255);
		}
		else
		{
			gDotTexture.setAlpha(255);
		}
		//render player
		SDL_RendererFlip flipType = SDL_FLIP_NONE;
		if (right)
		{
			flipType = SDL_FLIP_NONE;
			step++;
			if (step / 4 > 4) step = 1;
		}
		else if (left)
		{
			flipType = SDL_FLIP_HORIZONTAL;;
			step++;
			if (step / 4 > 4) step = 1;
		}
		else if (!left && !right)
		{
			step = 0;
		}
		else 
		{
			step = 0;
		}
		SDL_Rect walk = mBox;
		walk.x = (step / 4) * 32;
		walk.y = 0;
		walk.w = 32;
		walk.h = 32;
		if (t == 1)
		{
			gDotTexture.render(mBox.x - camera.x, mBox.y - camera.y, &walk, 0, NULL, flipType);
		}
		else
		{
			gDotTexture2.render(mBox.x - camera.x, mBox.y - camera.y, &walk, 0, NULL, flipType);
		}
		fade = 0;
	}
	else
	{
		stop();
		fade++;
		if (fade / 25 >= 4) step = 99;
		SDL_Rect walk = mBox;
		walk.x = (fade / 25 + 5) * 32;
		walk.y = 0;
		walk.w = 32;
		walk.h = 32;
		if (t == 1)
		{
			gDotTexture.render(mBox.x - camera.x, mBox.y - camera.y, &walk);
		}
		else
		{
			gDotTexture2.render(mBox.x - camera.x, mBox.y - camera.y, &walk);
		}
	}
}

void Dot::deadRender(SDL_Rect camera)
{
	SDL_Rect walk = mBox;
	walk.x = 5 * 32;
	walk.y = 0;
	walk.w = 32;
	walk.h = 32;
	gDotTexture.render(mBox.x - camera.x, mBox.y - camera.y, &walk);
	SDL_RenderPresent(gRenderer);
	SDL_Delay(1000);
}

int Dot::getRect_x()
{
	return mBox.x;
}

int Dot::getRect_y()
{
	return mBox.y;
}

SDL_Rect Dot::getRect()
{
	return mBox;
}

int Dot::getTotalBomb()
{
	return totalBomb;
}

bool Dot::getDead()
{
	if (life <= 0)
	{
		return true;
	}
	return false;
}

int Dot::getLife()
{
	return life;
}
void Dot::setLife(int _life)
{
	life = _life;
}
void Dot::setRect(int x, int y)
{
	mBox.x = x;
	mBox.y = y;
}
void Dot::resetVel()
{
	DOT_VEL = 2;
}

void Dot::stop()
{
	mVelX = 0;
	mVelY = 0;
	left = false;
	right = false;
}

void Dot::resetTotalBomb()
{
	totalBomb = 1;
}

Bot::Bot(int x, int y, bool k)
{
	//Initialize the collision box
	mBox.x = x;
	mBox.y = y;
	mBox.w = DOT_WIDTH;
	mBox.h = DOT_HEIGHT;

	//alive
	dead = false;

	change = true;

	intelligent = k;

	//Initialize the velocity
	mVelX = 0;
	mVelY = 0;
}


void Bot::move(Tile* tiles[], Bomb* bomb[])
{
	//Move the dot left or right
	mBox.x += mVelX;
	//If the dot went too far to the left or right or touched a wall
	if (touchesWall(mBox, tiles) || touchesBomb(mBox, bomb))
	{
		//move back
		touchesEgdeOfWall(mBox, tiles, mVelX, 0);
		mBox.x -= mVelX;
		mVelX = 0;
	}

	//Move the dot up or down
	mBox.y += mVelY;
	//If the dot went too far up or down or touched a wall
	if (touchesWall(mBox, tiles) || touchesBomb(mBox, bomb))
	{
		//move back
		touchesEgdeOfWall(mBox, tiles, 0, mVelY);
		mBox.y -= mVelY;
		mVelY = 0;
	}
}

void Bot::changeDirection()
{
	if (SDL_GetTicks() - time > 32 * 50)
	{
		change = true;
	}
}

void Bot::findDirection(Tile* tiles[], SDL_Rect dot)
{
	if (!dead)
	{
		if (!intelligent)
		{
			if (change)
			{
				mVelX = 0; mVelY = 0;
			}
			bool up = false, down = false, right = false, left = false;
			int x = mBox.x / 32;
			int y = mBox.y / 32;
			if (SDL_GetTicks() - time > 0 && mVelX == 0 && mVelY == 0)
			{
				if (tiles[y * 39 + x - 1]->getType() >= Land && tiles[y * 39 + x - 1]->getType() <= SpeedIteam) left = true;
				if (tiles[y * 39 + x + 1]->getType() >= Land && tiles[y * 39 + x + 1]->getType() <= SpeedIteam) right = true;
				if (tiles[(y - 1) * 39 + x]->getType() >= Land && tiles[(y - 1) * 39 + x]->getType() <= SpeedIteam) up = true;
				if (tiles[(y + 1) * 39 + x]->getType() >= Land && tiles[(y + 1) * 39 + x]->getType() <= SpeedIteam) down = true;
				bool k = true;
				if (up == false && down == false && left == false && right == false) k = false;

				while (k)
				{
					int random = rand() % 4;
					switch (random)
					{
					case 0:
						if (up)
						{
							k = false;
							mVelY = -1;
							change = false;
							time = SDL_GetTicks();
						}
						break;

					case 1:
						if (down)
						{
							k = false;
							mVelY = 1;
							change = false;
							time = SDL_GetTicks();
						}
						break;

					case 2:
						if (left)
						{
							k = false;
							mVelX = -1;
							change = false;
							time = SDL_GetTicks();
						}
						break;

					case 3:
						if (right)
						{
							k = false;
							mVelX = 1;
							change = false;
							time = SDL_GetTicks();
						}
						break;
					}
				}
			}
		}
		if (intelligent)
		{
			if (SDL_GetTicks() - time > 0)
			{
				int x = mBox.x / 32;
				int y = mBox.y / 32;
				if (x > 2)
				{
					if (tiles[y * 39 + x - 1]->getType() >= Land && tiles[y * 39 + x - 1]->getType() <= SpeedIteam
						&& tiles[y * 39 + x - 2]->getType() >= Land && tiles[y * 39 + x - 2]->getType() <= SpeedIteam
						&& tiles[y * 39 + x - 3]->getType() >= Land && tiles[y * 39 + x - 3]->getType() <= SpeedIteam)
					{
						if (checkCollision(dot, tiles[y * 39 + x - 1]->getBox())
							|| checkCollision(dot, tiles[y * 39 + x - 2]->getBox())
							|| checkCollision(dot, tiles[y * 39 + x - 3]->getBox()))
						{
							mVelY = 0;
							mVelX = -1;
						}
					}
				}
				if (x < 37)
				{
					if (tiles[y * 39 + x + 1]->getType() >= Land && tiles[y * 39 + x + 1]->getType() <= SpeedIteam
						&& tiles[y * 39 + x + 2]->getType() >= Land && tiles[y * 39 + x + 2]->getType() <= SpeedIteam
						&& tiles[y * 39 + x + 3]->getType() >= Land && tiles[y * 39 + x + 3]->getType() <= SpeedIteam)
					{
						if (checkCollision(dot, tiles[y * 39 + x + 1]->getBox())
							|| checkCollision(dot, tiles[y * 39 + x + 2]->getBox())
							|| checkCollision(dot, tiles[y * 39 + x + 3]->getBox()))
						{
							mVelY = 0;
							mVelX = 1;
						}
					}
				}
				if (y > 2)
				{

					if (tiles[(y - 1) * 39 + x]->getType() >= Land && tiles[(y - 1) * 39 + x]->getType() <= SpeedIteam
						&& tiles[(y - 2) * 39 + x]->getType() >= Land && tiles[(y - 2) * 39 + x]->getType() <= SpeedIteam
						&& tiles[(y - 3) * 39 + x]->getType() >= Land && tiles[(y - 3) * 39 + x]->getType() <= SpeedIteam)
					{
						if (checkCollision(dot, tiles[(y - 1) * 39 + x]->getBox())
							|| checkCollision(dot, tiles[(y - 2) * 39 + x]->getBox())
							|| checkCollision(dot, tiles[(y - 3) * 39 + x]->getBox()))
						{
							mVelX = 0;
							mVelY = -1;
						}
					}
				}
				if (y < 13)
				{
					if (tiles[(y + 1) * 39 + x]->getType() >= Land && tiles[(y + 1) * 39 + x]->getType() <= SpeedIteam
						&& tiles[(y + 2) * 39 + x]->getType() >= Land && tiles[(y + 2) * 39 + x]->getType() <= SpeedIteam
						&& tiles[(y + 3) * 39 + x]->getType() >= Land && tiles[(y + 3) * 39 + x]->getType() <= SpeedIteam)
					{
						if (checkCollision(dot, tiles[(y + 1) * 39 + x]->getBox())
							|| checkCollision(dot, tiles[(y + 2) * 39 + x]->getBox())
							|| checkCollision(dot, tiles[(y + 3) * 39 + x]->getBox()))
						{
							mVelX = 0;
							mVelY = +1;
						}
					}
				}
			}
			if (mVelX == 0 && mVelY == 0)
			{
				if (change)
				{
					mVelX = 0; mVelY = 0;
				}
				bool up = false, down = false, right = false, left = false;
				int x = mBox.x / 32;
				int y = mBox.y / 32;
				if (SDL_GetTicks() - time > 0 && mVelX == 0 && mVelY == 0)
				{
					if (tiles[y * 39 + x - 1]->getType() >= Land && tiles[y * 39 + x - 1]->getType() <= SpeedIteam) left = true;
					if (tiles[y * 39 + x + 1]->getType() >= Land && tiles[y * 39 + x + 1]->getType() <= SpeedIteam) right = true;
					if (tiles[(y - 1) * 39 + x]->getType() >= Land && tiles[(y - 1) * 39 + x]->getType() <= SpeedIteam) up = true;
					if (tiles[(y + 1) * 39 + x]->getType() >= Land && tiles[(y + 1) * 39 + x]->getType() <= SpeedIteam) down = true;
					bool k = true;
					if (up == false && down == false && left == false && right == false) k = false;

					while (k)
					{
						int random = rand() % 4;
						switch (random)
						{
						case 0:
							if (up)
							{
								k = false;
								mVelY = -1;
								change = false;
								time = SDL_GetTicks();
							}
							break;

						case 1:
							if (down)
							{
								k = false;
								mVelY = 1;
								change = false;
								time = SDL_GetTicks();
							}
							break;

						case 2:
							if (left)
							{
								k = false;
								mVelX = -1;
								change = false;
								time = SDL_GetTicks();
							}
							break;

						case 3:
							if (right)
							{
								k = false;
								mVelX = 1;
								change = false;
								time = SDL_GetTicks();
							}
							break;
						}
					}
				}
			}
		}
	}
}

void Bot::death(Bomb* bomb[], Tile* tiles[])
{
	for (int j = 0; j < MaxBomb; j++)
	{
		if (bomb[j]->getIsExploding())
		{
			int x = bomb[j]->getRect().x / 32;
			int y = bomb[j]->getRect().y / 32;
			for (int i = 1; i <= bomb[j]->getLength(); i++)
			{
				if (tiles[(y - i) * 39 + x]->getType() == 0)
				{
					SDL_Rect up = bomb[j]->getRect();
					up.y -= 32 * i;
					if (checkCollision(mBox, up))
					{
						dead = true;
						score += 100;
						mBox.x = 0;
						mBox.y = 0;
					}
				}
				else break;
			}
			for (int i = 1; i <= bomb[j]->getLength(); i++)
			{
				if (tiles[(y + i) * 39 + x]->getType() == 0)
				{
					SDL_Rect down = bomb[j]->getRect();
					down.y += 32 * i;
					if (checkCollision(mBox, down))
					{
						dead = true;
						score += 100;
						mBox.x = 0;
						mBox.y = 0;
					}
				}
				else break;
			}
			for (int i = 1; i <= bomb[j]->getLength(); i++)
			{
				if (tiles[y * 39 + x - i]->getType() == 0)
				{
					SDL_Rect left = bomb[j]->getRect();
					left.x -= 32 * i;
					if (checkCollision(mBox, left))
					{
						dead = true;
						score += 100;
						mBox.x = 0;
						mBox.y = 0;
					}
				}
				else break;
			}
			for (int i = 1; i <= bomb[j]->getLength(); i++)
			{
				if (tiles[y * 39 + x + i]->getType() == 0)
				{
					SDL_Rect right = bomb[j]->getRect();
					right.x += 32 * i;
					if (checkCollision(mBox, right))
					{
						dead = true;
						score += 100;
						mBox.x = 0;
						mBox.y = 0;
					}
				}
				else break;
			}
			if (checkCollision(mBox, bomb[j]->getRect()))
			{
				dead = true;
				score += 100;
				mBox.x = 0;
				mBox.y = 0;
			}
		}
	}
}

void Bot::render(SDL_Rect& camera)
{
	//Show the dot
	if (!dead)
	{
		if (!intelligent)
		{
			BotTexture1.render(mBox.x - camera.x, mBox.y - camera.y);
		}
		else
		{
			BotTexture2.render(mBox.x - camera.x, mBox.y - camera.y);
		}
	}
}

int Bot::getRect_x()
{
	return mBox.x;
}

int Bot::getRect_y()
{
	return mBox.y;
}

bool Bot::getDead()
{
	return dead;
}

SDL_Rect Bot::getRect()
{
	return mBox;
}


Bomb::Bomb() {
	mBomb.x = 0;
	mBomb.y = 0;
	mBomb.w = Bomb_WIDTH;
	mBomb.h = Bomb_HEIGHT;
	beUsed = false;
	through = false;
	length = 1;
	isExploding = false;
	check = false;
	step = 0;
}

void Bomb::explode(Tile* tiles[], SDL_Rect& camera)
{
	int x = mBomb.x / 32;
	int y = mBomb.y / 32;
	if (beUsed)
	{
		for (int i = 1; i <= length; i++)
		{
			if ((y - i) * 39 + x >= 0 && (y - i) * 39 + x <= TOTAL_TILES)
			{
				if (tiles[(y - i) * 39 + x]->getType() >= Box && tiles[(y - i) * 39 + x]->getType() <= BoxHasDoor)
				{
					switch (tiles[(y - i) * 39 + x]->getType())
					{
					case Box:
						tiles[(y - i) * 39 + x]->setType(BoxExploding);
						break;
					case BoxHasBombIteam:
						tiles[(y - i) * 39 + x]->setType(BoxHasBombIteamExploding);
						break;
					case BoxHasLengthIteam:
						tiles[(y - i) * 39 + x]->setType(BoxHasLengthIteamExploding);
						break;
					case BoxHasSpeedIteam:
						tiles[(y - i) * 39 + x]->setType(BoxHasSpeedIteamExploding);
						break;
					case BoxHasDoor:
						tiles[(y - i) * 39 + x]->setType(BoxHasDoorExploding);
						break;
					}
					break;
				}
				else if (tiles[(y - i) * 39 + x]->getType() == Wall)
				{
					break;
				}
			}
		}

		for (int i = 1; i <= length; i++)
		{
			if (y * 39 + x - i >= 0 && y * 39 + x - i <= TOTAL_TILES)
			{
				if (tiles[y * 39 + x - i]->getType() >= Box && tiles[y * 39 + x - i]->getType() <= BoxHasDoor)
				{
					switch (tiles[y * 39 + x - i]->getType())
					{
					case Box:
						tiles[y * 39 + x - i]->setType(BoxExploding);
						break;
					case BoxHasBombIteam:
						tiles[y * 39 + x - i]->setType(BoxHasBombIteamExploding);
						break;
					case BoxHasLengthIteam:
						tiles[y * 39 + x - i]->setType(BoxHasLengthIteamExploding);
						break;
					case BoxHasSpeedIteam:
						tiles[y * 39 + x - i]->setType(BoxHasSpeedIteamExploding);
						break;
					case BoxHasDoor:
						tiles[y * 39 + x - i]->setType(BoxHasDoorExploding);
						break;
					}
					break;
				}
				else if (tiles[y * 39 + x - i]->getType() == Wall)
				{
					break;
				}
			}
		}
		for (int i = 1; i <= length; i++)
		{
			if (y * 39 + x + i >= 0 && y * 39 + x + i <= TOTAL_TILES)
			{
				if (tiles[y * 39 + x + i]->getType() >= Box && tiles[y * 39 + x + i]->getType() <= BoxHasDoor)
				{
					switch (tiles[y * 39 + x + i]->getType())
					{
					case Box:
						tiles[y * 39 + x + i]->setType(BoxExploding);
						break;
					case BoxHasBombIteam:
						tiles[y * 39 + x + i]->setType(BoxHasBombIteamExploding);
						break;
					case BoxHasLengthIteam:
						tiles[y * 39 + x + i]->setType(BoxHasLengthIteamExploding);
						break;
					case BoxHasSpeedIteam:
						tiles[y * 39 + x + i]->setType(BoxHasSpeedIteamExploding);
						break;
					case BoxHasDoor:
						tiles[y * 39 + x + i]->setType(BoxHasDoorExploding);
						break;
					}
					break;
				}
				else if (tiles[y * 39 + x + i]->getType() == Wall)
				{
					break;
				}
			}
		}

		for (int i = 1; i <= length; i++)
		{
			if ((y + i) * 39 + x >= 0 && (y + i) * 39 + x <= TOTAL_TILES)
			{
				if (tiles[(y + i) * 39 + x]->getType() >= Box && tiles[(y + i) * 39 + x]->getType() <= BoxHasDoor)
				{
					switch (tiles[(y + i) * 39 + x]->getType())
					{
					case Box:
						tiles[(y + i) * 39 + x]->setType(BoxExploding);
						break;
					case BoxHasBombIteam:
						tiles[(y + i) * 39 + x]->setType(BoxHasBombIteamExploding);
						break;
					case BoxHasLengthIteam:
						tiles[(y + i) * 39 + x]->setType(BoxHasLengthIteamExploding);
						break;
					case BoxHasSpeedIteam:
						tiles[(y + i) * 39 + x]->setType(BoxHasSpeedIteamExploding);
						break;
					case BoxHasDoor:
						tiles[(y + i) * 39 + x]->setType(BoxHasDoorExploding);
						break;
					}
					break;
				}
				else if (tiles[(y + i) * 39 + x]->getType() == Wall)
				{
					break;
				}
			}
		}
	}
}

bool Bomb::handleEvent1(SDL_Event& e, SDL_Rect dot)
{
	if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE)
	{
		if (!beUsed)
		{
			beUsed = true;
			through = true;
			wait = SDL_GetTicks();
			mBomb.x = ((dot.x + Dot::DOT_HEIGHT / 2) / 32) * 32;
			mBomb.y = ((dot.y + Dot::DOT_WIDTH / 2) / 32) * 32;
			check = true;
			return true;
		}
	}
	return false;
}

bool Bomb::handleEvent2(SDL_Event& e, SDL_Rect dot)
{
	if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_KP_0)
	{
		if (!beUsed)
		{
			beUsed = true;
			through = true;
			wait = SDL_GetTicks();
			mBomb.x = ((dot.x + Dot::DOT_HEIGHT / 2) / 32) * 32;
			mBomb.y = ((dot.y + Dot::DOT_WIDTH / 2) / 32) * 32;
			check = true;
			return true;
		}
	}
	return false;
}

void Bomb::render(SDL_Rect& camera, Tile* tiles[])
{
	int x = mBomb.x / 32;
	int y = mBomb.y / 32;
	if (beUsed && SDL_GetTicks() - wait < 3000)
	{
		BombTexture.render(mBomb.x - camera.x, mBomb.y - camera.y);
	}
	else
	{
		if (SDL_GetTicks() - wait >= 3000 && SDL_GetTicks() - wait <= 3500 && beUsed)
		{
			step++;
			if (step / 5 >= 5) step = 24;
			SDL_Rect temp = mBomb;
			temp.x = 0;
			temp.y = (step / 5) * 32;

			BoomTexture.render(mBomb.x - camera.x, mBomb.y - camera.y, &temp);
			temp.x = 32;
			for (int i = 1; i <= length; i++)
			{
				if (tiles[(y - i) * 39 + x]->getType() == 0)
				{
					BoomTexture.render(mBomb.x - camera.x, mBomb.y - camera.y - TILE_HEIGHT * i, &temp, 90);
				}
				else break;
			}
			for (int i = 1; i <= length; i++)
			{
				if (tiles[(y + i) * 39 + x]->getType() == 0)
				{
					BoomTexture.render(mBomb.x - camera.x, mBomb.y - camera.y + TILE_HEIGHT * i, &temp, 270);
				}
				else break;
			}
			for (int i = 1; i <= length; i++)
			{
				if (tiles[y * 39 + x - i]->getType() == 0)
				{
					BoomTexture.render(mBomb.x - camera.x - TILE_WIDTH * i, mBomb.y - camera.y, &temp, 180);
				}
				else break;
			}
			for (int i = 1; i <= length; i++)
			{
				if (tiles[y * 39 + x + i]->getType() == 0)
				{
					BoomTexture.render(mBomb.x - camera.x + TILE_WIDTH * i, mBomb.y - camera.y, &temp, 0);
				}
				else break;
			}
			if (check)
			{
				explode(tiles, camera);
				check = false;
			}
			isExploding = true;
		}
		if (SDL_GetTicks() - wait > 3500 && beUsed)
		{
			step = 0;
			beUsed = false;
			mBomb.x = 0;
			mBomb.y = 0;
			through = false;
			isExploding = false;
			for (int i = 1; i <= length; i++)
			{
				if ((y - i) * 39 + x >= 0 && (y - i) * 39 + x <= TOTAL_TILES)
				{
					if (tiles[(y - i) * 39 + x]->getType() >= BoxExploding && tiles[(y - i) * 39 + x]->getType() <= BoxHasDoorExploding)
					{
						switch (tiles[(y - i) * 39 + x]->getType())
						{
						case BoxExploding:
							tiles[(y - i) * 39 + x]->setType(Land);
							break;
						case BoxHasBombIteamExploding:
							tiles[(y - i) * 39 + x]->setType(BombIteam);
							break;
						case BoxHasLengthIteamExploding:
							tiles[(y - i) * 39 + x]->setType(LengthIteam);
							break;
						case BoxHasSpeedIteamExploding:
							tiles[(y - i) * 39 + x]->setType(SpeedIteam);
							break;
						case BoxHasDoorExploding:
							tiles[(y - i) * 39 + x]->setType(Door);
							break;
						}
					}
					else if (tiles[(y - i) * 39 + x]->getType() == Wall)
					{
						break;
					}
				}
			}

			for (int i = 1; i <= length; i++)
			{
				if (y * 39 + x - i >= 0 && y * 39 + x - i <= TOTAL_TILES)
				{
					if (tiles[y * 39 + x - i]->getType() >= BoxExploding && tiles[y * 39 + x - i]->getType() <= BoxHasDoorExploding)
					{
						switch (tiles[y * 39 + x - i]->getType())
						{
						case BoxExploding:
							tiles[y * 39 + x - i]->setType(Land);
							break;
						case BoxHasBombIteamExploding:
							tiles[y * 39 + x - i]->setType(BombIteam);
							break;
						case BoxHasLengthIteamExploding:
							tiles[y * 39 + x - i]->setType(LengthIteam);
							break;
						case BoxHasSpeedIteamExploding:
							tiles[y * 39 + x - i]->setType(SpeedIteam);
							break;
						case BoxHasDoorExploding:
							tiles[y * 39 + x - i]->setType(Door);
							break;
						}
					}
					else if (tiles[y * 39 + x - i]->getType() == Wall)
					{
						break;
					}
				}
			}
			for (int i = 1; i <= length; i++)
			{
				if (y * 39 + x + i >= 0 && y * 39 + x + i <= TOTAL_TILES)
				{
					if (tiles[y * 39 + x + i]->getType() >= BoxExploding && tiles[y * 39 + x + i]->getType() <= BoxHasDoorExploding)
					{
						switch (tiles[y * 39 + x + i]->getType())
						{
						case BoxExploding:
							tiles[y * 39 + x + i]->setType(Land);
							break;
						case BoxHasBombIteamExploding:
							tiles[y * 39 + x + i]->setType(BombIteam);
							break;
						case BoxHasLengthIteamExploding:
							tiles[y * 39 + x + i]->setType(LengthIteam);
							break;
						case BoxHasSpeedIteamExploding:
							tiles[y * 39 + x + i]->setType(SpeedIteam);
							break;
						case BoxHasDoorExploding:
							tiles[y * 39 + x + i]->setType(Door);
							break;
						}
					}
					else if (tiles[y * 39 + x + i]->getType() == Wall)
					{
						break;
					}
				}
			}

			for (int i = 1; i <= length; i++)
			{
				if ((y + i) * 39 + x >= 0 && (y + i) * 39 + x <= TOTAL_TILES)
				{
					if (tiles[(y + i) * 39 + x]->getType() >= BoxExploding && tiles[(y + i) * 39 + x]->getType() <= BoxHasDoorExploding)
					{
						switch (tiles[(y + i) * 39 + x]->getType())
						{
						case BoxExploding:
							tiles[(y + i) * 39 + x]->setType(Land);
							break;
						case BoxHasBombIteamExploding:
							tiles[(y + i) * 39 + x]->setType(BombIteam);
							break;
						case BoxHasLengthIteamExploding:
							tiles[(y + i) * 39 + x]->setType(LengthIteam);
							break;
						case BoxHasSpeedIteamExploding:
							tiles[(y + i) * 39 + x]->setType(SpeedIteam);
							break;
						case BoxHasDoorExploding:
							tiles[(y + i) * 39 + x]->setType(Door);
							break;
						}
					}
					else if (tiles[(y + i) * 39 + x]->getType() == Wall)
					{
						break;
					}
				}
			}
		}
	}
}

SDL_Rect Bomb::getRect()
{
	return mBomb;
}

bool Bomb::getThrough()
{
	return through;
}
void Bomb::setThrough(bool _through)
{
	through = _through;
}

int Bomb::getX()
{
	return mBomb.x;
}
int Bomb::getY()
{
	return mBomb.y;
}
bool Bomb::getIsExploding()
{
	return isExploding;
}

void Bomb::incLength()
{
	length++;
}

int Bomb::getLength()
{
	return length;
}

void Bomb::resetLength()
{
	length = 1;
}

void Bomb::setLength(int _length)
{
	length = _length;
}

bool Bomb::getUsed()
{
	return beUsed;
}

void Bomb::setRect()
{
	beUsed = false;
}

LTimer::LTimer()
{
	//Initialize the variables
	mStartTicks = 0;
	mPausedTicks = 0;

	mPaused = false;
	mStarted = false;
}

void LTimer::start()
{
	//Start the timer
	mStarted = true;

	//Unpause the timer
	mPaused = false;

	//Get the current clock time
	mStartTicks = SDL_GetTicks();
	mPausedTicks = 0;
}

void LTimer::stop()
{
	//Stop the timer
	mStarted = false;

	//Unpause the timer
	mPaused = false;

	//Clear tick variables
	mStartTicks = 0;
	mPausedTicks = 0;
}

void LTimer::pause()
{
	//If the timer is running and isn't already paused
	if (mStarted && !mPaused)
	{
		//Pause the timer
		mPaused = true;

		//Calculate the paused ticks
		mPausedTicks = SDL_GetTicks() - mStartTicks;
		mStartTicks = 0;
	}
}

void LTimer::unpause()
{
	//If the timer is running and paused
	if (mStarted && mPaused)
	{
		//Unpause the timer
		mPaused = false;

		//Reset the starting ticks
		mStartTicks = SDL_GetTicks() - mPausedTicks;

		//Reset the paused ticks
		mPausedTicks = 0;
	}
}

Uint32 LTimer::getTicks()
{
	//The actual timer time
	Uint32 time = 0;

	//If the timer is running
	if (mStarted)
	{
		//If the timer is paused
		if (mPaused)
		{
			//Return the number of ticks when the timer was paused
			time = mPausedTicks;
		}
		else
		{
			//Return the current time minus the start time
			time = SDL_GetTicks() - mStartTicks;
		}
	}

	return time / 1000;
}

bool LTimer::isStarted()
{
	//Timer is running and paused or unpaused
	return mStarted;
}

bool LTimer::isPaused()
{
	//Timer is running and paused
	return mPaused && mStarted;
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}

				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}

				//Initialize SDL_mixer
				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
				{
					printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
					success = false;
				}
			}
		}
	}
	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load dot texture
	if (!gDotTexture.loadFromFile("E:\\UET\\Visual studio\\Bomberman\\Game\\x64\\Debug\\player.png"))
	{
		printf("Failed to load dot texture!\n");
		success = false;
	}
	if (!gDotTexture2.loadFromFile("E:\\UET\\Visual studio\\Bomberman\\Game\\x64\\Debug\\player2.png"))
	{
		printf("Failed to load dot texture!\n");
		success = false;
	}

	//Load tile texture
	if (!gTileTexture.loadFromFile("E:\\UET\\Visual studio\\Bomberman\\Game\\x64\\Debug\\map.png"))
	{
		printf("Failed to load tile set texture!\n");
		success = false;
	}

	if (!BombTexture.loadFromFile("E:\\UET\\Visual studio\\Bomberman\\Game\\x64\\Debug\\bomb.png"))
	{
		printf("Failed to load bomb texture!\n");
		success = false;
	}

	if (!BoomTexture.loadFromFile("E:\\UET\\Visual studio\\Bomberman\\Game\\x64\\Debug\\boom.png"))
	{
		printf("Failed to load boom texture!\n");
		success = false;
	}
	if (!BotTexture1.loadFromFile("E:\\UET\\Visual studio\\Bomberman\\Game\\x64\\Debug\\sciencetist3.png"))
	{
		printf("Failed to load bot texture!\n");
		success = false;
	}
	if (!BotTexture2.loadFromFile("E:\\UET\\Visual studio\\Bomberman\\Game\\x64\\Debug\\sciencetist2.png"))
	{
		printf("Failed to load bot texture!\n");
		success = false;
	}

	if (!EndGameTexture.loadFromFile("E:\\UET\\Visual studio\\Bomberman\\Game\\x64\\Debug\\bg.jpg"))
	{
		printf("Failed to load endgame texture!\n");
		success = false;
	}
	if (!MenuTexture.loadFromFile("E:\\UET\\Visual studio\\Bomberman\\Game\\x64\\Debug\\background.png"))
	{
		printf("Failed to load endgame texture!\n");
		success = false;
	}

	if (!PauseTexture.loadFromFile("E:\\UET\\Visual studio\\Bomberman\\Game\\x64\\Debug\\PauseGame.png"))
	{
		printf("Failed to load endgame texture!\n");
		success = false;
	}

	if (!gButtonSpriteSheetTexture.loadFromFile("E:\\UET\\Visual studio\\Bomberman\\Game\\x64\\Debug\\button.png"))
	{
		printf("Failed to load endgame texture!\n");
		success = false;
	}

	Music = Mix_LoadMUS("E:\\UET\\Visual studio\\Bomberman\\Game\\x64\\Debug\\TheFatRat - Fire [Chapter 8].mp3");
	if (Music == NULL)
	{
		printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}

	{
		gFont = TTF_OpenFont("E:\\UET\\Visual studio\\Bomberman\\Game\\x64\\Debug\\baijamjuree.ttf", 15);
		if (gFont == NULL)
		{
			printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
			success = false;
		}
		else
		{
			//Render text
			SDL_Color textColor = { 0, 0, 0 };
			if (!NextLevelText.loadFromRenderedText("Next Level", textColor))
			{
				printf("Failed to render text texture!\n");
				success = false;
			}
		}
	}


	
	gSpriteClips[BUTTON_SPRITE_MOUSE_OUT].x = 0;
	gSpriteClips[BUTTON_SPRITE_MOUSE_OUT].y = 0;
	gSpriteClips[BUTTON_SPRITE_MOUSE_OUT].w = BUTTON_WIDTH;
	gSpriteClips[BUTTON_SPRITE_MOUSE_OUT].h = BUTTON_HEIGHT;

	gSpriteClips[BUTTON_SPRITE_MOUSE_OVER_MOTION].x = 0;
	gSpriteClips[BUTTON_SPRITE_MOUSE_OVER_MOTION].y = 75;
	gSpriteClips[BUTTON_SPRITE_MOUSE_OVER_MOTION].w = BUTTON_WIDTH;
	gSpriteClips[BUTTON_SPRITE_MOUSE_OVER_MOTION].h = BUTTON_HEIGHT + 9;

	//Set buttons in corners
	gButtons[0].setPosition(SCREEN_WIDTH / 2 - BUTTON_WIDTH / 2, 400);
	gButtons[1].setPosition(SCREEN_WIDTH / 2 - BUTTON_WIDTH / 2, 400- 50);
	gButtons[2].setPosition(SCREEN_WIDTH / 2 - BUTTON_WIDTH / 2, 400 - 100);
	gButtons[3].setPosition(15, 372);
	gButtons[4].setPosition(SCREEN_WIDTH - 15 - BUTTON_WIDTH, 372);

	return success;
}

void close(Tile* tiles[], Bomb* bomb1[], Bomb* bomb2[], Bot* bot[])
{

	//Deallocate tiles
	if (currentLevel != 0)
	{
		for (int i = 0; i < TOTAL_TILES; ++i)
		{
			if (tiles[i] != NULL)
			{
				delete tiles[i];
				tiles[i] = NULL;
			}
		}
	}
	
	for (int i = 0; i < MaxBomb; ++i)
	{
		if (bomb1[i] != NULL)
		{
			delete bomb1[i];
			bomb1[i] = NULL;
		}
	}
	
	for (int i = 0; i < MaxBomb; ++i)
	{
		if (bomb2[i] != NULL)
		{
			delete bomb2[i];
			bomb2[i] = NULL;
		}
	}
	
	for (int i = 0; i < count; ++i)
	{
		if (bot[i] != NULL)
		{
			delete bot[i];
			bot[i] = NULL;
		}
	}
	
	//Free loaded images
	gDotTexture.free();
	gTileTexture.free();
	gButtonSpriteSheetTexture.free();
	gDotTexture.free();
	gTileTexture.free();
	BombTexture.free();
	BoomTexture.free();
	BotTexture1.free();
	BotTexture2.free();
	EndGameTexture.free();
	MenuTexture.free();
	PauseTexture.free();
	NextLevelText.free();
	gTimeTextTexture.free();
	LifeTextTexture.free();
	
	TTF_CloseFont(gFont);
	gFont = NULL;

	Mix_FreeMusic(Music);
	Music = NULL;

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}


bool checkCollision(SDL_Rect a, SDL_Rect b)
{
	//The sides of the rectangles
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	//Calculate the sides of rect A
	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

	//Calculate the sides of rect B
	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	bottomB = b.y + b.h;

	//If any of the sides from A are outside of B
	if (bottomA <= topB)
	{
		return false;
	}

	if (topA >= bottomB)
	{
		return false;
	}

	if (rightA <= leftB)
	{
		return false;
	}

	if (leftA >= rightB)
	{
		return false;
	}

	//If none of the sides from A are outside B
	return true;
}

bool setTiles(Tile* tiles[], Bot* bot[], int gameLevel)
{
	//Success flag
	bool tilesLoaded = true;

	//The tile offsets
	int x = 0, y = 0;

	//Open the map
	std::ifstream map;
	switch (currentLevel)
	{
	case 1:
		map.open("E:\\UET\\Visual studio\\Bomberman\\Game\\x64\\Debug\\level-1.map");
		break;
	case 2:
		map.open("E:\\UET\\Visual studio\\Bomberman\\Game\\x64\\Debug\\level-2.map");
		break;
	case 3:
		map.open("E:\\UET\\Visual studio\\Bomberman\\Game\\x64\\Debug\\level-3.map");
		break;
	case 4:
		map.open("E:\\UET\\Visual studio\\Bomberman\\Game\\x64\\Debug\\level-4.map");
		break;
	case 5:
		map.open("E:\\UET\\Visual studio\\Bomberman\\Game\\x64\\Debug\\level-5.map");
		break;
	case 6:
		map.open("E:\\UET\\Visual studio\\Bomberman\\Game\\x64\\Debug\\map1.map");
		break;
	case 7:
		map.open("E:\\UET\\Visual studio\\Bomberman\\Game\\x64\\Debug\\map2.map");
		break;
	case 8:
		map.open("E:\\UET\\Visual studio\\Bomberman\\Game\\x64\\Debug\\map3.map");
		break;
	}


	//If the map couldn't be loaded
	if (map.fail())
	{
		printf("Unable to load map file!\n");
		tilesLoaded = false;
	}
	else
	{
		//Initialize the tiles
		for (int i = 0; i < TOTAL_TILES; ++i)
		{
			//Determines what kind of tile will be made
			int tileType = -1;

			//Read tile from map file
			map >> tileType;

			//If the was a problem in reading the map
			if (map.fail())
			{
				//Stop loading map
				printf("Error loading map: Unexpected end of file!\n");
				tilesLoaded = false;
				break;
			}

			//clearBot(bot);
			//If the number is a valid tile number
			if ((tileType >= 0) && (tileType < TOTAL_TILE_SPRITES))
			{
				if (tileType == 1)
				{
					tileType = 0;
					bot[count++] = new Bot(x, y, false);
				}
				if (tileType == 18)
				{
					tileType = 0;
					bot[count++] = new Bot(x, y, true);
				}
				tiles[i] = new Tile(x, y, tileType);
			}
			//If we don't recognize the tile type
			else
			{
				//Stop loading map
				printf("Error loading map: Invalid tile type at %d!\n", i);
				tilesLoaded = false;
				break;
			}

			//Move to next tile spot
			x += TILE_WIDTH;

			//If we've gone too far
			if (x >= LEVEL_WIDTH)
			{
				//Move back
				x = 0;

				//Move to the next row
				y += TILE_HEIGHT;
			}
		}

		//Clip the sprite sheet
		if (tilesLoaded)
		{
			gTileClips[Land].x = 0;
			gTileClips[Land].y = 0;
			gTileClips[Land].w = TILE_WIDTH;
			gTileClips[Land].h = TILE_HEIGHT;

			gTileClips[Box].x = 0;
			gTileClips[Box].y = 64;
			gTileClips[Box].w = TILE_WIDTH;
			gTileClips[Box].h = TILE_HEIGHT;

			gTileClips[Wall].x = 0;
			gTileClips[Wall].y = 32;
			gTileClips[Wall].w = TILE_WIDTH;
			gTileClips[Wall].h = TILE_HEIGHT;

			gTileClips[BoxExploding].x = 0;
			gTileClips[BoxExploding].y = 96;
			gTileClips[BoxExploding].w = TILE_WIDTH;
			gTileClips[BoxExploding].h = TILE_HEIGHT;

			gTileClips[BombIteam].x = 32;
			gTileClips[BombIteam].y = 0;
			gTileClips[BombIteam].w = TILE_WIDTH;
			gTileClips[BombIteam].h = TILE_HEIGHT;

			gTileClips[BoxHasBombIteam].x = 0;
			gTileClips[BoxHasBombIteam].y = 64;
			gTileClips[BoxHasBombIteam].w = TILE_WIDTH;
			gTileClips[BoxHasBombIteam].h = TILE_HEIGHT;

			gTileClips[LengthIteam].x = 32;
			gTileClips[LengthIteam].y = 32;
			gTileClips[LengthIteam].w = TILE_WIDTH;
			gTileClips[LengthIteam].h = TILE_HEIGHT;

			gTileClips[BoxHasLengthIteam].x = 0;
			gTileClips[BoxHasLengthIteam].y = 64;
			gTileClips[BoxHasLengthIteam].w = TILE_WIDTH;
			gTileClips[BoxHasLengthIteam].h = TILE_HEIGHT;

			gTileClips[SpeedIteam].x = 32;
			gTileClips[SpeedIteam].y = 64;
			gTileClips[SpeedIteam].w = TILE_WIDTH;
			gTileClips[SpeedIteam].h = TILE_HEIGHT;

			gTileClips[BoxHasSpeedIteam].x = 0;
			gTileClips[BoxHasSpeedIteam].y = 64;
			gTileClips[BoxHasSpeedIteam].w = TILE_WIDTH;
			gTileClips[BoxHasSpeedIteam].h = TILE_HEIGHT;

			gTileClips[BoxHasBombIteamExploding].x = 0;
			gTileClips[BoxHasBombIteamExploding].y = 96;
			gTileClips[BoxHasBombIteamExploding].w = TILE_WIDTH;
			gTileClips[BoxHasBombIteamExploding].h = TILE_HEIGHT;

			gTileClips[BoxHasLengthIteamExploding].x = 0;
			gTileClips[BoxHasLengthIteamExploding].y = 96;
			gTileClips[BoxHasLengthIteamExploding].w = TILE_WIDTH;
			gTileClips[BoxHasLengthIteamExploding].h = TILE_HEIGHT;

			gTileClips[BoxHasSpeedIteamExploding].x = 0;
			gTileClips[BoxHasSpeedIteamExploding].y = 96;
			gTileClips[BoxHasSpeedIteamExploding].w = TILE_WIDTH;
			gTileClips[BoxHasSpeedIteamExploding].h = TILE_HEIGHT;

			gTileClips[BoxHasDoor].x = 0;
			gTileClips[BoxHasDoor].y = 64;
			gTileClips[BoxHasDoor].w = TILE_WIDTH;
			gTileClips[BoxHasDoor].h = TILE_HEIGHT;

			gTileClips[BoxHasDoorExploding].x = 0;
			gTileClips[BoxHasDoorExploding].y = 96;
			gTileClips[BoxHasDoorExploding].w = TILE_WIDTH;
			gTileClips[BoxHasDoorExploding].h = TILE_HEIGHT;

			gTileClips[Door].x = 32;
			gTileClips[Door].y = 96;
			gTileClips[Door].w = TILE_WIDTH;
			gTileClips[Door].h = TILE_HEIGHT;

			gTileClips[17].x = 0;
			gTileClips[17].y = 32;
			gTileClips[17].w = TILE_WIDTH;
			gTileClips[17].h = TILE_HEIGHT;
		}
	}

	//Close the file
	map.close();

	//If the map was loaded fine
	return tilesLoaded;
}

bool checkCoordinate(SDL_Rect a, Bomb& bomb)
{
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	a.x += 4; a.y += 4;
	a.w -= 8; a.h -= 8;

	//Calculate the sides of rect A
	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

	SDL_Rect b = bomb.getRect();
	//Calculate the sides of rect B
	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	bottomB = b.y + b.h;
	if (bomb.getThrough())
	{
		if (bottomA >= topB && bottomA <= bottomB && leftA >= leftB && leftA <= rightB)
		{
			return false;
		}
		if (topA >= topB && topA <= bottomB && rightA >= leftB && rightA <= rightB)
		{
			return false;
		}
		if (rightA >= leftB && rightA <= rightB && bottomA >= topB && bottomA <= bottomB)
		{
			return false;
		}
		if (leftA >= leftB && leftA <= rightB && topA >= topB && topA <= bottomB)
		{
			return false;
		}
	}
	bomb.setThrough(false);
	if (!checkCollision(a, b)) return false;
	return true;
}

bool touchesWall(SDL_Rect box, Tile* tiles[])
{
	//Go through the tiles
	for (int i = 0; i < TOTAL_TILES; ++i)
	{
		//If the tile is a wall type tile
		if (tiles[i]->getType() >= Wall && tiles[i]->getType() <= BoxHasDoorExploding || tiles[i]->getType() == BoxHasDoorExploding)
		{
			//If the collision box touches the wall tile
			if (checkCollision(box, tiles[i]->getBox()))
			{
				return true;
			}
		}
	}

	//If no wall tiles were touched
	return false;
}
SDL_Rect touchesEgdeOfWall(SDL_Rect box, Tile* tiles[], int velX, int velY)
{
	for (int i = 0; i < TOTAL_TILES; i++)
	{
		if (tiles[i]->getType() == Wall)
		{
			SDL_Rect wall = tiles[i]->getBox();
			if (SDL_HasIntersection(&box, &wall)) {
				// Adjust character's position
				if (velX != 0) {
					// If character was moving right and collides with obstacle, move character to left edge of obstacle
					if (box.y + Dot::DOT_HEIGHT > wall.y && box.y < wall.y + wall.h)
					{
						// If character is vertically aligned with obstacle, move up or down to avoid obstacle
						if (box.y + Dot::DOT_HEIGHT < wall.y + wall.h / 2 && tiles[i - 39]->getType() >= Land && tiles[i - 39]->getType() <= Door)
						{
							box.y -= 2;
						}
						else if (box.y > wall.y + wall.h * 1 / 2 && tiles[i + 39]->getType() >= Land && tiles[i - 39]->getType() <= Door)
						{
							box.y += 2;
						}
					}
				}

				if (velY != 0) {
					// If character was moving down and collides with obstacle, move character to top edge of obstacle
					if (box.x + Dot::DOT_WIDTH > wall.x && box.x < wall.x + wall.w)
					{
						// If character is horizontally aligned with obstacle, move left or right to avoid obstacle
						if (box.x + Dot::DOT_WIDTH < wall.x + wall.w / 2 && tiles[i - 1]->getType() >= Land && tiles[i - 39]->getType() <= Door)
						{
							box.x -= 2;
						}
						else if (box.x > wall.x + wall.w * 1 / 2 && tiles[i + 1]->getType() == Land && tiles[i - 39]->getType() <= Door)
						{
							box.x += 2;
						}
					}
				}
			}
		}
	}
	return box;
}

bool touchesBomb(SDL_Rect box, Bomb* bomb[])
{
	//Go through the tiles
	for (int i = 0; i < MaxBomb; ++i)
	{
		//If the collision box touches the wall tile
		if (checkCollision(box, bomb[i]->getRect()))
		{
			return true;
		}
	}
	//If no wall tiles were touched
	return false;
}

bool touchBomb(SDL_Rect box, Bomb* bomb[])
{
	//Go through the tiles
	for (int i = 0; i < MaxBomb; ++i)
	{
		//If the collision box touches the 
		if (checkCoordinate(box, *bomb[i]))
		{
			return true;
		}
	}

	//If no wall tiles were touched
	return false;
}

void NextLevel(Tile* tiles[], Dot& dot, Bot* bot[], Bomb* bomb[])
{
	count = 0;
	//for (int i = 0; i < totalBomb; i++)
	//{
	//	bomb[i]->setRect();
	//}
	if (!setTiles(tiles, bot, ++currentLevel))
	{
		printf("Failed to load media!\n");
	}
	dot.setRect(32, 32);
	UpLevel = false;
}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//The level tiles
		Tile* tileSet[TOTAL_TILES];
		Bot* bot[MaxBot];
		Bomb* bomb1[MaxBomb];
		Bomb* bomb2[MaxBomb];

		for (int i = 0; i < 20; i++)
		{
			bomb1[i] = new Bomb();
			bomb2[i] = new Bomb();
		}

		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//The dot that will be moving around on the screen
			Dot dot1(32, 32);
			Dot dot2(32 * 18, 32 * 13);

			//The application timer
			SDL_Color textColor = { 255, 255, 255, 255 };
			LTimer timer;

			//Level camera
			SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

			int transition = 0;

			//Play the music
			//Mix_PlayMusic(Music, -1);

			//While application is running
			while (!quit)
			{
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
					switch (game)
					{
					case Menu:
						if (gButtons[1].handleEvent(&e))
						{
							game = ChooseMode;
						}
						break;
					case ChooseMode:
						if (gButtons[4].handleEvent(&e))
						{
							mode = MultiPlayer;
							transition = 0;
							currentLevel = rand() % 3 + 6;
							//Load tile map
							count = 0;
							if (!setTiles(tileSet, bot, currentLevel))
							{
								printf("Failed to load tile set!\n");
								quit = true;
							}

							dot1.resetVel();
							dot1.resetTotalBomb();
							dot1.setLife(1);
							dot1.setRect(32, 32);
							for (int i = 0; i < dot1.getTotalBomb(); i++)
							{
								bomb1[i]->resetLength();
							}
							dot2.resetVel();
							dot2.resetTotalBomb();
							dot2.setLife(1);
							dot2.setRect(32 * 18, 32 * 13);
							for (int i = 0; i < dot1.getTotalBomb(); i++)
							{
								bomb2[i]->resetLength();
							}
							game = Playing;
						}
						if (gButtons[3].handleEvent(&e))
						{
							mode = NormalGame;
							transition = 0;
							currentLevel = 1;
							//Load tile map
							count = 0;
							if (!setTiles(tileSet, bot, currentLevel))
							{
								printf("Failed to load tile set!\n");
								quit = true;
							}
							timer.start();
							dot1.resetVel();
							dot1.resetTotalBomb();
							dot1.setRect(32, 32);
							dot1.setLife(3);
							for (int i = 0; i < dot1.getTotalBomb(); i++)
							{
								bomb1[i]->resetLength();
							}
							game = Playing;
						}
						break;
					case Pause:
						if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_p)
						{
							game = Playing;
							Mix_ResumeMusic();
							timer.unpause();
						}
						if (gButtons[0].handleEvent(&e))
						{
							game = Menu;
						}
						break;
					case Playing:
						if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_p)
						{
							game = Pause;
							Mix_PauseMusic();
							timer.pause();
						}
						else
						{
							if (mode == NormalGame)
							{
								dot1.handleEvent1(e);
								for (int i = 0; i < dot1.getTotalBomb(); i++)
								{
									if (bomb1[i]->handleEvent1(e, dot1.getRect())) break;
								}
							}
							else if (mode == MultiPlayer)
							{
								dot1.handleEvent1(e);
								for (int i = 0; i < dot1.getTotalBomb(); i++)
								{
									if (bomb1[i]->handleEvent1(e, dot1.getRect())) break;
								}
								dot2.handleEvent2(e);
								for (int i = 0; i < dot2.getTotalBomb(); i++)
								{
									if (bomb2[i]->handleEvent2(e, dot2.getRect())) break;
								}
							}
						}

					case EndGame:
						if (gButtons[2].handleEvent(&e))
						{
							mode = NormalGame;
							transition = 0;
							currentLevel = 1;
							//Load tile map
							count = 0;
							if (!setTiles(tileSet, bot, currentLevel))
							{
								printf("Failed to load tile set!\n");
								quit = true;
							}
							timer.start();
							dot1.resetVel();
							dot1.resetTotalBomb();
							dot1.setRect(32, 32);
							dot1.setLife(3);
							for (int i = 0; i < dot1.getTotalBomb(); i++)
							{
								bomb1[i]->resetLength();
							}
							game = Playing;
						}
						if (gButtons[0].handleEvent(&e))
						{
							game = Menu;
						}
						break;
					case Player1Win:
						if (gButtons[0].handleEvent(&e))
						{
							game = Menu;
						}
						break;
					case Player2Win:
						if (gButtons[0].handleEvent(&e))
						{
							game = Menu;
						}
						break;
					case WinGame:
						if (gButtons[0].handleEvent(&e))
						{
							game = Menu;
						}
						break;
					}
				}

				//Move the dot
				if (game == Menu)
				{
					MenuTexture.render(0, 0);
					gButtons[1].render(1);
					SDL_RenderPresent(gRenderer);
				}
				else if (game == ChooseMode)
				{
					MenuTexture.render(0, 0);
					gButtons[3].render(3);
					gButtons[4].render(4);
					SDL_RenderPresent(gRenderer);
				}
				else if (game == Pause)
				{
					dot1.stop();
					PauseTexture.setAlpha(10);
					PauseTexture.render(0, 0);
					gButtons[0].render(0);
					SDL_RenderPresent(gRenderer);
					dot1.stop();
					dot2.stop();
				}
				else if (game == Playing && mode == NormalGame)
				{

					int hours = timer.getTicks() / 3600;
					int mins = (timer.getTicks() % 3600) / 60;
					int secs = (timer.getTicks() % 3600) % 60;

					std::stringstream timerText;
					timerText.str("");
					timerText << "Time: " << std::to_string(hours) << "h" << std::to_string(mins) << "min" << std::to_string(secs) << "sec";

					if (!gTimeTextTexture.loadFromRenderedText(timerText.str().c_str(), textColor))
					{
						printf("Unable to render time texture!\n");
					}

					std::stringstream lifeText;
					lifeText.str("");
					lifeText << "Life: " << dot1.getLife();
					if (!LifeTextTexture.loadFromRenderedText(lifeText.str().c_str(), textColor))
					{
						printf("Unable to render time texture!\n");
					}

					dot1.move(tileSet, bomb1, bot);
					dot1.setCamera(camera);
					dot1.death(bomb1, bot, tileSet);
					dot1.recovery();

					for (int i = 0; i < count; i++)
					{
						bot[i]->changeDirection();
						bot[i]->findDirection(tileSet, dot1.getRect());
						bot[i]->move(tileSet, bomb1);
						bot[i]->death(bomb1, tileSet);
					}


					//Clear screen
					SDL_SetRenderDrawColor(gRenderer, 0x1F, 0x2F, 0x3F, 0x4F);
					SDL_RenderClear(gRenderer);

					//Render level
					for (int i = 0; i < TOTAL_TILES; ++i)
					{
						tileSet[i]->render(camera);
					}

					for (int i = 0; i < count; ++i)
					{
						bot[i]->render(camera);
					}

					//Render dot
					dot1.render(camera, 1);
					for (int i = 0; i < dot1.getTotalBomb(); i++)
					{
						bomb1[i]->render(camera, tileSet);
					}

					gTimeTextTexture.render(2, 10);
					LifeTextTexture.render(SCREEN_WIDTH - 60, 10);

					if (dot1.getDead())
					{
						transition++;
						if (transition > 100)
						{
							game = EndGame;
						}
					}
					//Update screen
					SDL_RenderPresent(gRenderer);
					if (UpLevel)
					{
						if (currentLevel < LevelNormal) NextLevel(tileSet, dot1, bot, bomb1);
						else
						{
							game = WinGame;
						}
					}
				}

				else if (game == Playing && mode == MultiPlayer)
				{
					dot1.move(tileSet, bomb1, bot, &dot2, bomb2);
					dot1.death(bomb1, bot, tileSet);
					dot1.death(bomb2, bot, tileSet);
					dot1.recovery();

					dot2.move(tileSet, bomb2, bot, &dot1, bomb1);
					dot2.death(bomb1, bot, tileSet);
					dot2.death(bomb2, bot, tileSet);
					dot2.recovery();


					//Clear screen
					SDL_SetRenderDrawColor(gRenderer, 0x1F, 0x2F, 0x3F, 0x4F);
					SDL_RenderClear(gRenderer);

					//Render level
					for (int i = 0; i < TOTAL_TILES; ++i)
					{
						tileSet[i]->render(camera);
					}

					//Render dot
					dot1.render(camera, 1);
					dot2.render(camera, 2);

					for (int i = 0; i < dot1.getTotalBomb(); i++)
					{
						bomb1[i]->render(camera, tileSet);
					}
					for (int i = 0; i < dot2.getTotalBomb(); i++)
					{
						bomb2[i]->render(camera, tileSet);
					}
					//Update screen
					SDL_RenderPresent(gRenderer);
					if (dot1.getDead())
					{
						transition++;
						if (transition > 100)
						{
							game = Player2Win;
						}
					}
					else if (dot2.getDead())
					{
						transition++;
						if (transition > 100)
						{
							game = Player1Win;
						}
					}
				}

				else if (game == EndGame)
				{
					dot1.stop();
					MenuTexture.render(0, 0);
					gButtons[0].render(0);
					gButtons[2].render(2);
					SDL_RenderPresent(gRenderer);
				}

				else if (game == WinGame)
				{
					dot1.stop();
					MenuTexture.render(0, 0);
					gButtons[0].render(0);
					SDL_RenderPresent(gRenderer);
				}

				else if (game == Player1Win)
				{
					dot1.stop();
					dot2.stop();
					MenuTexture.render(0, 0);
					gButtons[0].render(0);
					SDL_RenderPresent(gRenderer);
				}

				else if (game == Player2Win)
				{
					dot1.stop();
					dot2.stop();
					MenuTexture.render(0, 0);
					gButtons[0].render(0);
					SDL_RenderPresent(gRenderer);
				}
			}
		}
		//Free resources and close SDL
		close(tileSet, bomb1, bomb2, bot);
	}
	return 0;
}
