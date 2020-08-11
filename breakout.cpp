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

// Allocate surfaces
//SDL_Surface *numbers = NULL;
SDL_Surface *press_a_key = NULL;
//SDL_Surface *p2win = NULL;

// SDL Rect for positions of numbers in numbers.png
//SDL_Rect num[11];

// SDL event for handling input
SDL_Event input;

// Variables for player scores
//int scoreplayer1;
//int scoreplayer2;

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
    /*
    // Check if collision is with edge
    if (obj2 == NULL) {
        if (obj->pos_x <= 0) {
            scoreplayer2++;
            obj->pos_x = 300;
            obj->pos_y = 100;
            if (obj->step_y > 0) { obj->step_y = 1; } else { obj->step_y = -1; }
        } else if (obj->pos_x+obj->size_x >= area_x) {
            scoreplayer1++;
            obj->pos_x = 300;
            obj->pos_y = 100;
            if (obj->step_y > 0) { obj->step_y = 1; } else { obj->step_y = -1; }
        }
    } else if (collide_axis == 1) {
        int size = obj2->size_y+obj->size_y;
        int point = obj2->pos_y-obj->size_y;
        int seg = size/4;
        int y1 = point+seg;
        int y2 = y1+seg+seg;
        int y3 = y2+seg;

        if (obj->pos_y >= point) {
            if (obj->pos_y <= y1) {
                obj->step_y = -2;
            } else if (obj->pos_y <= y2) {
                if (obj->step_y > 0) { obj->step_y = 1; } else { obj->step_y = -1; }
            } else if (obj->pos_y <= y3) {
                obj->step_y = 2;
            }
        }
    }
    */
}

void breakout()
{
    // Variables
/*
    // Variables for player scores
    scoreplayer1 = 0;
    scoreplayer2 = 0;

    // Positions for numbers.png
    // 0
    num[0].x = 0;
    num[0].y = 0;
    num[0].w = 50;
    num[0].h = 50;

    // 1
    num[1].x = 50;
    num[1].y = 0;
    num[1].w = 50;
    num[1].h = 50;

    // 2
    num[2].x = 100;
    num[2].y = 0;
    num[2].w = 50;
    num[2].h = 50;

    // 3
    num[3].x = 150;
    num[3].y = 0;
    num[3].w = 50;
    num[3].h = 50;

    // 4
    num[4].x = 200;
    num[4].y = 0;
    num[4].w = 50;
    num[4].h = 50;

    // 5
    num[5].x = 250;
    num[5].y = 0;
    num[5].w = 50;
    num[5].h = 50;

    // 6
    num[6].x = 300;
    num[6].y = 0;
    num[6].w = 50;
    num[6].h = 50;

    // 7
    num[7].x = 350;
    num[7].y = 0;
    num[7].w = 50;
    num[7].h = 50;

    // 8
    num[8].x = 400;
    num[8].y = 0;
    num[8].w = 50;
    num[8].h = 50;

    // 9
    num[9].x = 450;
    num[9].y = 0;
    num[9].w = 50;
    num[9].h = 50;

    // 10
    num[10].x = 450;
    num[10].y = 0;
    num[10].w = 50;
    num[10].h = 50;
*/

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
    //numbers = SDL_DisplayFormat(IMG_Load("numbers.png"));
    //p2win = SDL_DisplayFormat(IMG_Load("p2win.png"));
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

    // Timer
    //unsigned int timer = SDL_GetTicks();

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
            // Move left paddle
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

/*
        // First number
        offset.x = 200;
        offset.y = 0;
        SDL_BlitSurface(numbers, &num[scoreplayer1], screen, &offset );

        // Second number
        offset.x = 390;
        offset.y = 0;
        SDL_BlitSurface(numbers, &num[scoreplayer2], screen, &offset );
*/

//        if (wait_for_input) {
//            offset.x = 306;
//            offset.y = 290;
//            SDL_BlitSurface(press_a_key, NULL, screen, &offset);
//        }

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

