#include "physics.h"
#include "graphics.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#define RES_X 800
#define RES_Y 600
#define BPP 32

#define NUM_BLOCKS_X 8
#define NUM_BLOCKS_Y 5
#define BLOCK_WIDTH 80
#define BLOCK_HEIGHT 20
#define BLOCK_SPACE_X 4
#define BLOCK_SPACE_X_FIRST 66
#define BLOCK_SPACE_Y 4
#define BLOCK_SPACE_Y_FIRST 100

SDL_Surface *press_a_key = NULL;
SDL_Event input;
bool wait_for_input = false;

void collision_callback(phys_obj *obj, phys_obj *obj2, int collide_axis, int area_x, int area_y)
{
    if (obj2 == NULL) {
        if (obj->pos_y+obj->size_y >= area_y) {
            obj->pos_x = 100;
            obj->pos_y = 500;
            wait_for_input = true;
        }
    } else if (obj2->pos_y < 500) {
        obj2->active = false;
    } else {
        if (obj->pos_x <= obj2->pos_x+20) {
            obj->step_x = -2;
        } else if (obj->pos_x >= obj2->pos_x+80) {
            obj->step_x = 2;
        } else if (obj->step_x == 2) {
            obj->step_x = 1;
        } else if (obj->step_x == -2) {
            obj->step_x = -1;
        }
    }
}

void breakout()
{
    // bool vars for control directions and quit event
    bool quit = false;
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;

    wait_for_input = true;

    graphics *window = new graphics("SDL BREAKOUT", RES_X, RES_Y, BPP);

    // Game objects
    graphics_obj *paddle = new graphics_obj;
    phys_obj *paddle_phys = new phys_obj;
    graphics_obj *ball = new graphics_obj;
    phys_obj *ball_phys = new phys_obj;
    graphics_obj *blocks[NUM_BLOCKS_Y][NUM_BLOCKS_X];
    phys_obj *blocks_phys[NUM_BLOCKS_Y][NUM_BLOCKS_X];
    graphics_obj *press_a_key = new graphics_obj;

    int block_pos_x = BLOCK_SPACE_X_FIRST;
    int block_pos_y = BLOCK_SPACE_Y_FIRST;

    const int row_colours_r[NUM_BLOCKS_Y] = {0, 51, 102, 153, 204};
    const int row_colours_g[NUM_BLOCKS_Y] = {0, 51, 102, 153, 204};
    const int row_colours_b[NUM_BLOCKS_Y] = {255, 255, 255, 255, 255};

    // Paddle
    paddle_phys->pos_x = 50;
    paddle_phys->pos_y = 550;
    paddle_phys->size_x = 100;
    paddle_phys->size_y = 20;
    paddle_phys->step_x = 0;
    paddle_phys->step_y = 0;
    paddle_phys->delay = 0;
    paddle_phys->delay_counter = 0;
    paddle_phys->bounce = 0;
    paddle_phys->collided = NULL;
    paddle_phys->callback = NULL;
    paddle_phys->active = true;
    paddle->sprite = SDL_CreateRGBSurface(0, 100, 20, 32, 0, 0, 0, 0);
    paddle->pos_x = &paddle_phys->pos_x;
    paddle->pos_y = &paddle_phys->pos_y;
    paddle->active = &paddle_phys->active;
    SDL_FillRect(paddle->sprite, NULL, SDL_MapRGB(window->screen->format, 255, 255, 255));

    // Ball
    ball_phys->pos_x = 100;
    ball_phys->pos_y = 500;
    ball_phys->size_x = 20;
    ball_phys->size_y = 20;
    ball_phys->step_x = 1;
    ball_phys->step_y = -1;
    ball_phys->delay = 0;
    ball_phys->delay_counter = 0;
    ball_phys->bounce = 1;
    ball_phys->collided = NULL;
    ball_phys->callback = collision_callback;
    ball_phys->active = true;
    ball->sprite = SDL_CreateRGBSurface(0, 20, 20, 32, 0, 0, 0, 0);
    ball->pos_x = &ball_phys->pos_x;
    ball->pos_y = &ball_phys->pos_y;
    ball->active = &ball_phys->active;
    SDL_FillRect(ball->sprite, NULL, SDL_MapRGB(window->screen->format, 255, 255, 255));


    // Load images
    press_a_key->sprite = SDL_DisplayFormat(IMG_Load("press_a_key.png"));
    press_a_key->draw_pos_x = 306;
    press_a_key->draw_pos_y = 290;
    press_a_key->draw_active = false;
    press_a_key->pos_x = &press_a_key->draw_pos_x;
    press_a_key->pos_y = &press_a_key->draw_pos_y;
    press_a_key->active = &press_a_key->draw_active;

    // Physics
    phys *physics = new phys(RES_X, RES_Y);
    physics->add_object(paddle_phys);
    physics->add_object(ball_phys);

    window->add_object(paddle);
    window->add_object(ball);
    window->add_object(press_a_key);

    for (int y = 0; y < NUM_BLOCKS_Y; y++) {
        for (int x = 0; x < NUM_BLOCKS_X; x++) {
            // Blocks
            blocks[y][x] = new graphics_obj;
            blocks_phys[y][x] = new phys_obj;

            blocks_phys[y][x]->pos_x = block_pos_x;
            blocks_phys[y][x]->pos_y = block_pos_y;
            blocks_phys[y][x]->size_x = BLOCK_WIDTH;
            blocks_phys[y][x]->size_y = BLOCK_HEIGHT;
            blocks_phys[y][x]->step_x = 0;
            blocks_phys[y][x]->step_y = 0;
            blocks_phys[y][x]->delay = 0;
            blocks_phys[y][x]->delay_counter = 0;
            blocks_phys[y][x]->bounce = 0;
            blocks_phys[y][x]->collided = NULL;
            blocks_phys[y][x]->callback = NULL;
            blocks_phys[y][x]->active = true;
            blocks[y][x]->sprite = SDL_CreateRGBSurface(0, BLOCK_WIDTH, BLOCK_HEIGHT, 32, 0, 0, 0, 0);
            blocks[y][x]->pos_x = &blocks_phys[y][x]->pos_x;
            blocks[y][x]->pos_y = &blocks_phys[y][x]->pos_y;
            blocks[y][x]->active = &blocks_phys[y][x]->active;
            SDL_FillRect(blocks[y][x]->sprite, NULL, SDL_MapRGB(window->screen->format, row_colours_r[y], row_colours_g[y], row_colours_b[y]));

            physics->add_object(blocks_phys[y][x]);
            window->add_object(blocks[y][x]);

            block_pos_x += BLOCK_WIDTH + BLOCK_SPACE_X;
        }

        block_pos_y += BLOCK_HEIGHT + BLOCK_SPACE_Y;
        block_pos_x = BLOCK_SPACE_X_FIRST;
    }

    // Main loop
    while (quit==false)
    {
        // Read inputs
        while (SDL_PollEvent(&input))
        {
            switch (input.type)
            {
                case SDL_KEYDOWN:
                    switch (input.key.keysym.sym)
                        {
                            case SDLK_LEFT:
                                left = true;
                                break;
                            case SDLK_RIGHT:
                                right = true;
                                break;
                            case SDLK_UP:
                                up = true;
                                break;
                            case SDLK_DOWN:
                                down = true;
                                break;
                            case SDLK_q:
                                quit = true;
                                break;
                        }
                        break;
                case SDL_KEYUP:
                    switch (input.key.keysym.sym)
                        {
                            case SDLK_LEFT:
                                left = false;
                                break;
                            case SDLK_RIGHT:
                                right = false;
                                break;
                            case SDLK_UP:
                                up = false;
                                break;
                            case SDLK_DOWN:
                                down = false;
                                break;
                        }
            }
        }

        if (!wait_for_input) {
            // Move paddle
            paddle_phys->step_x = 0;
            if (left) { paddle_phys->step_x = -1; }
            if (right) { paddle_phys->step_x = 1; }

            // Advance physics
            physics->advance();
        } else {
            if (left || right || up || down) {
                wait_for_input = false;
            }
        }

        if (wait_for_input) {
            press_a_key->draw_active = true;
        } else {
            press_a_key->draw_active = false;
        }

        // Redraw screen
        window->draw(2);
    }

    SDL_Quit();

    delete physics;
    delete window;
    delete paddle_phys;
    delete paddle;
    delete ball_phys;
    delete ball;
    for (int y = 0; y < NUM_BLOCKS_Y; y++) {
        for (int x = 0; x < NUM_BLOCKS_X; x++) {
            delete blocks_phys[y][x];
            delete blocks[y][x];
        }
    }

    return;
}

int main (int argc, char *argv[])
{
    breakout();
    return 0;
}

