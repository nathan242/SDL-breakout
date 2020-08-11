#ifndef RENDER_H
#define RENDER_H

#include "physics.h"
#include <SDL/SDL.h>

struct game_obj
{
    SDL_Surface *sprite;
    phys_obj *phys;

    int *pos_x;
    int *pos_y;
    int draw_pos_x;
    int draw_pos_y;

    bool *active;
    bool draw_active;
};

class render
{
    private:
        SDL_Rect offset;
        Uint32 clear_colour;

        int list_len;

        struct obj_list
        {
            game_obj *obj;
            obj_list *next;
        };
        obj_list *list_head;

    public:
        SDL_Surface *screen;

        render(const char* caption, int res_x, int res_y, int bpp);
        int add_object(game_obj *obj);
        void draw(int delay);
        ~render();
};

#endif

