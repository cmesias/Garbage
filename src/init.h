int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 360;


int count_digit(int number) {
   return int(log10(number) + 1);
}

float roundMe(int var)
{
    // 37.66666 * 100 =3766.66
    // 3766.66 + .5 =3767.16    for rounding off value
    // then type cast to int so value is 3767
    // then divided by 100 so the value converted into 37.67
	int value = (int)(var * 100 + .5);

    return  (int)value / 100;
}

// Texture wrapper class
class LTexture {
public:
	//Initializes variables
	LTexture();

	//Deallocates memory
	~LTexture();

	//Loads image at specified path
	bool loadFromFile(std::string path);

#ifdef _SDL_TTF_H
	//Creates image from font string
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor,
			TTF_Font *fontname);
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
	void render(int x, int y, int w, int h, SDL_Rect* clip = NULL,
			double angle = 0.0, SDL_Point* center = NULL,
			SDL_RendererFlip flip = SDL_FLIP_NONE);

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

class LWindow {
public:
	//Intializes internals
	LWindow();

	//Creates window
	bool init();

	//Creates renderer from internal window
	SDL_Renderer* createRenderer();

	//Handles window events
	void handleEvent(SDL_Event& e);

	//Deallocates internals
	void free();

	//Window dimensions
	int getWidth();
	int getHeight();

	//Window focii
	bool hasMouseFocus();
	bool hasKeyboardFocus();
	bool isMinimized();

private:
	//Window data
	SDL_Window* mWindow;

	//Window dimensions
	int mWidth;
	int mHeight;

	//Window focus
	bool mMouseFocus;
	bool mKeyboardFocus;
	bool mFullScreen;
	bool mMinimized;
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Our custom window
LWindow gWindow;

//The window renderer
SDL_Renderer* gRenderer = NULL;

LTexture::LTexture() {
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture() {
	//Deallocate
	free();
}

bool LTexture::loadFromFile(std::string path) {
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL) {
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(),
		IMG_GetError());
	} else {
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE,
				SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL) {
			printf("Unable to create texture from %s! SDL Error: %s\n",
					path.c_str(), SDL_GetError());
		} else {
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

#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText(std::string textureText,
		SDL_Color textColor, TTF_Font *fontname) {
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(fontname,
			textureText.c_str(), textColor);
	if (textSurface != NULL) {
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL) {
			//printf(
			//		"Unable to create texture from rendered text! SDL Error: %s\n",
			//		SDL_GetError());
		} else {
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	} else {
		//printf("Unable to render text surface! SDL_ttf Error: %s\n",
		//TTF_GetError());
	}

	//Return success
	return mTexture != NULL;
}
#endif

void LTexture::free() {
	//Free texture if it exists
	if (mTexture != NULL) {
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue) {
	//Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending) {
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha) {
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, int w, int h, SDL_Rect* clip, double angle,
				SDL_Point* center, SDL_RendererFlip flip) {


	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, w, h };

	//Set clip rendering dimensions
	if (clip != NULL) {
		renderQuad.w = w;
		renderQuad.h = h;
	}

	//Render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center,
			flip);
}

int LTexture::getWidth() {
	return mWidth;
}

int LTexture::getHeight() {
	return mHeight;
}

LWindow::LWindow() {
	//Initialize non-existant window
	mWindow = NULL;
	mMouseFocus = false;
	mKeyboardFocus = false;
	mFullScreen = false;
	mMinimized = false;
	mWidth = 0;
	mHeight = 0;
}

bool LWindow::init() {
	//Create window
	mWindow = SDL_CreateWindow("I am Garbage?", SDL_WINDOWPOS_UNDEFINED,
	SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
			SDL_WINDOW_SHOWN);
	if (mWindow != NULL) {
		mMouseFocus = true;
		mKeyboardFocus = true;
		mWidth = SCREEN_WIDTH;
		mHeight = SCREEN_HEIGHT;
	}

	return mWindow != NULL;
}

SDL_Renderer* LWindow::createRenderer() {
	return SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void LWindow::handleEvent(SDL_Event& e) {
	//Window event occured
	if (e.type == SDL_WINDOWEVENT) {
		//Caption update flag
		bool updateCaption = false;

		switch (e.window.event) {
		//Get new dimensions and repaint on window size change
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			mWidth = e.window.data1;
			mHeight = e.window.data2;
			SDL_RenderPresent(gRenderer);
			break;

			//Repaint on exposure
		case SDL_WINDOWEVENT_EXPOSED:
			SDL_RenderPresent(gRenderer);
			break;

			//Mouse entered window
		case SDL_WINDOWEVENT_ENTER:
			mMouseFocus = true;
			updateCaption = true;
			break;

			//Mouse left window
		case SDL_WINDOWEVENT_LEAVE:
			mMouseFocus = false;
			updateCaption = true;
			break;

			//Window has keyboard focus
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			mKeyboardFocus = true;
			updateCaption = true;
			break;

			//Window lost keyboard focus
		case SDL_WINDOWEVENT_FOCUS_LOST:
			mKeyboardFocus = false;
			updateCaption = true;
			break;

			//Window minimized
		case SDL_WINDOWEVENT_MINIMIZED:
			mMinimized = true;
			break;

			//Window maxized
		case SDL_WINDOWEVENT_MAXIMIZED:
			mMinimized = false;
			break;

			//Window restored
		case SDL_WINDOWEVENT_RESTORED:
			mMinimized = false;
			break;
		}

		//Update window caption with new data
		if (updateCaption) {
			std::stringstream caption;
			caption << "I am Garbage?";

			SDL_SetWindowTitle(mWindow, caption.str().c_str());
		}
	}
	//Enter exit full screen on return key
	//else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
	else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F11) {
		if (mFullScreen) {
			//SDL_SetWindowFullscreen(mWindow, SDL_FALSE);
			//mFullScreen = false;
		} else {
			//SDL_SetWindowFullscreen(mWindow, SDL_TRUE);
			//mFullScreen = true;
			//mMinimized = false;
		}
	}
}

void LWindow::free() {
	if (mWindow != NULL) {
		SDL_DestroyWindow(mWindow);
	}

	mMouseFocus = false;
	mKeyboardFocus = false;
	mWidth = 0;
	mHeight = 0;
}

int LWindow::getWidth() {
	return mWidth;
}

int LWindow::getHeight() {
	return mHeight;
}

bool LWindow::hasMouseFocus() {
	return mMouseFocus;
}

bool LWindow::hasKeyboardFocus() {
	return mKeyboardFocus;
}

bool LWindow::isMinimized() {
	return mMinimized;
}

bool init() {
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init( SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	} else {
		//Set texture filtering to linear
		SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1");

		/////////////////////////////
		////////// DRAWING //////////
		//Create window
		gWindow.init();
		//Create vsynced renderer for window
		gRenderer = gWindow.createRenderer();
		//Initialize renderer color
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		////////// DRAWING //////////
		/////////////////////////////

		//Initialize PNG loading
		int imgFlags = IMG_INIT_PNG;
		IMG_Init(imgFlags);
		TTF_Init();

		//SDL_Init(SDL_INIT_EVERYTHING);

		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
			printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n",
			Mix_GetError());
			success = false;
		}

	}

	return success;
}

void checkMaxLimitDouble(double &value, double maxLimitNeg, double maxLimitNegNew, double maxLimitPos, double maxLimitPosNew){
	//Set max limits for one number (double)
	if (value <= maxLimitNeg){
		value = maxLimitNegNew;
	}
	if (value >= maxLimitPos){
		value= maxLimitPosNew;
	}
}

// Check collision between 2 objects
bool checkCollision(int x, int y, int w, int h, int x2, int y2, int w2, int h2){
	bool collide;
	if (x+w > x2 && x < x2 + w2 && y+h > y2 && y < y2 + h2){
		collide = true;
	}else{
		collide = false;
	}
	return collide;
}

// Colors
SDL_Color black = { 0, 0, 0, 255 };
SDL_Color blue = { 0, 0, 255, 255 };
SDL_Color green = { 0, 255, 0, 255 };
SDL_Color red = { 255, 0, 0, 255 };
SDL_Color white = { 255, 255, 255, 255 };
SDL_Color orange = { 180, 90, 20, 255 };

// Textures
LTexture gText;

// Fonts
TTF_Font *gFont12 = NULL;
TTF_Font *gFont21 = NULL;

// Sound
Mix_Chunk *sTime = NULL;
Mix_Music *sMusic = NULL;

bool loadMedia() {
	bool success = true;

	gFont12 = TTF_OpenFont("resource/PressStart2P.ttf", 12);
	gFont21 = TTF_OpenFont("resource/PressStart2P.ttf", 21);
	sTime = Mix_LoadWAV("resource/snd_time.wav");
	sMusic = Mix_LoadMUS("resource/OHC 469 Simple Mayhem.mp3");

	return success;
}

void close() {
	gText.free();
	TTF_CloseFont(gFont12);
	TTF_CloseFont(gFont21);
	Mix_FreeChunk(sTime);
	Mix_FreeMusic(sMusic);

	gFont12 = NULL;
	gFont21 = NULL;
	sTime 	= NULL;
	sMusic 	= NULL;

	SDL_DestroyRenderer(gRenderer);
	gWindow.free();

	//Quit SDL subsystems
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

// Level size
int mapX = 0;
int mapY = 0;
int mapW = 640;
int mapH = 360;

// trashBin bin variables
int trashBinX = mapX+mapW/2 - 150/2;
int trashBinY = mapY+mapH/2 - 80/2;
int trashBinW = 150;
int trashBinH = 80;

// Player variables
float x = 0+1280/2 - 32/2 - 200;
float y = 0+720/2 - 32/2;
int w = 24;
int h = 24;
bool moveL = false;
bool moveR = false;
bool moveU = false;
bool moveD = false;
float velX = 0.0, velY = 0.0;

// AI variables
float x2 = trashBinX+trashBinW + 100;
float y2 = trashBinY+trashBinH/2 - 24/2;
int w2 = 24;
int h2 = 24;

/*
 * 0: Playing Game scene
 * 2: Lost scene
 */
int gameResult = 0;
int highscore = -1;
int previousHighScore = -1;
int score = 0;

bool protection = true;
int protectionTimer = 0;
bool quit = false;

void SaveHighScore() {
	bool saveHighscore = false;

	// Open highscore first to check value
	int tempScore = -1;
	std::fstream fileOpen("data/highscore.txt");
	fileOpen >> tempScore;
	fileOpen.close();

	// If current score is higher than previously saved score, save it
	if (score > tempScore) {
		saveHighscore = true;
	}

	// Now save high score
	if (saveHighscore) {
		std::ofstream fileSave;
		fileSave.open("data/highscore.txt");
		fileSave << score;
		fileSave.close();
	}
}

void LoadHighScore() {
	std::fstream fileTileDataL("data/highscore.txt");
	fileTileDataL >> previousHighScore;
	fileTileDataL.close();
}

void ContinueGame() {
	// Win, continue playing
	if (gameResult == 0) {
		// Load highscore
		LoadHighScore();

		// Play SFX
		Mix_PlayChannel(-1, sTime, 0);

		// Add player score
		score += 10;

		// Set gamemode to play game
		gameResult = 0;

		// Random size for house
		trashBinW = rand() % 130 + 20;
		trashBinH = rand() % 40 + 40;

		// Random x pos for House
		int num1;
		int num2;
		num1 = rand() % (560 - trashBinW);
		trashBinX = 50 + num1;

		// Random y pos for House
		num2 = rand() % (270 - trashBinH);
		trashBinY = 40 + num2;

		// Reset AI position
		x2 = trashBinX + trashBinW/2;
		y2 = trashBinW + trashBinH/2;

		// Reset protection
		protection = true;
		protectionTimer = 0;
	}

	// Lose, reset game
	else if (gameResult == 1)
	{
		// Save highscore
		SaveHighScore();

		LoadHighScore();

		// Play SFX
		Mix_PlayChannel(-1, sTime, 0);

		// Set gamemode to play game
		gameResult = 0;

		// Random size for house
		trashBinW = rand() % 130 + 20;
		trashBinH = rand() % 40 + 40;

		// Random x pos for House
		int num1;
		int num2;
		num1 = rand() % (560 - trashBinW);
		trashBinX = 50 + num1;

		// Random y pos for House
		num2 = rand() % (270 - trashBinH);
		trashBinY = 40 + num2;

		// Reset AI position
		x2 = trashBinX + trashBinW/2;
		y2 = trashBinW + trashBinH/2;

		// Reset score
		score = 0;

		// Reset protection
		protection = true;
		protectionTimer = 0;
	}
}

void Update() {
	// Game state: Playing game
	if (gameResult == 0) {
		// Update Player controls
		if (moveU){
			velY -= 1;
		}else if (moveD){
			velY += 1;
		}
		if (moveL){
			velX -= 1;
		}else if (moveR){
			velX += 1;
		}

		// Max velocity
		if (velX > 6){
			velX = 6;
		}
		if (velX < -6){
			velX = -6;
		}
		if (velY > 6){
			velY = 6;
		}
		if (velY < -6){
			velY = -6;
		}

		// Player velocity
		x += velX;
		y += velY;

		// Drag coefficient
		if (!moveU && !moveD){
			velY = velY - velY * 0.5;
		}
		if (!moveL && !moveR){
			velX = velX - velX * 0.5;
		}

		// Player bounds
		if( x < 0 ){
			x = 0;
		}
		if( y < 0 ){
			y = 0;
		}
		if( x+w > mapW ){
			x = mapW-w ;
		}
		if( y+h  > mapH ){
			y = mapH-h ;
		}

		// If player protection on
		if (protection) {
			protectionTimer += 1;

			// Protect player for 1.5 seconds
			if (protectionTimer > 45) {
				protectionTimer = 0;
				protection = false;
			}
		}

		// Follow player
		{
			// Follow player
			float vX=0, vY=0;
			float bmx  = x+w/2;
			float bmy  = y+h/2;
			float distance = sqrt((bmx - x2-w2/2) * (bmx - x2-w2/2)+
								  (bmy - y2-h2/2) * (bmy - y2-h2/2));

			// Start following player
			if (distance > 10){
				// Move towards player
				vX 	= 5 * (bmx - x2-w2/2) / distance;
				vY 	= 5 * (bmy - y2-h2/2) / distance;
				x2 += vX;
				y2 += vY;
			}

			// Check if player is not protected first
			if (!protection)
			{
				// AI touched the garbage!
				if (checkCollision(x, y, w, h, x2, y2, w2, h2))
				{
					// Play SFX
					Mix_PlayChannel(-1, sTime, 0);

					// Show losing screen
					gameResult = 1;
				}

				// Event, Check if Player1 entered House (Arena)
				else if (checkCollision(x, y, w, h, trashBinX, trashBinY, trashBinW, trashBinH))
				{
					ContinueGame();
				}
			}
		}
	}
}

void Render() {
	// Render textures
	{
		// Render Map
		SDL_Rect tempMap = { mapX, mapY, mapW, mapH };
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		SDL_RenderDrawRect(gRenderer, &tempMap);

		// Render House
		SDL_Rect tempHouse = { trashBinX, trashBinY, trashBinW, trashBinH };
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		SDL_RenderDrawRect(gRenderer, &tempHouse);

		// Render protection
		if (protection) {
			SDL_Rect tempR = { x-4, y-4, w+8, h+8 };
			SDL_SetRenderDrawColor(gRenderer, 0, 200, 200, 255);
			SDL_RenderFillRect(gRenderer, &tempR);
		}
		// Render Player1
		SDL_Rect tempR = { x, y, w, h };
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		SDL_RenderFillRect(gRenderer, &tempR);

		// Render Player2
		SDL_Rect tempR2 = { x2, y2, w2, h2 };
		SDL_SetRenderDrawColor(gRenderer, 250, 30, 30, 255);
		SDL_RenderFillRect(gRenderer, &tempR2);
	}

	// Playing game scene
	if (gameResult == 0)
	{
		// Render text above house
		std::stringstream tempss;
		tempss << "Garbage Bin";
		gText.loadFromRenderedText(tempss.str().c_str(), black, gFont12);
		gText.render(trashBinX+trashBinW/2-gText.getWidth()/2, trashBinY-gText.getHeight()-2, gText.getWidth(), gText.getHeight());

		// Render text above player 1
		tempss.str(std::string());
		tempss << "Garbage";
		gText.loadFromRenderedText(tempss.str().c_str(), black, gFont12);
		gText.render(x+w/2-gText.getWidth()/2, y-gText.getHeight()-2, gText.getWidth(), gText.getHeight());

		// Render text above player AI
		tempss.str(std::string());
		tempss << "Hoarder";
		gText.loadFromRenderedText(tempss.str().c_str(), black, gFont12);
		gText.render(x2+w2/2-gText.getWidth()/2, y2+h2+2, gText.getWidth(), gText.getHeight());

		// Render score text top-right of screen
		tempss.str(std::string());
		if (count_digit(previousHighScore) == 1) {
			tempss << "Highscore: 00000" << previousHighScore;
		}
		else if (count_digit(previousHighScore) == 2) {
			tempss << "Highscore: 00000" << previousHighScore;
		}
		else if (count_digit(previousHighScore) == 3) {
			tempss << "Highscore: 0000" << previousHighScore;
		}
		else if (count_digit(previousHighScore) == 4) {
			tempss << "Highscore: 000" << previousHighScore;
		}
		else if (count_digit(previousHighScore) == 5) {
			tempss << "Highscore: 00" << previousHighScore;
		}
		else if (count_digit(previousHighScore) == 6) {
			tempss << "Highscore: 0" << previousHighScore;
		} else {
			tempss << "Highscore: 000000" << previousHighScore;
		}

		gText.loadFromRenderedText(tempss.str().c_str(), orange, gFont12);
		gText.render(gWindow.getWidth() - gText.getWidth() - 10, 4, gText.getWidth(), gText.getHeight());

		// Render score text top-right of screen
		tempss.str(std::string());
		if (count_digit(score) == 1) {
			tempss << "Score: 000000" << score;
		}
		else if (count_digit(score) == 2) {
			tempss << "Score: 00000" << score;
		}
		else if (count_digit(score) == 3) {
			tempss << "Score: 0000" << score;
		}
		else if (count_digit(score) == 4) {
			tempss << "Score: 000" << score;
		}
		else if (count_digit(score) == 5) {
			tempss << "Score: 00" << score;
		}
		else if (count_digit(score) == 6) {
			tempss << "Score: 0" << score;
		} else {
			tempss << "Score: 000000" << score;
		}
		gText.loadFromRenderedText(tempss.str().c_str(), orange, gFont12);
		gText.render(gWindow.getWidth() - gText.getWidth() - 10, 16, gText.getWidth(), gText.getHeight());
	}

	// Lost scene
	else if (gameResult == 1) {
		std::stringstream tempss;
		tempss << "You lose. Boo hoo.";
		gText.loadFromRenderedText(tempss.str().c_str(), black, gFont12);
		gText.render(gWindow.getWidth()/2 - gText.getWidth()/2,
				 gWindow.getHeight() * 0.90 - gText.getHeight()-22,
					 gText.getWidth(), gText.getHeight());

		tempss.str(std::string());
		tempss << "Press Space to Start.";
		gText.loadFromRenderedText(tempss.str().c_str(), black, gFont12);
		gText.render(gWindow.getWidth()/2 - gText.getWidth()/2,
				 gWindow.getHeight() * 0.90 - gText.getHeight(),
					 gText.getWidth(), gText.getHeight());
	}

}
