#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2_gfxPrimitives.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 640
#define NUM 32

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

void populate_segment_array(int *array)
{
	int step = WINDOW_HEIGHT / NUM;

	for (int n=1; n<=NUM; n++)
		array[n] = step * n;
}

void swap(int *a, int *b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
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

void draw_segments(int *height)
{
	int SEGMENT_WIDTH = WINDOW_WIDTH / NUM;

	// foreground, segment struct, gruvbox
	SDL_Rect segment;
	SDL_SetRenderDrawColor(rend, 251, 241, 199, 255);
	segment.y = WINDOW_HEIGHT;
	segment.w = SEGMENT_WIDTH;

	populate_segment_array(height);
	shuffle_segments(height, NUM);

	for (int n=0; n<NUM; n++) 
	{
		segment.x = n * SEGMENT_WIDTH;
		segment.h = - height[n]; // NOTE: direction of increasing y reversed in SDL
		SDL_RenderFillRect(rend, &segment); // draw
	}
}

void draw_state(int *height) 
{
	// backsplash of window, gruxbox gray
	SDL_SetRenderDrawColor(rend, 40, 40, 40, 255);
	SDL_Rect rect;
	rect.x = rect.y = 0;
	rect.w = WINDOW_WIDTH;
	rect.h = WINDOW_HEIGHT;
	SDL_RenderFillRect(rend, &rect);

	// draw current array sequence
	draw_segments(height);

	// fps handling
	frameCount++;
	int timerFPS = SDL_GetTicks() - lastFrame;
	if(timerFPS<(1000/60))
		SDL_Delay((1000/60) - timerFPS);

	//draw
	SDL_RenderPresent(rend);
}

int main()
{
	running = 1; // running state is acted upon by input()
	int segment_height[NUM]; // array of segment heights, primary argument
	
	// SDL2 initialization
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		printf("Failed at SDL_Init()\n");
	if (SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &win, &rend) < 0)
		printf("Failed at SDL_CreateWindowAndRenderer()\n");
	SDL_SetWindowTitle(win, "Visual Sort");
	SDL_ShowCursor(0);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

	// INITIAL CONDITIONS: Seed RNG, populate array and apply Fisher-Yates shuffle
	srand(time(NULL));
	populate_segment_array(segment_height);
	shuffle_segments(segment_height, NUM);

	// RUNNING STATE //
	while (running) 
	{
		lastFrame = SDL_GetTicks();
		if (lastFrame >= (lastTime+1000)) 
		{
			lastTime = lastFrame;
			fps = frameCount;
			frameCount = 0;
		}
		
		// print statement for fps debugging.
		printf("%d\n", fps);

		// look for input.
		input();

		// draw state to frame
		draw_state(segment_height);
	}

	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();
}
