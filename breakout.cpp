#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#define NUM_BLOCKS_X 8
#define NUM_BLOCKS_Y 5
#define BLOCK_WIDTH 80
#define BLOCK_HEIGHT 20
#define BLOCK_SPACE_X 4
#define BLOCK_SPACE_X_FIRST 66
#define BLOCK_SPACE_Y 4
#define BLOCK_SPACE_Y_FIRST 100

// Allocate surfaces
SDL_Surface *screen = NULL;
//SDL_Surface *numbers = NULL;
SDL_Surface *press_a_key = NULL;
//SDL_Surface *p2win = NULL;

// SDL Rect for positions of numbers in numbers.png
//SDL_Rect num[11];

// SDL event for handling input
SDL_Event input;

// SDL Rect for positioning sprites
SDL_Rect offset;

// Variables for player scores
//int scoreplayer1;
//int scoreplayer2;

bool wait_for_input = false;

struct phys_obj
{
    SDL_Surface *sprite;

    int pos_x;
    int pos_y;

    int size_x;
    int size_y;

    int step_x;
    int step_y;

    int delay;
    int delay_counter;

    int bounce;
    phys_obj *collided;

    void (*callback)(phys_obj *obj, phys_obj *obj2, int collide_axis, int area_x, int area_y);

    bool active;
};

class phys
{
    private:
        int area_x;
        int area_y;

        int list_len;

        struct obj_list
        {
            int id;
            phys_obj *obj;
            obj_list *next;
        };
        obj_list *list_head;

        void check_collide(phys_obj *obj, int id);
    public:
        phys(int x, int y);
        int add_object(phys_obj *obj);
        void advance();
        ~phys();
};

phys::phys(int x, int y)
{
    area_x = x;
    area_y = y;

    list_len = 0;

    list_head = NULL;
}

int phys::add_object(phys_obj *obj)
{
    obj_list *list = new obj_list;
    list->id = list_len;
    list->obj = obj;
    list->next = NULL;

    // First
    if (list_head == NULL) {
        list_head = list;
    } else {
        // Append to list
        obj_list *list_item = list_head;
        while (list_item->next != NULL) {
            list_item = list_item->next;
        }
        list_item->next = list;
    }

    return ++list_len;
}

void phys::advance()
{
    obj_list *list = NULL;
    phys_obj *obj = NULL;

    list = list_head;

    while (list != NULL) {
        // Get object
        obj = list->obj;

        if (obj->active) {
            // Check if object is colliding with another
            check_collide(obj, list->id);

            // Move object
            if (obj->delay_counter == obj->delay) {
                obj->pos_x += obj->step_x;
                obj->pos_y += obj->step_y;

                obj->delay_counter = 0;
            } else {
                obj->delay_counter++;
            }
        }

        // Get next
        list = list->next;
    }
    
}

void phys::check_collide(phys_obj *obj, int id)
{
    obj_list *list = NULL;
    phys_obj *obj2 = NULL;

    int x1;
    int x2;
    int y1;
    int y2;

    int diff_x;
    int diff_y;

    list = list_head;

    // Check collision with other objects
    while (list != NULL) {
        if (list->id != id && list->obj->active) {
            obj2 = list->obj;

            // Left side
            x1 = obj2->pos_x-obj->size_x;
            // Right side
            x2 = obj2->pos_x+obj2->size_x;
            // Top side
            y1 = obj2->pos_y-obj->size_y;
            // Bottom side
            y2 = obj2->pos_y+obj2->size_y;

            if (obj->pos_x >= x1 && obj->pos_x <= x2 && obj->pos_y >= y1 && obj->pos_y <= y2) {
                if (obj->collided != obj2) {
                    if (obj->pos_x-x1 > x2-obj->pos_x) {
                        // Collided on right side
                        diff_x = x2-obj->pos_x;
                    } else {
                        // Collided on left side
                        diff_x = obj->pos_x-x1;
                    }

                    if (obj->pos_y-y1 > y2-obj->pos_y) {
                        // Collided on bottom side
                        diff_y = y2-obj->pos_y;
                    } else {
                        // Collided on top side
                        diff_y = obj->pos_y-y1;
                    }

                    if (diff_y > diff_x) {
                        if (obj->callback != NULL) { obj->callback(obj, obj2, 1, area_x, area_y); }
                        if (obj->bounce > 0) {
                            if ((obj->step_x > 0 && obj->pos_x < obj2->pos_x) || (obj->step_x < 0 && obj->pos_x > obj2->pos_x)) {
                                obj->step_x = obj->step_x*-1;
                            }
                        } else {
                            obj->step_x = 0;
                            obj->step_y = 0;
                        }
                    } else {
                        if (obj->callback != NULL) { obj->callback(obj, obj2, 2, area_x, area_y); }
                        if (obj->bounce > 0) {
                            if ((obj->step_y > 0 && obj->pos_y < obj2->pos_y) || (obj->step_y < 0 && obj->pos_y > obj2->pos_y)) {
                                obj->step_y = obj->step_y*-1;
                            }
                        } else {
                            obj->step_x = 0;
                            obj->step_y = 0;
                        }
                    }
                    obj->collided = obj2;
                }
            } else if (obj->collided == obj2) {
                obj->collided = NULL;
            }
        }

        list = list->next;
    }

    // Check collision with edges
    if (obj->pos_x >= area_x-obj->size_x || obj->pos_x <= 0) { 
        if (obj->callback != NULL) { obj->callback(obj, NULL, 0, area_x, area_y); }
        if (obj->bounce > 0) {
            obj->step_x = obj->step_x*-1;
        } else if (obj->pos_x >= area_x-obj->size_x && obj->step_x > 0 || obj->pos_x <= 0 && obj->step_x < 0) {
            obj->step_x = 0;
            obj->step_y = 0;
        }
    }
    if (obj->pos_y >= area_y-obj->size_y || obj->pos_y <= 0) {
        if (obj->callback != NULL) { obj->callback(obj, NULL, 0, area_x, area_y); }
        if (obj->bounce > 0) {
            obj->step_y = obj->step_y*-1;
        } else if (obj->pos_y >= area_y-obj->size_y && obj->step_y > 0 || obj->pos_y <= 0 && obj->step_y < 0) {
            obj->step_x = 0;
            obj->step_y = 0;
        }
    }
}

phys::~phys()
{
    if (list_head != NULL) {
        obj_list *list = NULL;
        obj_list *prev = NULL;
        phys_obj *obj = NULL;

        list = list_head;

        while (list != NULL) {
            obj = list->obj;
            delete obj;
            prev = list;
            list = list->next;
            delete prev;
        }
    }
}

void collision_callback(phys_obj *obj, phys_obj *obj2, int collide_axis, int area_x, int area_y)
{
    if (obj2 == NULL) {
        if (obj->pos_y+obj->size_y >= area_y) {
            obj->pos_x = 100;
            obj->pos_y = 500;
            wait_for_input = true;
        }
    } else if (obj2 != NULL && obj2->pos_y < 500) {
        obj2->active = false;
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

    // Constant variables
    const int res_x = 800;
    const int res_y = 600;
    const int bpp = 32;

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

    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);
    screen = SDL_SetVideoMode(res_x, res_y, bpp, SDL_HWSURFACE);
    SDL_WM_SetCaption("SDL BREAKOUT", NULL);

    // Set screen clearing colour
    Uint32 clear_colour = SDL_MapRGB(screen->format, 0, 0, 0);

    // Physics objects
    phys_obj *paddle = new phys_obj;
    phys_obj *ball = new phys_obj;
    phys_obj *blocks[NUM_BLOCKS_Y][NUM_BLOCKS_X];

    int block_pos_x = BLOCK_SPACE_X_FIRST;
    int block_pos_y = BLOCK_SPACE_Y_FIRST;

    const int row_colours_r[NUM_BLOCKS_Y] = {0, 51, 102, 153, 204};
    const int row_colours_g[NUM_BLOCKS_Y] = {0, 51, 102, 153, 204};
    const int row_colours_b[NUM_BLOCKS_Y] = {255, 255, 255, 255, 255};

    // Paddle
    paddle->sprite = SDL_CreateRGBSurface(0, 100, 20, 32, 0, 0, 0, 0);
    paddle->pos_x = 50;
    paddle->pos_y = 550;
    paddle->size_x = 100;
    paddle->size_y = 20;
    paddle->step_x = 0;
    paddle->step_y = 0;
    paddle->delay = 0;
    paddle->delay_counter = 0;
    paddle->bounce = 0;
    paddle->collided = NULL;
    paddle->callback = NULL;
    paddle->active = true;
    SDL_FillRect(paddle->sprite, NULL, SDL_MapRGB(screen->format, 255, 255, 255));

    // Ball
    ball->sprite = SDL_CreateRGBSurface(0, 20, 20, 32, 0, 0, 0, 0);
    ball->pos_x = 100;
    ball->pos_y = 500;
    ball->size_x = 20;
    ball->size_y = 20;
    ball->step_x = 1;
    ball->step_y = -1;
    ball->delay = 0;
    ball->delay_counter = 0;
    ball->bounce = 1;
    ball->collided = NULL;
    ball->callback = collision_callback;
    ball->active = true;
    SDL_FillRect(ball->sprite, NULL, SDL_MapRGB(screen->format, 255, 255, 255));


    // Load images
    //numbers = SDL_DisplayFormat(IMG_Load("numbers.png"));
    press_a_key = SDL_DisplayFormat(IMG_Load("press_a_key.png"));
    //p2win = SDL_DisplayFormat(IMG_Load("p2win.png"));

    // Physics
    phys *physics = new phys(res_x, res_y);
    physics->add_object(paddle);
    physics->add_object(ball);

    for (int y = 0; y < NUM_BLOCKS_Y; y++) {
        for (int x = 0; x < NUM_BLOCKS_X; x++) {
            // Blocks
            blocks[y][x] = new phys_obj;

            blocks[y][x]->sprite = SDL_CreateRGBSurface(0, BLOCK_WIDTH, BLOCK_HEIGHT, 32, 0, 0, 0, 0);
            blocks[y][x]->pos_x = block_pos_x;
            blocks[y][x]->pos_y = block_pos_y;
            blocks[y][x]->size_x = BLOCK_WIDTH;
            blocks[y][x]->size_y = BLOCK_HEIGHT;
            blocks[y][x]->step_x = 0;
            blocks[y][x]->step_y = 0;
            blocks[y][x]->delay = 0;
            blocks[y][x]->delay_counter = 0;
            blocks[y][x]->bounce = 0;
            blocks[y][x]->collided = NULL;
            blocks[y][x]->callback = NULL;
            blocks[y][x]->active = true;
            SDL_FillRect(blocks[y][x]->sprite, NULL, SDL_MapRGB(screen->format, row_colours_r[y], row_colours_g[y], row_colours_b[y]));

            physics->add_object(blocks[y][x]);

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
            paddle->step_x = 0;
            if (left) { paddle->step_x = -1; }
            if (right) { paddle->step_x = 1; }

            // Advance physics
            physics->advance();
        } else {
            if (left || right || up || down) {
                wait_for_input = false;
            }
        }

        // Redraw screen
        // Clear screen
        SDL_FillRect(screen, NULL, clear_colour);

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

        if (wait_for_input) {
            offset.x = 306;
            offset.y = 290;
            SDL_BlitSurface(press_a_key, NULL, screen, &offset);
        }

        // Ball
        offset.x = ball->pos_x;
        offset.y = ball->pos_y;
        SDL_BlitSurface(ball->sprite, NULL, screen, &offset);

        // Paddle
        offset.x = paddle->pos_x;
        offset.y = paddle->pos_y;
        SDL_BlitSurface(paddle->sprite, NULL, screen, &offset);

        for (int y = 0; y < NUM_BLOCKS_Y; y++) {
            for (int x = 0; x < NUM_BLOCKS_X; x++) {
                if (blocks[y][x]->active) {
                    offset.x = blocks[y][x]->pos_x;
                    offset.y = blocks[y][x]->pos_y;
                    SDL_BlitSurface(blocks[y][x]->sprite, NULL, screen, &offset);
                }
            }
        }

        // Flip screen
        SDL_Flip(screen);
        SDL_Delay(2);
    }

    delete physics;

    return;
}

int main (int argc, char *argv[])
{
    breakout();
    return 0;
}

