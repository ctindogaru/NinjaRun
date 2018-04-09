// NinjaRun.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "NinjaRun.h"
#include <hge.h>
#include <hgesprite.h>
#include <hgefont.h>
#include <hgeparticle.h>

// Pointer to the HGE interface.
// Helper classes require this to work.
HGE *hge = 0;

// GUI Screen Constants


// Pointers to the HGE objects we will use
hgeSprite*			jackSpr;
hgeSprite*			brickSpr;
hgeSprite*			groundSpr;
hgeSprite*			particleSpr;
hgeFont*			fnt;
hgeParticleSystem*	par;

// Handles for HGE resourcces
HTEXTURE			jackTex;
HTEXTURE			brickTex;
HTEXTURE			groundTex;
HEFFECT				snd;

// Some "gameplay" variables
float x = 0.0f, y = 470.0f;
float dx = 0.0f, dy = 0.0f;

const float speed = 10;
const float friction = 0.98f;

// Play sound effect
void boom() {
	int pan = int((x - 400) / 4);
	float pitch = (dx*dx + dy * dy)*0.0005f + 0.2f;
	hge->Effect_PlayEx(snd, 100, pan, pitch);
}

bool FrameFunc()
{
	float dt = hge->Timer_GetDelta();

	// Process keys
	if (hge->Input_GetKeyState(HGEK_ESCAPE)) return true;
	if (hge->Input_GetKeyState(HGEK_LEFT)) dx -= speed * dt;
	if (hge->Input_GetKeyState(HGEK_RIGHT)) dx += speed * dt;
	if (hge->Input_GetKeyState(HGEK_UP)) dy -= speed * dt;
	if (hge->Input_GetKeyState(HGEK_DOWN)) dy += speed * dt;

	// Do some movement calculations and collision detection	
	dx *= friction; 
	dy *= friction; 
	x += dx; 
	y += dy;
	if (x>1170) 
	{ 
		x = 1170 - (x - 1170); 
		//dx = -dx; 
		//boom(); 
	}
	if (x<0) 
	{ 
		x = 0 + 0 - x; 
		//dx = -dx; 
		//boom(); 
	}
	if (y>470) 
	{ 
		y = 470 - (y - 470); 
		//dy = -dy; 
		//boom(); 
	}
	if (y<280) 
	{ 
		y = 280 + 280 - y; 
		//dy = -dy; 
		//boom(); 
	}

	// Update particle system
	par->info.nEmission = (int)(dx*dx + dy * dy) * 2;
	par->MoveTo(x, y);
	par->Update(dt);

	return false;
}


bool RenderFunc()
{
	// Render graphics
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(0);
	par->Render();
	jackSpr->Render(x, y);
	groundSpr->Render(0, 590);
	fnt->printf(5, 5, HGETEXT_LEFT, "dt:%.3f\nFPS:%d (constant)\nx:%.1f\ny:%.1f\ndx:%.1f\ndy:%.1f", hge->Timer_GetDelta(), hge->Timer_GetFPS(), x, y, dx, dy);
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

	if (hge->System_Initiate()) {

		// Load sound and texture
		snd = hge->Effect_Load("menu.wav");
		jackTex = hge->Texture_Load("character.png");
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
		jackSpr = new hgeSprite(jackTex, 0, 0, 108, 140);

		// Create and set up the brick sprite
		groundSpr = new hgeSprite(groundTex, 0, 0, 128, 128);

		// Load a font
		fnt = new hgeFont("font1.fnt");

		// Create and set up a particle system
		particleSpr = new hgeSprite(jackTex, 0, 0, 648, 120);
		particleSpr->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHAADD | BLEND_NOZWRITE);
		particleSpr->SetHotSpot(16, 16);
		par = new hgeParticleSystem("trail.psi", particleSpr);
		par->Fire();

		// Let's rock now!
		hge->System_Start();

		// Delete created objects and free loaded resources
		delete par;
		delete fnt;
		delete particleSpr;
		delete jackSpr;
		delete brickSpr;
		delete groundSpr;

		hge->Texture_Free(jackTex);
		hge->Texture_Free(groundTex);
		hge->Effect_Free(snd);
	}

	// Clean up and shutdown
	hge->System_Shutdown();
	hge->Release();
	return 0;
}
