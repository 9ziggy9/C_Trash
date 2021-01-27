#ifndef _PHYSICS_H_
#define _PHYSICS_H_

int generate_particles(SDL_Renderer* renderer, SDL_Window* window, float SPEED_X, float SPEED_Y, int NUM,
                        int WINDOW_WIDTH, int WINDOW_HEIGHT);

void physics(SDL_Renderer* renderer, SDL_Texture* texture, float SPEED_X, float SPEED_Y, int NUM,
              int WINDOW_WIDTH, int WINDOW_HEIGHT);

#endif
