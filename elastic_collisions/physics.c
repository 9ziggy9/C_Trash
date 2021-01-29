#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include "physics.h"

int generate_particles(SDL_Renderer* renderer, SDL_Window* window, float SPEED_X, float SPEED_Y, int NUM, int WINDOW_WIDTH, int WINDOW_HEIGHT)
{
    SDL_Surface* surface[NUM];
    SDL_Texture* tex[NUM];

    for (int j=0; j<NUM; ++j)
    {
        // load the image into memory using SDL_image library function
        surface[j] = IMG_Load("resources/poke.png");
        if (!surface[j])
        {
            printf("error creating surface\n");
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
    }

    for(int k=0; k<NUM; ++k)
    {
        // load the image data into the graphics hardware's memory
        tex[k] = SDL_CreateTextureFromSurface(renderer, surface[k]);
        SDL_FreeSurface(surface[k]);
        if (!tex[k])
        {
            printf("error creating texture: %s\n", SDL_GetError());
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
    }

    physics(renderer, *tex, SPEED_X, SPEED_Y, NUM, WINDOW_WIDTH, WINDOW_HEIGHT);

    return 0;
}

void physics(SDL_Renderer* renderer, SDL_Texture* texture, float SPEED_X, float SPEED_Y, int NUM, int WINDOW_WIDTH, int WINDOW_HEIGHT)
{
    // struct to hold the position and size of the sprite
    SDL_Rect dest[NUM];
    float x_pos[NUM], y_pos[NUM], x_vel[NUM], y_vel[NUM];
    float vx_jj, vx_lj, vx_jl, vx_ll, xx_jj, xx_ll, xx_jl, subjl_x, subjl_y, 
          sublj_x, sublj_y;
    float Dx_vel[NUM], Dy_vel[NUM];
    int X, Y;
    int particle = 0;
    bool occupied = 1;
    bool unoccupied = 0;

    for (int i=0; i<NUM; ++i)
    {
        // get and scale the dimensions of texture
        SDL_QueryTexture(texture, NULL, NULL, &dest[i].w, &dest[i].h);
        dest[i].w /= 4;
        dest[i].h /= 4;
    }

    unsigned int MAX_X = WINDOW_WIDTH / dest[0].w;
    unsigned int MAX_Y = WINDOW_HEIGHT / dest[0].h;
    bool grid[MAX_X][MAX_Y];

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

    for(int count_y=0; count_y<MAX_Y; ++count_y)
    {
        for(int count_x=0; count_x<MAX_X; ++count_x)
        {
            if (grid[count_x][count_y])
            {
                // start sprite in grid location
                x_pos[particle] = count_x * dest[0].w;
                y_pos[particle] = count_y * dest[0].h;

                // give sprite initial velocity
                x_vel[particle] = pow(-1, rand()%2) * SPEED_X;
                y_vel[particle] = pow(-1, rand()%2) * SPEED_Y;

                ++particle;
                printf("Particle %d generated.\n", particle);
            }      
        }
    }

    // set to 1 when window close button is pressed
    int close_requested = 0;
    
        // animation loop
        while (!close_requested)
        {
            // process events
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                switch(event.type)
                {
                    case SDL_QUIT:
                        close_requested = 1;
                        break;
                    case SDL_KEYDOWN:
                        close_requested = 1;
                        break;
                    default:
                        break;
                }
            }

            for (int j=0; j<NUM; ++j)    
            {
                // collision detection on bounds
                if (x_pos[j] <= 0)
                {
                    x_pos[j] = 0;
                    x_vel[j] = -x_vel[j];
                }
                if (y_pos[j] <= 0)
                {
                    y_pos[j] = 0;
                    y_vel[j] = -y_vel[j];
                }
                if (x_pos[j] >= WINDOW_WIDTH - dest[j].w) 
                {
                    x_pos[j] = WINDOW_WIDTH - dest[j].w;
                    x_vel[j] = -x_vel[j];
                }
                if (y_pos[j] >= WINDOW_HEIGHT - dest[j].h)
                {
                    y_pos[j] = WINDOW_HEIGHT - dest[j].h;
                    y_vel[j] = -y_vel[j];
                }

                for (int l=0; l<j; ++l)
                {
                    // collision detection on particles
                    if ((fabsf(x_pos[j] - x_pos[l]) <= dest[0].w) && 
                        (fabsf(y_pos[j] - y_pos[l]) <= dest[0].w))
                    {
                        vx_jj = x_vel[j]*x_pos[j] + y_vel[j]*y_pos[j];
                        vx_lj = x_vel[l]*x_pos[j] + y_vel[l]*y_pos[j];
                        vx_jl = x_vel[j]*x_pos[l] + y_vel[j]*y_pos[l];
                        vx_ll = x_vel[l]*x_pos[l] + y_vel[l]*y_pos[l];

                        xx_jj = x_pos[j]*x_pos[j] + y_pos[j]*y_pos[j];
                        xx_ll = x_pos[l]*x_pos[l] + y_pos[l]*y_pos[l];
                        xx_jl = x_pos[j]*x_pos[l] + y_pos[j]*y_pos[l];

                        subjl_x = x_pos[j] - x_pos[l];
                        subjl_y = y_pos[j] - y_pos[l];
                        sublj_x = x_pos[l] - x_pos[j];
                        sublj_y = y_pos[l] - y_pos[j];

                        Dx_vel[j] = x_vel[j] - (((vx_jj - vx_lj - vx_jl + vx_ll) / (xx_jj + xx_ll - (2 * xx_jl))) * subjl_x );
                        
                        Dy_vel[j] = y_vel[j] - (((vx_jj - vx_lj - vx_jl + vx_ll) / (xx_jj + xx_ll - (2 * xx_jl))) * subjl_y );
                        
                        Dx_vel[l] = x_vel[l] - (((vx_jj - vx_lj - vx_jl + vx_ll) / (xx_jj + xx_ll - (2 * xx_jl))) * sublj_x );
                        
                        Dy_vel[l] = y_vel[l] - (((vx_jj - vx_lj - vx_jl + vx_ll) / (xx_jj + xx_ll - (2 * xx_jl))) * sublj_y );

                        x_vel[j] = Dx_vel[j];
                        x_vel[l] = Dx_vel[l];
                        y_vel[j] = Dy_vel[j];
                        y_vel[l] = Dy_vel[l];
                    }
                        for (int m=0; m<NUM; ++m)
                        {
                            x_pos[m] += x_vel[m] / 60;
                            y_pos[m] += y_vel[m] / 60;
                        }
                }

                // set the positions in the struct
                dest[j].y = (int) y_pos[j];
                dest[j].x = (int) x_pos[j];
            }   

        // clear the window
        SDL_RenderClear(renderer);

        for (int k=0; k<NUM; ++k)
        {
            // draw the image to the window
            SDL_RenderCopy(renderer, texture, NULL, &dest[k]);
            SDL_RenderPresent(renderer);
        }

        // wait
        SDL_Delay(1000/((NUM+1)*30));
    }
}
