#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

#define WINDOW_WIDTH (640)
#define WINDOW_HEIGHT (480)

// speed in pixels/second
#define SPEED (15)
#define NUM (6)

void animate_particle(SDL_Renderer* renderer, SDL_Texture* texture, float VEL_X, float VEL_Y);
int generate_particles(SDL_Renderer* renderer, SDL_Window* window, float VEL_X, float VEL_Y);

int main(void)
{
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
    
    generate_particles(rend, win, SPEED, SPEED);
    
    // clean up resources before exiting
       SDL_DestroyRenderer(rend);
       SDL_DestroyWindow(win);
       SDL_Quit();
}

int generate_particles(SDL_Renderer* renderer, SDL_Window* window, float VEL_X, float VEL_Y)
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

    animate_particle(renderer, *tex, VEL_X, VEL_Y);

    return 0;
}

void animate_particle(SDL_Renderer* renderer, SDL_Texture* texture, float VEL_X, float VEL_Y)
{
    // struct to hold the position and size of the sprite
    SDL_Rect dest[NUM];
    float x_pos[NUM], y_pos[NUM], x_vel[NUM], y_vel[NUM];
    float vx_jj, vx_lj, vx_jl, vx_ll, xx_jj, xx_ll, xx_jl, subjl_x, subjl_y, sublj_x, sublj_y;
    float Dx_vel[NUM], Dy_vel[NUM];

    for (int i=0; i<NUM; ++i)
    {
        // get and scale the dimensions of texture
        SDL_QueryTexture(texture, NULL, NULL, &dest[i].w, &dest[i].h);
        dest[i].w /= 4;
        dest[i].h /= 4;

        // start sprite in center of screen
        x_pos[i] = (WINDOW_WIDTH - dest[i].w) / 2;
        y_pos[i] = (WINDOW_HEIGHT - dest[i].h) / 2;

        // give sprite initial velocity
        x_vel[i] = (i+1)*VEL_X;
        y_vel[i] = (i+1)*VEL_Y;
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
                if (event.type == SDL_QUIT)
                {
                    close_requested = 1;
                }
            }

            for (int j=0; j<NUM; ++j)    
            {
                // collision detection with bounds
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
                    // collision detection with bounds
                    if ((fabsf(x_pos[j] - x_pos[l]) <= dest[j].w) && (fabsf(y_pos[j] - y_pos[l]) <= dest[j].h))
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

                        Dx_vel[j] = x_vel[j] - (((vx_jj - vx_lj - vx_jl + vx_ll) / 
                                    (xx_jj + xx_ll - (2 * xx_jl))) * subjl_x );
                        Dy_vel[j] = y_vel[j] - (((vx_jj - vx_lj - vx_jl + vx_ll) / 
                                    (xx_jj + xx_ll - (2 * xx_jl))) * subjl_y );
                        Dx_vel[l] = x_vel[l] - (((vx_jj - vx_lj - vx_jl + vx_ll) / 
                                    (xx_jj + xx_ll - (2 * xx_jl))) * sublj_x );
                        Dy_vel[l] = y_vel[l] - (((vx_jj - vx_lj - vx_jl + vx_ll) / 
                                    (xx_jj + xx_ll - (2 * xx_jl))) * sublj_y );

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

        // wait 1/60th of a second
        SDL_Delay(1000/60);
    }
}
