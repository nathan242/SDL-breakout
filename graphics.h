#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>

struct graphics_obj
{
    SDL_Surface *sprite;
    SDL_Texture *texture;

    int *pos_x;
    int *pos_y;
    int draw_pos_x;
    int draw_pos_y;

    int size_x;
    int size_y;

    bool *active;
    bool draw_active;
};

class graphics
{
    private:
        SDL_Rect offset;

        int list_len;

        struct obj_list
        {
            graphics_obj *obj;
            obj_list *next;
        };

        obj_list *list_head;
        obj_list *list_curr;

    public:
        SDL_Window *window;
        SDL_Renderer *renderer;

        graphics(const char* caption, int res_x, int res_y, int bpp);
        int add_object(graphics_obj *obj);
        void draw();
        ~graphics();
};

#endif

