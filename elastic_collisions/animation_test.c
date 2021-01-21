#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

#define WINDOW_WIDTH (640)
#define WINDOW_HEIGHT (480)

// speed in pixels/second
#define SPEED (300)

void initialize_particle(SDL_Renderer* renderer, SDL_Texture* texture, float POS_X, float POS_Y);
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
    
    generate_particles(rend, win, 2*SPEED, SPEED);
    
    // clean up resources before exiting
    // SDL_DestroyTexture(tex);
    // SDL_DestroyRenderer(rend);
    // SDL_DestroyWindow(win);
    // SDL_Quit();
}

int generate_particles(SDL_Renderer* renderer, SDL_Window* window, float VEL_X, float VEL_Y)
{
    SDL_Surface* surface[2];
    SDL_Texture* tex[2];

    for (int j=0; j<2; ++j)
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

    for(int k=0; k<2; ++k)
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
    SDL_Rect dest;

    // get and scale the dimensions of texture
    SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
    dest.w /= 4;
    dest.h /= 4;

    // start sprite in center of screen
    float x_pos = (WINDOW_WIDTH - dest.w) / 2;
    float y_pos = (WINDOW_HEIGHT - dest.h) / 2;

    // give sprite initial velocity
    float x_vel = VEL_X;
    float y_vel = VEL_Y;

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
        
        // collision detection with bounds
        if (x_pos <= 0)
        {
            x_pos = 0;
            x_vel = -x_vel;
        }
        if (y_pos <= 0)
        {
            y_pos = 0;
            y_vel = -y_vel;
        }
        if (x_pos >= WINDOW_WIDTH - dest.w) 
        {
            x_pos = WINDOW_WIDTH - dest.w;
            x_vel = -x_vel;
        }
        if (y_pos >= WINDOW_HEIGHT - dest.h)
        {
            y_pos = WINDOW_HEIGHT - dest.h;
            y_vel = -y_vel;
        }

        // update positions
        x_pos += x_vel / 60;
        y_pos += y_vel / 60;

        // set the positions in the struct
        dest.y = (int) y_pos;
        dest.x = (int) x_pos;
        
        // clear the window
        SDL_RenderClear(renderer);

        // draw the image to the window
        SDL_RenderCopy(renderer, texture, NULL, &dest);
        SDL_RenderPresent(renderer);

        // wait 1/60th of a second
        SDL_Delay(1000/60);
    }
}
