#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include "physics.h"

int main(int argc, char *argv[])
{
    int WINDOW_WIDTH = 640;
    int WINDOW_HEIGHT = 480;
    int opt;
    char *number_value = NULL;
    char *speed_value = NULL;

    opterr = 0;

    while ((opt = getopt(argc, argv, "n:s:")) != -1)
        switch (opt)
        {
            case 'n':
                number_value = optarg;
                break;
            case 's':
                speed_value = optarg;
                break;
            case '?':
                fprintf (stderr, "Unknown options");
                return 1;
            default:
                abort ();
        }

    int NUM = atoi(number_value);
    int SPEED = atoi(speed_value);

    printf("\nNumber of particles: %d\n", NUM);
    printf("Particle speed: %d\n", SPEED);
    printf("\n");

    // attempt to initialize graphics and timer system
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* win = SDL_CreateWindow("test",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       WINDOW_WIDTH, WINDOW_HEIGHT,0);
    if (!win)
    {
        printf("error creating window: %s\n", SDL_GetError());
        SDL_Quit();
	      return 1;
    }

    // create a renderer, which sets up the graphics hardware
    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);
    if (!rend)
    {
      printf("error creating renderer number: %s\n", SDL_GetError());
      SDL_DestroyWindow(win);
      SDL_Quit();
      return 1;
    }
    
    generate_particles(rend, win, SPEED, SPEED, NUM, WINDOW_WIDTH, WINDOW_HEIGHT                         );
    
    // clean up resources before exiting
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
}
