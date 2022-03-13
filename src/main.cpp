/*
 *
 *
 *
 * Author: Cmesias
 *
 * Date: 3/12/2022
 *
 * Game Name: Idk tbh
 *
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>

using namespace std;

int count_digit(int number) {
   return int(log10(number) + 1);
}

//LTexture, Window, and Initialize stuff
#include "init.h"


// Level size
int mapX = 0;
int mapY = 0;
int mapW = 640;
int mapH = 360;

// House
int houseX = mapX+mapW/2 - 150/2;
int houseY = mapY+mapH/2 - 80/2;
int houseW = 150;
int houseH = 80;

// Player1
float x = 0+1280/2 - 32/2 - 200;
float y = 0+720/2 - 32/2;
int w = 24;
int h = 24;
bool moveL = false;
bool moveR = false;
bool moveU = false;
bool moveD = false;
float velX = 0.0, velY = 0.0;

//Player 2
float x2 = houseX+houseW + 100;
float y2 = houseY+houseH/2 - 24/2;
int w2 = 24;
int h2 = 24;

/*
 * 0: playing game
 * 1: winner
 * 2: loser
 */
int gameResult = 0;
int highscore = -1;
int previousHighScore = -1;
int score = 0;
int escapeTimer = (rand() % 5 + 5) * 60;		// Random seconds to escape from 5 - 15 seconds

bool protection = true;
int protectionTimer = 0;
bool quit = false;
float camx = 0, camy = 0;

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

		// New timer to escape
		escapeTimer = (rand() % 5 + 5) * 60;

		// Random size for house
		houseW = rand() % 130 + 20;
		houseH = rand() % 40 + 40;

		// Random x pos for House
		int num1;
		int num2;
		num1 = rand() % (560 - houseW);
		houseX = 50 + num1;

		// Random y pos for House
		num2 = rand() % (270 - houseH);
		houseY = 40 + num2;

		// Reset AI position
		x2 = houseX + houseW/2;
		y2 = houseW + houseH/2;

		// Reset protection
		protection = true;
		protectionTimer = 0;
	}

	// Lose, reset game
	else if (gameResult == 2)
	{
		// Save highscore
		SaveHighScore();

		LoadHighScore();

		// Play SFX
		Mix_PlayChannel(-1, sTime, 0);

		// Set gamemode to play game
		gameResult = 0;

		// New timer to escape
		escapeTimer = (rand() % 5 + 5) * 60;

		// Random size for house
		houseW = rand() % 130 + 20;
		houseH = rand() % 40 + 40;

		// Random x pos for House
		int num1;
		int num2;
		num1 = rand() % (560 - houseW);
		houseX = 50 + num1;

		// Random y pos for House
		num2 = rand() % (270 - houseH);
		houseY = 40 + num2;

		// Reset AI position
		x2 = houseX + houseW/2;
		y2 = houseW + houseH/2;

		// Reset score
		score = 0;

		// Reset protection
		protection = true;
		protectionTimer = 0;
	}
}

int main(int argc, char **argv) {

	init();
	loadMedia();

	LoadHighScore();

	// Load music
	Mix_FadeInMusic(sMusic, -1, 0);

	while (!quit) {

		// Camera target distance
		float bmx, bmy;
		bmx  = x+w/2-gWindow.getWidth()/2;
		bmy  = y+h/2-gWindow.getHeight()/2;
		float distance = sqrt((bmx - camx) * (bmx - camx)+
							  (bmy - camy) * (bmy - camy));

		// Camera target
		float vX, vY;
		if (distance > 0.5){
			vX 	= 2 * (10*distance/600) * (bmx - camx) / distance;
			vY 	= 2 * (10*distance/600) * (bmy - camy) / distance;
		}
		camx += vX;
		camy += vY;

		// Camera bounds
		if( camx < 0 ){
			camx = 0;
		}
		if( camy < 0 ){
			camy = 0;
		}
		if( camx+gWindow.getWidth() > mapW ){
			camx = mapW-gWindow.getWidth() ;
		}
		if( camy+gWindow.getHeight()  > mapH ){
			camy = mapH-gWindow.getHeight() ;
		}


		SDL_Event e;
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}

			//Handle window events
			gWindow.handleEvent(e);

			//If Key Pressed
			if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
				switch (e.key.keysym.sym) {
				case SDLK_w:
					moveU = true;
					break;
				case SDLK_s:
					moveD = true;
					break;
				case SDLK_a:
					moveL = true;
					break;
				case SDLK_d:
					moveR = true;
					break;
				case SDLK_SPACE:
					if (gameResult == 2) {
						ContinueGame();
					}
					break;
				}
			}
			//If Key Released
			else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
				switch (e.key.keysym.sym) {
				case SDLK_w:
					moveU = false;
					break;
				case SDLK_s:
					moveD = false;
					break;
				case SDLK_a:
					moveL = false;
					break;
				case SDLK_d:
					moveR = false;
					break;
				}
			}
		} // end controls

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
				bmx  = x+w/2;
				bmy  = y+h/2;
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
						gameResult = 2;
					}

					// Event, Check if Player1 entered House (Arena)
					else if (checkCollision(x, y, w, h, houseX, houseY, houseW, houseH))
					{
						ContinueGame();

						/*// Start following player
						movePlayer2ToStart = true;

						for (int i=0; i<15; i++) {
							if (escapeTimer == i*30) {
								// Add score
								score += 2;
							}
							if (escapeTimer == i*60) {
								// Play SFX
								Mix_PlayChannel(-1, sTime, 0);
							}
						}

						// start escape timer
						escapeTimer -= 1;

						// Show winning screen
						if (escapeTimer/60 < 1) {
							//gameResult = 1;

							// Add player score
							score += 100;

							ContinueGame();
						}*/
					}
				}




			}
		}

		//Clear render screen
		SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
		SDL_RenderClear(gRenderer);

			// Render textures
			{
				// Render Map
				SDL_Rect tempMap = { mapX-camx, mapY-camy, mapW, mapH };
				SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
				SDL_RenderDrawRect(gRenderer, &tempMap);

				// Render House
				SDL_Rect tempHouse = { houseX-camx, houseY-camy, houseW, houseH };
				SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
				SDL_RenderDrawRect(gRenderer, &tempHouse);

				// Render protection
				if (protection) {
					SDL_Rect tempR = { x-4-camx, y-4-camy, w+8, h+8 };
					SDL_SetRenderDrawColor(gRenderer, 0, 200, 200, 255);
					SDL_RenderFillRect(gRenderer, &tempR);
				}
				// Render Player1
				SDL_Rect tempR = { x-camx, y-camy, w, h };
				SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
				SDL_RenderFillRect(gRenderer, &tempR);

				// Render Player2
				SDL_Rect tempR2 = { x2-camx, y2-camy, w2, h2 };
				SDL_SetRenderDrawColor(gRenderer, 250, 30, 30, 255);
				SDL_RenderFillRect(gRenderer, &tempR2);
			}

			// Congrats screen
			if (gameResult == 0) {
				// Render Kissing booth text
				std::stringstream tempss;
				tempss << "Garbage Bin";
				gText.loadFromRenderedText(tempss.str().c_str(), black, gFont12);
				gText.render(houseX+houseW/2-gText.getWidth()/2-camx, houseY-gText.getHeight()-2-camy, gText.getWidth(), gText.getHeight());

				// Render timer text
				tempss.str(std::string());
				double roundedNum = escapeTimer/60;
				roundedNum = roundMe(roundedNum);
				if (roundedNum >= 10) {
					tempss << "00:" << roundedNum << "s";
				}

				else if (roundedNum < 10) {
					tempss << "00:" << "0" << roundedNum << "s";
				}
				gText.loadFromRenderedText(tempss.str().c_str(), black, gFont12);
				//gText.render(houseX+houseW/2-gText.getWidth()/2-camx, houseY-gText.getHeight()-5-camy, gText.getWidth(), gText.getHeight());

				// Render text above player 1
				tempss.str(std::string());
				tempss << "Garbage";
				gText.loadFromRenderedText(tempss.str().c_str(), black, gFont12);
				gText.render(x+w/2-gText.getWidth()/2-camx, y-gText.getHeight()-2-camy, gText.getWidth(), gText.getHeight());

				// Render text above player AI
				tempss.str(std::string());
				tempss << "Hoarder";
				gText.loadFromRenderedText(tempss.str().c_str(), black, gFont12);
				gText.render(x2+w2/2-gText.getWidth()/2-camx, y2+h2+2-camy, gText.getWidth(), gText.getHeight());

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

			// Congrats screen
			else if (gameResult == 1) {
				std::stringstream tempss;
				tempss << "You've escaped! Good job :)";
				gText.loadFromRenderedText(tempss.str().c_str(), black, gFont12);
				gText.render(gWindow.getWidth()/2 - gText.getWidth()/2,
							 gWindow.getHeight() * 0.90 - gText.getHeight() - 22,
							 gText.getWidth(), gText.getHeight());

				tempss.str(std::string());
				tempss << "Press Space to Start.";
				gText.loadFromRenderedText(tempss.str().c_str(), black, gFont12);
				gText.render(gWindow.getWidth()/2 - gText.getWidth()/2,
						 	 gWindow.getHeight() * 0.90 - gText.getHeight(),
							 gText.getWidth(), gText.getHeight());
			}

			// Lost screen
			else if (gameResult == 2) {
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


		SDL_RenderPresent(gRenderer);
	}
	close();			//Free resources and close SDL
	return 0;
}
