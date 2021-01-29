#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2_gfxPrimitives.h>

#define RAD (10)
#define NUM (6)
#define WINDOW_WIDTH (640)
#define WINDOW_HEIGHT (480)

int main(void)
{
    float x_pos[NUM], y_pos[NUM], x_vel[NUM], y_vel[NUM];
    int X, Y;
    int particle = 0;
    bool occupied = 1;
    bool unoccupied = 0;
    unsigned int MAX_X = WINDOW_WIDTH / (2*RAD);
    unsigned int MAX_Y = WINDOW_HEIGHT / (2*RAD);
    bool grid[MAX_X][MAX_Y];
    
    // attempt to initialize graphics and timer system
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* win = SDL_CreateWindow("grid",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       WINDOW_WIDTH,
                                       WINDOW_HEIGHT,
                                       0);
    if (!win)
    {
        printf("error creating window: %s\n", SDL_GetError());
        SDL_Quit();
	    return 1;
    }

    // create a renderer, which sets up the graphics hardware
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);
    if (!rend)
    {
      printf("error creating renderer: %s\n", SDL_GetError());
      SDL_DestroyWindow(win);
      SDL_Quit();
      return 1;
    }

    // seed RNG
    srand(time(NULL));

    // set grid to unoccupied
    for(int i=0; i<MAX_X; ++i)
        for(int j=0; j<MAX_Y; ++j)
            grid[i][j] = unoccupied;

    // randomly fill grid
    for(int count=0; count<NUM; ++count)
    {
        X = rand() % MAX_X;
        Y = rand() % MAX_Y;
        
        if (!grid[X][Y])
            grid[X][Y] = occupied;
        else
            --count;
    }

    // clear the window
    SDL_RenderClear(rend);
    
    for(int count_y=0; count_y<MAX_Y; ++count_y)
    {
        for(int count_x=0; count_x<MAX_X; ++count_x)
        {
            if (grid[count_x][count_y])
            {
                // start sprite in grid location
                x_pos[particle] = count_x * (2*RAD);
                y_pos[particle] = count_y * (2*RAD);
                filledCircleColor(rend, x_pos[particle], y_pos[particle],
                                  RAD, 0xcc241dff);
                ++particle;
                printf("Particle %d generated.\n", particle);
            }      
        }
    }
    

    // draw the image to the window
    SDL_RenderPresent(rend);

    // wait a few seconds
    SDL_Delay(5000);
    
    // clean up resources before exiting
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
}
