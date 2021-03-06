// NinjaRun.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "NinjaRun.h"
#include <iostream>
#include <cstdlib>
#include <hge.h>
#include <hgesprite.h>
#include <hgefont.h>
#include <hgeparticle.h>
#include <vector>

using namespace std;

// Pointer to the HGE interface.
// Helper classes require this to work.
HGE *hge = 0;

// Pointers to the HGE objects we will use
hgeFont*			fnt;

typedef struct {
	hgeSprite* spr;
	float x;
	float y;
}sprite;

typedef struct {
	hgeSprite* spr;
	int x;
	int y;
}ground;

typedef struct {
	hgeSprite* spr;
	int x;
	int y;
}brick;

// Create Jack
sprite jackSpr;

vector<ground> groundStructs;
vector<brick> brickStructs;

// Handles for HGE resourcces
HTEXTURE			jackTex;
HTEXTURE			brickTex;
HTEXTURE			groundTex;
HEFFECT				snd;

// Some "gameplay" variables
float x = 0, y = 490;
float dx = 0.0f, dy = 0.0f;
float score = 0;
float reward = 0.01;

const float speed = 15;
const float fallSpeed = 200;
const float friction = 0.98f;
const int GROUND_NR = 10;
const int GROUND_WIDTH = 125;
const int JACK_HEIGHT = 105;
const int JACK_WIDTH = 70;
const int BRICK_WIDTH = 130;
const int BRICK_HEIGHT = 85;
int JUMP_LIMIT = 510;
const int JUMP_POWER = 150;
int GAME_SPEED = 3;

bool mustDie = false;
bool mustJump = false;
bool endGame = false;

int jumpsNr = 0;

// Play sound effect
void boom() {
	int pan = int((x - 400) / 4);
	float pitch = (dx*dx + dy * dy)*0.0005f + 0.2f;
	hge->Effect_PlayEx(snd, 100, pan, pitch);
}

bool FrameFunc()
{
	score += reward;
	float dt = hge->Timer_GetDelta();

	if (endGame == true)
	{
		return true;
	}
	// Do some movement calculations and collision detection
	if (!mustDie) 
	{
		// Process keys
		if (hge->Input_GetKeyState(HGEK_ESCAPE)) return true;
		if (mustJump) 
		{
			if (y < JUMP_LIMIT) 
			{
				// get down
				mustJump = false;
			}
			else 
			{
				// go higher
				dy -= speed * dt;
				dy *= friction;
				y += dy;
			}
		}
		else 
		{
			if (jumpsNr < 2 && hge->Input_GetKeyState(HGEK_UP)) 
			{
				jumpsNr++;
				mustJump = true;
				JUMP_LIMIT = y - JUMP_POWER;
			}
			else 
			{
				bool groundUnder = false;
				for (int i = 0; i < groundStructs.size(); i++) 
				{
					// i'm on a ground block
					if (groundStructs[i].x <= x) 
					{
						groundUnder = true;
						break;
					}
				}
				if (y + JACK_HEIGHT < 595) 
				{
					y += fallSpeed * dt;
					// it means he's touching the ground
					if (y > 490) 
					{
y = 490;
jumpsNr = 0;
					}
				}
				else if (!groundUnder)
				{
					y += fallSpeed * dt;
				}
				//for (int i = 0; i < brickStructs.size(); i++)
				//{

				//}
				if (y + JACK_HEIGHT > 595)
				{
					mustDie = true;
					GAME_SPEED = 0;
				}
			}
		}
	}
	else
	{
		y += fallSpeed * dt;
	}

	if (y > 600)
	{
		return true;
	}

	return false;
}

ground generate_ground(int x)
{
	ground gr;
	gr.spr = new hgeSprite(groundTex, 0, 0, GROUND_WIDTH, 128);
	gr.x = x;
	gr.y = 595;
	return gr;
}

brick generate_brick(int x, int y)
{
	brick br;
	br.spr = new hgeSprite(brickTex, 0, 0, BRICK_WIDTH, BRICK_HEIGHT);
	br.x = x;
	br.y = y;
	return br;
}

bool RenderFunc()
{
	// Render graphics
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(0);
	jackSpr.spr->Render(x, y);

	vector<int> to_delete;
	for (int i = 0; i < groundStructs.size(); i++)
	{
		groundStructs[i].spr->Render(groundStructs[i].x, groundStructs[i].y);
		groundStructs[i].x -= GAME_SPEED;
		if (groundStructs[i].x < -GROUND_WIDTH)
		{
			to_delete.push_back(i);
		}
	}
	for (int i = 0; i < to_delete.size(); i++)
	{
		groundStructs.erase(groundStructs.begin() + to_delete.at(i));
	}

	int rnd = (int)rand() % 60;
	if (rnd == 1)
	{
		groundStructs.push_back(generate_ground(1300));
	}

	to_delete.clear();
	for (int i = 0; i < brickStructs.size(); i++)
	{
		brickStructs[i].spr->Render(brickStructs[i].x, brickStructs[i].y);
		brickStructs[i].x -= GAME_SPEED;
		if (brickStructs[i].x < -BRICK_WIDTH)
		{
			to_delete.push_back(i);
		}
		// Collision detection between Jack and a Brick
		// x and y are Jack's coordinates
		if ((x >= brickStructs[i].x && x <= (brickStructs[i].x + BRICK_WIDTH)) ||
			((x + JACK_WIDTH) >= brickStructs[i].x && (x + JACK_WIDTH) <= (brickStructs[i].x + BRICK_WIDTH)) )
		{
			if ((y >= brickStructs[i].y && y <= (brickStructs[i].y + BRICK_HEIGHT)) ||
				((y + JACK_HEIGHT) >= brickStructs[i].y && (y + JACK_HEIGHT) <= (brickStructs[i].y + BRICK_HEIGHT)))
			{
				// there's a collision with a brick
				endGame = true;
			}
			else
			{
				// there's no collision with a brick
			}
		}
	}
	for (int i = 0; i < to_delete.size(); i++)
	{
		brickStructs.erase(brickStructs.begin() + to_delete.at(i));
	}

	rnd = (int)rand() % 400;
	if (rnd == 1) 
	{
		int brick_y = rand() % (510 - 300 + 1) + 300;
		brickStructs.push_back(generate_brick(1300, brick_y));
	}
	fnt->printf(5, 5, HGETEXT_LEFT, "score: %.2f", score);
	// fnt->printf(5, 5, HGETEXT_LEFT, "dt:%.3f\nFPS:%d (constant)\nx:%.1f\ny:%.1f\ndx:%.1f\ndy:%.1f", hge->Timer_GetDelta(), hge->Timer_GetFPS(), x, y, dx, dy);
	hge->Gfx_EndScene();

	return false;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	hge = hgeCreate(HGE_VERSION);

	hge->System_SetState(HGE_LOGFILE, "NinjaRun.log");
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_TITLE, "Ninja Run - You can never escape what's comming");
	hge->System_SetState(HGE_FPS, 100);
	hge->System_SetState(HGE_WINDOWED, true);
	hge->System_SetState(HGE_SCREENWIDTH, 1280);
	hge->System_SetState(HGE_SCREENHEIGHT, 720);
	hge->System_SetState(HGE_SCREENBPP, 32);

	if (hge->System_Initiate()) 
	{
		// Load sound and texture
		snd = hge->Effect_Load("menu.wav");
		jackTex = hge->Texture_Load("character_1.png");
		brickTex = hge->Texture_Load("brick.png");
		groundTex = hge->Texture_Load("ground.png");

		if (!snd || !jackTex || !groundTex)
		{
			// If one of the data files is not found, display
			// an error message and shutdown.
			// MessageBox(NULL, "Can't load one of the following files:\nMENU.WAV, PARTICLES.PNG, FONT1.FNT, FONT1.PNG, TRAIL.PSI", "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
			hge->System_Shutdown();
			hge->Release();
			return 0;
		}

		// Create and set up Jack's sprite
		jackSpr.spr = new hgeSprite(jackTex, 0, 0, JACK_WIDTH, JACK_HEIGHT);

		// Create and set up the brick sprites
		for (int i = 0; i < GROUND_NR; i++) 
		{
			ground gr = generate_ground(i * GROUND_WIDTH);
			groundStructs.push_back(gr);
		}

		// Load a font
		fnt = new hgeFont("font1.fnt");

		// Let's rock now!
		hge->System_Start();

		// Delete created objects and free loaded resources
		delete fnt;
		delete jackSpr.spr;

		hge->Texture_Free(jackTex);
		hge->Texture_Free(groundTex);
		hge->Effect_Free(snd);
	}

	// Clean up and shutdown
	hge->System_Shutdown();
	hge->Release();
	return 0;
}
