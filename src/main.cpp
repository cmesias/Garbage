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
#include <SDL2/SDL_mouse.h>

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>

using namespace std;

//LTexture, Window, and Initialize stuff
#include "init.h"

int main(int argc, char **argv) {

	init();
	loadMedia();

	bool quit = false;

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

	//Mouse
	int mx, my;

	// Player1
	float x = 0+1280/2 - 32/2 - 200;
	float y = 0+720/2 - 32/2;
	int w = 24;
	int h = 24;
	bool moving = false;
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
	bool movePlayer2ToStart = false;
	bool moveL2 = false;
	bool moveR2 = false;
	bool moveU2 = false;
	bool moveD2 = false;

	int mouseWheelY = 0;

	float camx = 0, camy = 0;
	// Random seconds to escape from 5 - 15 seconds
	int escapeTimer = (rand() % 10 + 5) * 60;

	/*
	 * 0: playing game
	 * 1: winner
	 * 2: loser
	 */
	int gameResult = 0;

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

		//Get mouse x, y positions
		SDL_GetMouseState(&mx, &my);


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
					// If we are not playing, lets play
					if (gameResult != 0) {

						// Set gamemode to play game
						gameResult = 0;

						// New timer to escape
						escapeTimer = (rand() % 10 + 5) * 60;

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
						x2 = mapX + mapW - 100;
						y2 = mapY + mapH - h2/2;
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
			if (e.type == SDL_MOUSEWHEEL) {
					mouseWheelY = e.wheel.y;
			}
			if (e.type == SDL_MOUSEBUTTONDOWN) {
				if (e.button.button == SDL_BUTTON_LEFT) {
					//
				}
				if (e.button.button == SDL_BUTTON_RIGHT) {
					//
				}
			}
			if (e.type == SDL_MOUSEBUTTONUP) {
				if (e.button.button == SDL_BUTTON_LEFT) {
					//
				}
			}
		} // end controls

		// Game state: Playing game
		if (gameResult == 0) {

			// Update Player controls
			if (moveU){
				velY -= 1;
				moving = true;
			}else if (moveD){
				velY += 1;
				moving = true;
			}
			if (moveL){
				velX -= 1;
				moving = true;
			}else if (moveR){
				velX += 1;
				moving = true;
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

			// Player stopped moving, start Idle animation
			if (!moveU && !moveD && !moveL && !moveR){
				moving = false;
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

			// Event, Check if Player1 entered House (Arena)
			if (checkCollision(x, y, w, h, houseX, houseY, houseW, houseH))
			{
				// Start following player
				movePlayer2ToStart = true;

				for (int i=0; i<15; i++) {
					if (escapeTimer == i*60) {
						Mix_PlayChannel(-1, sTime, 0);
					}
				}

				// start escape timer
				escapeTimer -= 1;

				// Show winning screen
				if (escapeTimer/60 < 1) {
					gameResult = 1;
				}
			}

			// Start following player
			if (movePlayer2ToStart){
				float bmx, bmy;
				//bmx  = houseX+houseW-w2-35;
				//bmy  = houseY+h2+125;
				// Follow player
				bmx  = x+w/2;
				bmy  = y+h/2;
				float distance = sqrt((bmx - x2-w2/2) * (bmx - x2-w2/2)+
									  (bmy - y2-h2/2) * (bmy - y2-h2/2));

				// Player2
				float vX, vY;
				if (distance > 0.5){
					vX 	= 2 * (10*distance/600) * (bmx - x2-w2/2) / distance;
					vY 	= 2 * (10*distance/600) * (bmy - y2-h2/2) / distance;
				}
				if (distance < 24){
					moveL = false;
					moveR = false;
					moveU = false;
					moveD = false;
					moving = false;
					// Play SFX
					Mix_PlayChannel(-1, sTime, 0);

					// Show losing screen
					gameResult = 2;
				}
				x2 += vX;
				y2 += vY;
			}
		}

		//Clear render screen
		SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
		SDL_RenderClear(gRenderer);

			// Congrats screen
			if (gameResult == 0) {
				// Render Kissing booth text
				std::stringstream tempss;
				tempss << "Trash Can";
				gText.loadFromRenderedText(tempss.str().c_str(), black, gFont42);
				gText.render(houseX+houseW/2-gText.getWidth()/2-camx, houseY-gText.getHeight()-5-20-camy, gText.getWidth(), gText.getHeight());

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
				gText.loadFromRenderedText(tempss.str().c_str(), black, gFont42);
				gText.render(houseX+houseW/2-gText.getWidth()/2-camx, houseY-gText.getHeight()-5-camy, gText.getWidth(), gText.getHeight());

				// Render player1 prompt
				tempss.str(std::string());
				tempss << "Trash";
				gText.loadFromRenderedText(tempss.str().c_str(), black, gFont42);
				gText.render(x+w/2-gText.getWidth()/2-camx, y-gText.getHeight()-2-camy, gText.getWidth(), gText.getHeight());

				// Render player2 prompt
				tempss.str(std::string());
				tempss << "Im Hoarder!";
				gText.loadFromRenderedText(tempss.str().c_str(), black, gFont42);
				gText.render(x2+w2/2-gText.getWidth()/2-camx, y2-gText.getHeight()-2-camy, gText.getWidth(), gText.getHeight());
			}

			// Congrats screen
			else if (gameResult == 1) {
				std::stringstream tempss;
				tempss << "You've escaped! Good job :)";
				gText.loadFromRenderedText(tempss.str().c_str(), black, gFont42);
				gText.render(gWindow.getWidth()/2 - gText.getWidth()/2,
							 gWindow.getHeight() * 0.90 - gText.getHeight() - 22,
							 gText.getWidth(), gText.getHeight());

				tempss.str(std::string());
				tempss << "Press Space to Start.";
				gText.loadFromRenderedText(tempss.str().c_str(), black, gFont42);
				gText.render(gWindow.getWidth()/2 - gText.getWidth()/2,
						 	 gWindow.getHeight() * 0.90 - gText.getHeight(),
							 gText.getWidth(), gText.getHeight());
			}

			// Lost screen
			else if (gameResult == 2) {
				std::stringstream tempss;
				tempss << "You lose. Boo hoo.";
				gText.loadFromRenderedText(tempss.str().c_str(), black, gFont42);
				gText.render(gWindow.getWidth()/2 - gText.getWidth()/2,
						 gWindow.getHeight() * 0.90 - gText.getHeight()-22,
							 gText.getWidth(), gText.getHeight());

				tempss.str(std::string());
				tempss << "Press Space to Start.";
				gText.loadFromRenderedText(tempss.str().c_str(), black, gFont42);
				gText.render(gWindow.getWidth()/2 - gText.getWidth()/2,
					 	 gWindow.getHeight() * 0.90 - gText.getHeight(),
							 gText.getWidth(), gText.getHeight());
			}

			// Render Player1
			SDL_Rect tempR = { x-camx, y-camy, w, h };
			SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
			SDL_RenderFillRect(gRenderer, &tempR);

			// Render Player2
			SDL_Rect tempR2 = { x2-camx, y2-camy, w2, h2 };
			SDL_SetRenderDrawColor(gRenderer, 250, 30, 30, 255);
			SDL_RenderFillRect(gRenderer, &tempR2);

			// Render House
			SDL_Rect tempHouse = { houseX-camx, houseY-camy, houseW, houseH };
			SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
			SDL_RenderDrawRect(gRenderer, &tempHouse);

			// Render Map
			SDL_Rect tempMap = { mapX-camx, mapY-camy, mapW, mapH };
			SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
			SDL_RenderDrawRect(gRenderer, &tempMap);


		SDL_RenderPresent(gRenderer);
	}
	close();			//Free resources and close SDL
	return 0;
}
