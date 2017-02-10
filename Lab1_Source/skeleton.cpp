#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModel.h"
#include "functions.cpp"


using namespace std;
using glm::vec3;
using glm::mat3;

/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
SDL_Surface* screen;
int t;

/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

void Update();
void Draw();

int main( int argc, char* argv[] )
{
	screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
	t = SDL_GetTicks();	// Set start value for timer.
	
	// Create stars at random start positions
	vector<vec3> stars( 1000 );
	Create_Random_Starfield(stars);

	while( NoQuitMessageSDL() )
	{
		// Update();
		// Draw();
		// Draw_bilinear();
		// Draw_Starfield();
		Update_motion(stars);
		Draw_Starfield(stars);
	}

	SDL_SaveBMP( screen, "screenshot.bmp" );

	return 0;
}

void Update()
{
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2-t);
	t = t2;
	cout << "Render time: " << dt << " ms." << endl;
}

void Update_motion(vector<vec3>& stars)
{
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2-t);
	t = t2;
	cout << "Render time: " << dt << " ms." << endl;

	float v = 0.002;
	for( int s=0; s<stars.size(); ++s ){
    // Add code for update of stars
		stars[s].z = stars[s].z + v*dt;

		if( stars[s].z <= 0 )
			stars[s].z += 1;
	    if( stars[s].z > 1 )
			stars[s].z -= 1;
	}
}

void Draw()
{
	if( SDL_MUSTLOCK(screen) )
		SDL_LockSurface(screen);

	vec3 color( 255.0, 255.0, 255.0 );
	fill_screen(color);

	if( SDL_MUSTLOCK(screen) )
		SDL_UnlockSurface(screen);

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}

void Draw_bilinear(){
	if( SDL_MUSTLOCK(screen) )
		SDL_LockSurface(screen);

	vec3 topLeft(1,0,0);     // red
	vec3 topRight(0,0,1);    // blue
	vec3 bottomRight(0,1,0); // green
	vec3 bottomLeft(1,1,0);  // yellow
	vector<vec3> leftSide( SCREEN_HEIGHT );
	vector<vec3> rightSide( SCREEN_HEIGHT );
	Interpolate( topLeft, bottomLeft, leftSide );
	Interpolate( topRight, bottomRight, rightSide );

	for( int y=0; y<SCREEN_HEIGHT; ++y )
	{
		vector<vec3> left_right_intrp( SCREEN_WIDTH );
		Interpolate( leftSide[y], rightSide[y], left_right_intrp );
		
		for( int x=0; x<SCREEN_WIDTH; ++x )
		{
			PutPixelSDL( screen, x, y, left_right_intrp[x]);
		}
	}

	if( SDL_MUSTLOCK(screen) )
		SDL_UnlockSurface(screen);

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}

void Draw_Starfield(){
	// Initialise to random values
	vector<vec3> stars( 1000 );
	Create_Random_Starfield(stars);
	Draw_Starfield(stars);
}

void Draw_Starfield(vector<vec3>& stars){	
	// Fill screen with black
	SDL_FillRect( screen, 0, 0 );

	if( SDL_MUSTLOCK(screen) )
		SDL_LockSurface(screen);
	
	// code for projecting and drawing each star
	for( size_t s=0; s<stars.size(); ++s )
    {	
		vec3 color_white( 255.0, 255.0, 255.0 );
		float f = SCREEN_HEIGHT / 2;
		float u = f * (stars[s].x / stars[s].z) + (SCREEN_WIDTH / 2);
		float v = f * (stars[s].y / stars[s].z) + (SCREEN_HEIGHT / 2);

		//Fade brightness
		vec3 color = 0.2f * vec3(1,1,1) / (stars[s].z * stars[s].z);
    	
    	PutPixelSDL( screen, u, v, color);
	}

	if( SDL_MUSTLOCK(screen) )
		SDL_UnlockSurface(screen);

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}

void Create_Random_Starfield(vector<vec3>& stars){
	for (int i = 0; i < stars.size(); i++){
		stars[i].x = PlusOrMinusOne() * float(rand()) / float(RAND_MAX);
		stars[i].y = PlusOrMinusOne() * float(rand()) / float(RAND_MAX);
		stars[i].z = float(rand()) / float(RAND_MAX);
	}
}

void fill_screen(vec3 color){
	for( int y=0; y<SCREEN_HEIGHT; ++y )
	{
		for( int x=0; x<SCREEN_WIDTH; ++x )
		{
			PutPixelSDL( screen, x, y, color );
		}
	}
}

int PlusOrMinusOne() {
    return (rand() % 2) * 2 - 1;
}







