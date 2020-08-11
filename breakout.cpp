#include "physics.h"
#include "render.h"
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

    render *renderer = new render("SDL BREAKOUT", RES_X, RES_Y, BPP);

    // Game objects
    game_obj *paddle = new game_obj;
    game_obj *ball = new game_obj;
    game_obj *blocks[NUM_BLOCKS_Y][NUM_BLOCKS_X];
    game_obj *press_a_key = new game_obj;

    int block_pos_x = BLOCK_SPACE_X_FIRST;
    int block_pos_y = BLOCK_SPACE_Y_FIRST;

    const int row_colours_r[NUM_BLOCKS_Y] = {0, 51, 102, 153, 204};
    const int row_colours_g[NUM_BLOCKS_Y] = {0, 51, 102, 153, 204};
    const int row_colours_b[NUM_BLOCKS_Y] = {255, 255, 255, 255, 255};

    // Paddle
    paddle->sprite = SDL_CreateRGBSurface(0, 100, 20, 32, 0, 0, 0, 0);
    paddle->phys = new phys_obj;
    paddle->phys->pos_x = 50;
    paddle->phys->pos_y = 550;
    paddle->phys->size_x = 100;
    paddle->phys->size_y = 20;
    paddle->phys->step_x = 0;
    paddle->phys->step_y = 0;
    paddle->phys->delay = 0;
    paddle->phys->delay_counter = 0;
    paddle->phys->bounce = 0;
    paddle->phys->collided = NULL;
    paddle->phys->callback = NULL;
    paddle->phys->active = true;
    paddle->pos_x = &paddle->phys->pos_x;
    paddle->pos_y = &paddle->phys->pos_y;
    paddle->active = &paddle->phys->active;
    SDL_FillRect(paddle->sprite, NULL, SDL_MapRGB(renderer->screen->format, 255, 255, 255));

    // Ball
    ball->sprite = SDL_CreateRGBSurface(0, 20, 20, 32, 0, 0, 0, 0);
    ball->phys = new phys_obj;
    ball->phys->pos_x = 100;
    ball->phys->pos_y = 500;
    ball->phys->size_x = 20;
    ball->phys->size_y = 20;
    ball->phys->step_x = 1;
    ball->phys->step_y = -1;
    ball->phys->delay = 0;
    ball->phys->delay_counter = 0;
    ball->phys->bounce = 1;
    ball->phys->collided = NULL;
    ball->phys->callback = collision_callback;
    ball->phys->active = true;
    ball->pos_x = &ball->phys->pos_x;
    ball->pos_y = &ball->phys->pos_y;
    ball->active = &ball->phys->active;
    SDL_FillRect(ball->sprite, NULL, SDL_MapRGB(renderer->screen->format, 255, 255, 255));


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
    physics->add_object(paddle->phys);
    physics->add_object(ball->phys);

    renderer->add_object(paddle);
    renderer->add_object(ball);
    renderer->add_object(press_a_key);

    for (int y = 0; y < NUM_BLOCKS_Y; y++) {
        for (int x = 0; x < NUM_BLOCKS_X; x++) {
            // Blocks
            blocks[y][x] = new game_obj;
            blocks[y][x]->phys = new phys_obj;

            blocks[y][x]->sprite = SDL_CreateRGBSurface(0, BLOCK_WIDTH, BLOCK_HEIGHT, 32, 0, 0, 0, 0);
            blocks[y][x]->phys->pos_x = block_pos_x;
            blocks[y][x]->phys->pos_y = block_pos_y;
            blocks[y][x]->phys->size_x = BLOCK_WIDTH;
            blocks[y][x]->phys->size_y = BLOCK_HEIGHT;
            blocks[y][x]->phys->step_x = 0;
            blocks[y][x]->phys->step_y = 0;
            blocks[y][x]->phys->delay = 0;
            blocks[y][x]->phys->delay_counter = 0;
            blocks[y][x]->phys->bounce = 0;
            blocks[y][x]->phys->collided = NULL;
            blocks[y][x]->phys->callback = NULL;
            blocks[y][x]->phys->active = true;
            blocks[y][x]->pos_x = &blocks[y][x]->phys->pos_x;
            blocks[y][x]->pos_y = &blocks[y][x]->phys->pos_y;
            blocks[y][x]->active = &blocks[y][x]->phys->active;
            SDL_FillRect(blocks[y][x]->sprite, NULL, SDL_MapRGB(renderer->screen->format, row_colours_r[y], row_colours_g[y], row_colours_b[y]));

            physics->add_object(blocks[y][x]->phys);
            renderer->add_object(blocks[y][x]);

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
            paddle->phys->step_x = 0;
            if (left) { paddle->phys->step_x = -1; }
            if (right) { paddle->phys->step_x = 1; }

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
        renderer->draw(2);
    }

    SDL_Quit();

    delete physics;
    delete renderer;
    delete paddle->phys;
    delete paddle;
    delete ball->phys;
    delete ball;
    for (int y = 0; y < NUM_BLOCKS_Y; y++) {
        for (int x = 0; x < NUM_BLOCKS_X; x++) {
            delete blocks[y][x]->phys;
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

