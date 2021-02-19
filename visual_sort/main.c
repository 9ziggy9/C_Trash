// Suppress usleep() usage warning
#define _XOPEN_SOURCE 500

// SDL2 libraries
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
// Standard libraries
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
// MACROS: remember to replace with getopts
#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 640
#define NUM 96

bool running;

SDL_Renderer* rend;
SDL_Window* win;

int frameCount, lastTime, timerFPS, lastFrame, fps;

void input() 
{
	SDL_Event e;
	while(SDL_PollEvent(&e))
		if(e.type == SDL_QUIT)
			running = false;
	const Uint8 *keystates = SDL_GetKeyboardState(NULL);
	if(keystates[SDL_SCANCODE_ESCAPE])
		running = false;
}

void render_backsplash()
{
	// backsplash of window, gruxbox gray
	SDL_SetRenderDrawColor(rend, 40, 40, 40, 255);
	SDL_Rect rect;
	rect.x = rect.y = 0;
	rect.w = WINDOW_WIDTH;
	rect.h = WINDOW_HEIGHT;
	SDL_RenderFillRect(rend, &rect);
}

void populate_segment_height(int *array)
{
	int step = WINDOW_HEIGHT / NUM;

	for (int n=0; n<NUM; n++)
		array[n] = step * (n+1);
}

void swap(int *a, int *b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

void unhighlight_segments(bool *hl)
{
	for(int j=0; j<NUM; j++)
		hl[j] = false;
}

void shuffle_segments(int *array, int n)
{
	// Fisher-Yates shuffle implementation
	for (int i = n-1; i>0; i--)
	{
		int j = rand()%(i+1);
		swap(&array[i], &array[j]);
	}
}

void draw_segments(int *array, bool *hl)
{
	int SEGMENT_WIDTH = WINDOW_WIDTH / NUM;

	// foreground, segment struct, gruvbox
	SDL_Rect segment;
	SDL_SetRenderDrawColor(rend, 251, 241, 199, 255);
	segment.y = WINDOW_HEIGHT;
	segment.w = SEGMENT_WIDTH;

	for (int n=0; n<NUM; n++) 
	{
		segment.x = n * SEGMENT_WIDTH;
		segment.h = - array[n]; // NOTE: direction of increasing y reversed in SDL
		if (hl[n]) {
			SDL_SetRenderDrawColor(rend, 251, 73, 52, 255);
			SDL_RenderFillRect(rend, &segment); // draw
		} else {
				SDL_SetRenderDrawColor(rend, 251, 241, 199, 255);
			  SDL_RenderFillRect(rend, &segment); // draw
			}
	}
}

void draw_initially(int *array)
{
	int SEGMENT_WIDTH = WINDOW_WIDTH / NUM;
	
	render_backsplash();
	
	// foreground, segment struct, gruvbox
	SDL_Rect segment;
	SDL_SetRenderDrawColor(rend, 251, 241, 199, 255);
	segment.y = WINDOW_HEIGHT;
	segment.w = SEGMENT_WIDTH;

	for (int n=0; n<NUM; n++) 
	{
		segment.x = n * SEGMENT_WIDTH;
		segment.h = - array[n]; // NOTE: direction of increasing y reversed in SDL
		SDL_RenderFillRect(rend, &segment); // draw
		SDL_RenderPresent(rend);

		// added delay for step effect, usleep is actually DEPRECATED now,
		// please return and replace with nanosleep()!
		usleep(15000);
	}
}

void draw_finally(int *array)
{
	int SEGMENT_WIDTH = WINDOW_WIDTH / NUM;
	
	render_backsplash();
	
	// foreground, segment struct, gruvbox
	SDL_Rect segment;
	SDL_SetRenderDrawColor(rend, 102, 255, 0, 255);
	segment.y = WINDOW_HEIGHT;
	segment.w = SEGMENT_WIDTH;

	for (int n=0; n<NUM; n++) 
	{
		segment.x = n * SEGMENT_WIDTH;
		segment.h = - array[n]; // NOTE: direction of increasing y reversed in SDL
		SDL_RenderFillRect(rend, &segment); // draw
		SDL_RenderPresent(rend);

		// added delay for step effect, usleep is actually DEPRECATED now,
		// please return and replace with nanosleep()!
		usleep(15000);
	}
}

void draw_state(int *array, bool *hl) 
{
	render_backsplash();

	// draw current array sequence
	draw_segments(array, hl);

	// fps handling
	frameCount++;
	int timerFPS = SDL_GetTicks() - lastFrame;
	if(timerFPS<(1000/60))
		SDL_Delay((1000/60) - timerFPS);

	//draw
	SDL_RenderPresent(rend);
}

/////////////////////////
// SORTING ALGORITHMS //
////////////////////////

// Insertion Sort
void insertionSort(int *array, int n) 
{ 
	int i, element, j; 
	bool highlight[NUM] = {false};
	
	for (i = 1; i < n; i++)
	{ 
		element = array[i]; 
		j = i - 1; 
		while (j >= 0 && array[j] > element) 
		{ 
			array[j + 1] = array[j]; 
			j = j - 1; 
		} 
		array[j + 1] = element; 
		highlight[j+1] = true;
		draw_state(array, highlight);
		unhighlight_segments(highlight);
		usleep(50000);
	} 
	unhighlight_segments(highlight);
	draw_state(array, highlight);
} 

int main()
{
	running = 1; // running state is acted upon by input()
	bool SORT_COMPLETE = false;
	int segment_height[NUM]; // array of segment arrays, primary argument
	
	// SDL2 initialization
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		printf("Failed at SDL_Init()\n");
	if (SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &win, &rend) < 0)
		printf("Failed at SDL_CreateWindowAndRenderer()\n");
	SDL_SetWindowTitle(win, "Visual Sort");
	SDL_ShowCursor(0);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

	// message box surface/texture initialization
	if(TTF_Init()==-1) 
	{
    printf("TTF_Init: %s\n", TTF_GetError());
    exit(2);
  }

	// INITIAL CONDITIONS: Seed RNG, populate array and apply Fisher-Yates shuffle.
	// Please note that draw_initially() contains a usleep() call. usleep() is
	// now deprecated and should be replaced ny nanosleep() in the future.
	srand(time(NULL));
	populate_segment_height(segment_height);
	shuffle_segments(segment_height, NUM);
	draw_initially(segment_height);

	// RUNNING STATE //
	while (running) 
	{
		// fps handling
		lastFrame = SDL_GetTicks();
		if (lastFrame >= (lastTime+1000)) 
		{
			lastTime = lastFrame;
			fps = frameCount;
			frameCount = 0;
		}

		// look for input.
		input();
		
		if(!SORT_COMPLETE) {
			insertionSort(segment_height, NUM);
			SORT_COMPLETE = true;
			draw_finally(segment_height);
		}

	}

	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();
}
