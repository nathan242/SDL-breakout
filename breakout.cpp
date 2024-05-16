#include "physics.h"
#include "graphics.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define RES_X 800
#define RES_Y 600
#define BPP 32

#define NUM_BLOCKS 40
#define BLOCK_WIDTH 80
#define BLOCK_HEIGHT 20
#define BLOCK_SPACE_X 4
#define BLOCK_SPACE_Y 4

#define MAX_LIVES 3
#define MAX_LEVELS 3

#define BALL_PHYS_DELAY 3100000
#define PADDLE_PHYS_DELAY 3000000

SDL_Event input;
bool wait_for_input = false;
bool input_released = false;
bool is_game_over = false;
bool level_changed = false;
bool life_lost = false;
int lives;
int level;
int block_count;
timespec inittime {0, 0};

void collision_callback(phys_obj *obj, phys_obj *obj2, int collide_axis, int area_x, int area_y)
{
    if (obj2 == NULL) {
        if (obj->pos_y+obj->size_y >= area_y) {
            obj->pos_x = 100;
            obj->pos_y = 500;

            if (--lives == 0) {
                is_game_over = true;
            } else {
                life_lost = true;
                input_released = false;
                wait_for_input = true;
            }
        }
    } else if (obj2->pos_y < 500) {
        obj2->active = false;

        if (--block_count == 0) {
            block_count = NUM_BLOCKS;
            level++;
            level_changed = true;
            obj->pos_x = 100;
            obj->pos_y = 500;
            input_released = false;
            wait_for_input = true;
        }
    } else {
        obj->move_x_every = BALL_PHYS_DELAY;
        if (obj->pos_x <= obj2->pos_x+20) {
            obj->step_x = -1;
            obj->move_x_every = BALL_PHYS_DELAY - 100000;
        } else if (obj->pos_x >= obj2->pos_x+60) {
            obj->step_x = 1;
            obj->move_x_every = BALL_PHYS_DELAY - 100000;
        }
        // else if (obj->step_x == 2) {
        //     obj->step_x = 1;
        //     obj->move_x_every = DEFAULT_PHYS_DELAY;
        // } else if (obj->step_x == -2) {
        //     obj->step_x = -1;
        //     obj->move_x_every = DEFAULT_PHYS_DELAY;
        // }

        if ((obj->pos_x >= obj2->pos_x+21 && obj->pos_x <= obj2->pos_x+35) || (obj->pos_x >= obj2->pos_x+45 && obj->pos_x <= obj2->pos_x+59)) {
            obj->move_x_every = BALL_PHYS_DELAY;
        } else if (obj->pos_x >= obj2->pos_x+36 && obj->pos_x <= obj2->pos_x+44) {
            obj->move_x_every = BALL_PHYS_DELAY + 2400000;
        }
    }
}

void setup_blocks_level_0(graphics *window, phys_obj *blocks_phys[], graphics_obj *blocks[])
{
    int block_pos_x = 66;
    int block_pos_y = 100;
    int block_id;

    const int row_colours_r[5] = {0, 51, 102, 153, 204};
    const int row_colours_g[5] = {0, 51, 102, 153, 204};
    const int row_colours_b[5] = {255, 255, 255, 255, 255};

    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 8; x++) {
            // Blocks
            block_id = (y*8)+x;

            blocks_phys[block_id]->pos_x = block_pos_x;
            blocks_phys[block_id]->pos_y = block_pos_y;
            blocks_phys[block_id]->size_x = BLOCK_WIDTH;
            blocks_phys[block_id]->size_y = BLOCK_HEIGHT;
            blocks_phys[block_id]->step_x = 0;
            blocks_phys[block_id]->step_y = 0;
            blocks_phys[block_id]->move_x_every = 0;
            blocks_phys[block_id]->move_y_every = 0;
            blocks_phys[block_id]->move_x_last = inittime;
            blocks_phys[block_id]->move_y_last = inittime;
            blocks_phys[block_id]->bounce = 0;
            blocks_phys[block_id]->collided = NULL;
            blocks_phys[block_id]->callback = NULL;
            blocks_phys[block_id]->active = true;
            blocks[block_id]->pos_x = &blocks_phys[block_id]->pos_x;
            blocks[block_id]->pos_y = &blocks_phys[block_id]->pos_y;
            blocks[block_id]->active = &blocks_phys[block_id]->active;
            SDL_FillRect(blocks[block_id]->sprite, NULL, SDL_MapRGB(blocks[block_id]->sprite->format, row_colours_r[y], row_colours_g[y], row_colours_b[y]));

            if (blocks[block_id]->texture != NULL) {
                SDL_DestroyTexture(blocks[block_id]->texture);
            }

            blocks[block_id]->texture = SDL_CreateTextureFromSurface(window->renderer, blocks[block_id]->sprite);

            block_pos_x += BLOCK_WIDTH + BLOCK_SPACE_X;
        }

        block_pos_y += BLOCK_HEIGHT + BLOCK_SPACE_Y;
        block_pos_x = 66;
    }
}

void setup_blocks_level_1(graphics *window, phys_obj *blocks_phys[], graphics_obj *blocks[])
{
    int block_pos_x = 66;
    int block_pos_y = 100;
    int block_id;

    const int row_colours_r[10] = {0, 25, 51, 75, 102, 125, 153, 175, 204, 225};
    const int row_colours_g[10] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
    const int row_colours_b[10] = {0, 25, 51, 75, 102, 125, 153, 175, 204, 225};

    for (int y = 0; y < 10; y++) {
        if (y % 2 == 0) {
            block_pos_x += BLOCK_WIDTH + BLOCK_SPACE_X;
        }

        for (int x = 0; x < 4; x++) {
            // Blocks
            block_id = (y*4)+x;

            blocks_phys[block_id]->pos_x = block_pos_x;
            blocks_phys[block_id]->pos_y = block_pos_y;
            blocks_phys[block_id]->size_x = BLOCK_WIDTH;
            blocks_phys[block_id]->size_y = BLOCK_HEIGHT;
            blocks_phys[block_id]->step_x = 0;
            blocks_phys[block_id]->step_y = 0;
            blocks_phys[block_id]->move_x_every = 0;
            blocks_phys[block_id]->move_y_every = 0;
            blocks_phys[block_id]->move_x_last = inittime;
            blocks_phys[block_id]->move_y_last = inittime;
            blocks_phys[block_id]->bounce = 0;
            blocks_phys[block_id]->collided = NULL;
            blocks_phys[block_id]->callback = NULL;
            blocks_phys[block_id]->active = true;
            blocks[block_id]->pos_x = &blocks_phys[block_id]->pos_x;
            blocks[block_id]->pos_y = &blocks_phys[block_id]->pos_y;
            blocks[block_id]->active = &blocks_phys[block_id]->active;
            SDL_FillRect(blocks[block_id]->sprite, NULL, SDL_MapRGB(blocks[block_id]->sprite->format, row_colours_r[y], row_colours_g[y], row_colours_b[y]));

            if (blocks[block_id]->texture != NULL) {
                SDL_DestroyTexture(blocks[block_id]->texture);
            }

            blocks[block_id]->texture = SDL_CreateTextureFromSurface(window->renderer, blocks[block_id]->sprite);

            block_pos_x += BLOCK_WIDTH + BLOCK_SPACE_X + BLOCK_WIDTH + BLOCK_SPACE_X;
        }

        block_pos_y += BLOCK_HEIGHT + BLOCK_SPACE_Y;
        block_pos_x = 66;
    }
}

void setup_blocks_level_2(graphics *window, phys_obj *blocks_phys[], graphics_obj *blocks[])
{
    int block_pos_x = 66;
    int block_pos_y = 100;
    int block_id;

    const int row_colours_r[10] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
    const int row_colours_g[10] = {0, 25, 51, 75, 102, 125, 153, 175, 204, 225};
    const int row_colours_b[10] = {0, 25, 51, 75, 102, 125, 153, 175, 204, 225};

    for (int y = 0; y < 10; y++) {
        if (y % 2 == 0) {
            block_pos_x += BLOCK_WIDTH + BLOCK_SPACE_X;
        }

        for (int x = 0; x < 4; x++) {
            // Blocks
            block_id = (y*4)+x;

            blocks_phys[block_id]->pos_x = block_pos_x;
            blocks_phys[block_id]->pos_y = block_pos_y;
            blocks_phys[block_id]->size_x = BLOCK_WIDTH;
            blocks_phys[block_id]->size_y = BLOCK_HEIGHT;
            blocks_phys[block_id]->step_x = y % 2;
            blocks_phys[block_id]->step_y = 0;
            blocks_phys[block_id]->move_x_every = 0;
            blocks_phys[block_id]->move_y_every = 0;
            blocks_phys[block_id]->move_x_last = inittime;
            blocks_phys[block_id]->move_y_last = inittime;
            blocks_phys[block_id]->bounce = 1;
            blocks_phys[block_id]->collided = NULL;
            blocks_phys[block_id]->callback = NULL;
            blocks_phys[block_id]->active = true;
            blocks[block_id]->pos_x = &blocks_phys[block_id]->pos_x;
            blocks[block_id]->pos_y = &blocks_phys[block_id]->pos_y;
            blocks[block_id]->active = &blocks_phys[block_id]->active;
            SDL_FillRect(blocks[block_id]->sprite, NULL, SDL_MapRGB(blocks[block_id]->sprite->format, row_colours_r[y], row_colours_g[y], row_colours_b[y]));

            if (blocks[block_id]->texture != NULL) {
                SDL_DestroyTexture(blocks[block_id]->texture);
            }

            blocks[block_id]->texture = SDL_CreateTextureFromSurface(window->renderer, blocks[block_id]->sprite);

            block_pos_x += BLOCK_WIDTH + BLOCK_SPACE_X + BLOCK_WIDTH + BLOCK_SPACE_X;
        }

        block_pos_y += BLOCK_HEIGHT + BLOCK_SPACE_Y;
        block_pos_x = 66;
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
    lives = 3;
    level = 0;
    block_count = NUM_BLOCKS;
    char num_image[] = "x.png";
    char num_str[2];

    graphics *window = new graphics("SDL BREAKOUT", RES_X, RES_Y, BPP);

    // Game objects
    graphics_obj *paddle = new graphics_obj;
    phys_obj *paddle_phys = new phys_obj;
    graphics_obj *ball = new graphics_obj;
    phys_obj *ball_phys = new phys_obj;
    graphics_obj *blocks[NUM_BLOCKS];
    phys_obj *blocks_phys[NUM_BLOCKS];
    graphics_obj *press_a_key = new graphics_obj;
    graphics_obj *game_over = new graphics_obj;
    graphics_obj *level_text = new graphics_obj;
    graphics_obj *lives_text = new graphics_obj;
    graphics_obj *level_num[MAX_LEVELS];
    graphics_obj *lives_num[MAX_LIVES];

    // Paddle
    paddle_phys->pos_x = 50;
    paddle_phys->pos_y = 550;
    paddle_phys->size_x = 100;
    paddle_phys->size_y = 20;
    paddle_phys->step_x = 0;
    paddle_phys->step_y = 0;
    paddle_phys->move_x_every = PADDLE_PHYS_DELAY;
    paddle_phys->move_y_every = PADDLE_PHYS_DELAY;
    paddle_phys->move_x_last = inittime;
    paddle_phys->move_y_last = inittime;
    paddle_phys->bounce = 0;
    paddle_phys->collided = NULL;
    paddle_phys->callback = NULL;
    paddle_phys->active = true;
    paddle->sprite = SDL_CreateRGBSurface(0, 100, 20, 32, 0, 0, 0, 0);
    paddle->pos_x = &paddle_phys->pos_x;
    paddle->pos_y = &paddle_phys->pos_y;
    paddle->size_x = 100;
    paddle->size_y = 20;
    paddle->active = &paddle_phys->active;
    SDL_FillRect(paddle->sprite, NULL, SDL_MapRGB(paddle->sprite->format, 255, 255, 255));
    paddle->texture = SDL_CreateTextureFromSurface(window->renderer, paddle->sprite);

    // Ball
    ball_phys->pos_x = 100;
    ball_phys->pos_y = 500;
    ball_phys->size_x = 20;
    ball_phys->size_y = 20;
    ball_phys->step_x = 1;
    ball_phys->step_y = -1;
    ball_phys->move_x_every = BALL_PHYS_DELAY;
    ball_phys->move_y_every = BALL_PHYS_DELAY;
    ball_phys->move_x_last = inittime;
    ball_phys->move_y_last = inittime;
    ball_phys->bounce = 1;
    ball_phys->collided = NULL;
    ball_phys->callback = collision_callback;
    ball_phys->active = true;
    ball->sprite = SDL_CreateRGBSurface(0, 20, 20, 32, 0, 0, 0, 0);
    ball->pos_x = &ball_phys->pos_x;
    ball->pos_y = &ball_phys->pos_y;
    ball->size_x = 20;
    ball->size_y = 20;
    ball->active = &ball_phys->active;
    SDL_FillRect(ball->sprite, NULL, SDL_MapRGB(ball->sprite->format, 255, 255, 255));
    ball->texture = SDL_CreateTextureFromSurface(window->renderer, paddle->sprite);

    // Load images
    press_a_key->sprite = IMG_Load("press_a_key.png");
    press_a_key->texture = SDL_CreateTextureFromSurface(window->renderer, press_a_key->sprite);
    press_a_key->draw_pos_x = 294;
    press_a_key->draw_pos_y = 290;
    press_a_key->draw_active = false;
    press_a_key->pos_x = &press_a_key->draw_pos_x;
    press_a_key->pos_y = &press_a_key->draw_pos_y;
    press_a_key->size_x = 212;
    press_a_key->size_y = 20;
    press_a_key->active = &press_a_key->draw_active;

    game_over->sprite = IMG_Load("game_over.png");
    game_over->texture = SDL_CreateTextureFromSurface(window->renderer, game_over->sprite);
    game_over->draw_pos_x = 343;
    game_over->draw_pos_y = 290;
    game_over->draw_active = false;
    game_over->pos_x = &game_over->draw_pos_x;
    game_over->pos_y = &game_over->draw_pos_y;
    game_over->size_x = 115;
    game_over->size_y = 20;
    game_over->active = &game_over->draw_active;

    level_text->sprite = IMG_Load("level.png");
    level_text->texture = SDL_CreateTextureFromSurface(window->renderer, level_text->sprite);
    level_text->draw_pos_x = 718;
    level_text->draw_pos_y = 0;
    level_text->draw_active = true;
    level_text->pos_x = &level_text->draw_pos_x;
    level_text->pos_y = &level_text->draw_pos_y;
    level_text->size_x = 68;
    level_text->size_y = 20;
    level_text->active = &level_text->draw_active;

    lives_text->sprite = IMG_Load("lives.png");
    lives_text->texture = SDL_CreateTextureFromSurface(window->renderer, lives_text->sprite);
    lives_text->draw_pos_x = 634;
    lives_text->draw_pos_y = 0;
    lives_text->draw_active = true;
    lives_text->pos_x = &lives_text->draw_pos_x;
    lives_text->pos_y = &lives_text->draw_pos_y;
    lives_text->size_x = 66;
    lives_text->size_y = 20;
    lives_text->active = &lives_text->draw_active;

    for (int x = 0; x < MAX_LEVELS; x++) {
        sprintf(num_str, "%d", x+1);
        num_image[0] = num_str[0];

        level_num[x] = new graphics_obj;
        level_num[x]->sprite = IMG_Load(num_image);
        level_num[x]->texture = SDL_CreateTextureFromSurface(window->renderer, level_num[x]->sprite);
        level_num[x]->draw_pos_x = 788;
        level_num[x]->draw_pos_y = 0;
        level_num[x]->draw_active = false;
        level_num[x]->pos_x = &level_num[x]->draw_pos_x;
        level_num[x]->pos_y = &level_num[x]->draw_pos_y;
        level_num[x]->size_x = 12;
        level_num[x]->size_y = 20;
        level_num[x]->active = &level_num[x]->draw_active;

        window->add_object(level_num[x]);
    }

    for (int x = 0; x < MAX_LIVES; x++) {
        sprintf(num_str, "%d", x+1);
        num_image[0] = num_str[0];

        lives_num[x] = new graphics_obj;
        lives_num[x]->sprite = IMG_Load(num_image);
        lives_num[x]->texture = SDL_CreateTextureFromSurface(window->renderer, lives_num[x]->sprite);
        lives_num[x]->draw_pos_x = 702;
        lives_num[x]->draw_pos_y = 0;
        lives_num[x]->draw_active = false;
        lives_num[x]->pos_x = &lives_num[x]->draw_pos_x;
        lives_num[x]->pos_y = &lives_num[x]->draw_pos_y;
        lives_num[x]->size_x = 12;
        lives_num[x]->size_y = 20;
        lives_num[x]->active = &lives_num[x]->draw_active;

        window->add_object(lives_num[x]);
    }

    // Physics
    phys *physics = new phys(RES_X, RES_Y);

    for (int x = 0; x < NUM_BLOCKS; x++) {
        blocks_phys[x] = new phys_obj;
        physics->add_object(blocks_phys[x]);
        blocks[x] = new graphics_obj;
        blocks[x]->sprite = SDL_CreateRGBSurface(0, BLOCK_WIDTH, BLOCK_HEIGHT, 32, 0, 0, 0, 0);
        blocks[x]->texture = NULL;
        blocks[x]->size_x = BLOCK_WIDTH;
        blocks[x]->size_y = BLOCK_HEIGHT;
        window->add_object(blocks[x]);
    }

    physics->add_object(paddle_phys);
    physics->add_object(ball_phys);

    window->add_object(paddle);
    window->add_object(ball);
    window->add_object(press_a_key);
    window->add_object(game_over);
    window->add_object(level_text);
    window->add_object(lives_text);

    level_num[0]->draw_active = true;
    lives_num[lives-1]->draw_active = true;
    setup_blocks_level_0(window, blocks_phys, blocks);

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

        if (level_changed) {
            level_changed = false;
            switch (level) {
                case 0:
                    setup_blocks_level_0(window, blocks_phys, blocks);
                    break;
                case 1:
                    setup_blocks_level_1(window, blocks_phys, blocks);
                    break;
                case 2:
                    setup_blocks_level_2(window, blocks_phys, blocks);
                    break;
                default:
                    level = 0;

                    for (int x = 0; x < MAX_LEVELS; x++) {
                        level_num[x]->draw_active = false;
                    }

                    setup_blocks_level_0(window, blocks_phys, blocks);
            }

            level_num[level]->draw_active = true;

            if (level != 0) {
                level_num[level-1]->draw_active = false;
            }
        }

        if (!is_game_over) {
            if (life_lost) {
                life_lost = false;

                lives_num[lives-1]->draw_active = true;
                lives_num[lives]->draw_active = false;
            }

            if (!wait_for_input) {
                // Move paddle
                paddle_phys->step_x = 0;
                if (left) { paddle_phys->step_x = -1; }
                if (right) { paddle_phys->step_x = 1; }

                // Advance physics
                physics->advance();
            } else if (input_released && (left || right || up || down)) {
                // Reset ball direction
                ball_phys->step_x = 1;
                ball_phys->step_y = -1;
                ball_phys->move_x_every = BALL_PHYS_DELAY;
                wait_for_input = false;
                physics->reset_timings();
            } else if (!left && !right && !up && !down) {
                input_released = true;
            }
        }

        if (is_game_over) {
            game_over->draw_active = true;
        } else if (wait_for_input) {
            press_a_key->draw_active = true;
        } else {
            press_a_key->draw_active = false;
        }

        // Redraw screen
        window->draw();
    }

    SDL_Quit();

    delete physics;
    delete window;
    delete paddle_phys;
    delete paddle;
    delete ball_phys;
    delete ball;
    delete press_a_key;
    delete game_over;
    delete level_text;
    delete lives_text;

    for (int x = 0; x < NUM_BLOCKS; x++) {
        delete blocks_phys[x];
        delete blocks[x];
    }

    for (int x = 0; x < MAX_LEVELS; x++) {
        delete level_num[x];
    }

    for (int x = 0; x < MAX_LIVES; x++) {
        delete lives_num[x];
    }

    return;
}

int main (int argc, char *argv[])
{
    breakout();
    return 0;
}

