#include "graphics.h"

graphics::graphics(const char* caption, int res_x, int res_y, int bpp)
{
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow(caption, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, res_x, res_y, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    list_len = 0;
    list_head = NULL;

    ticks_last_draw = SDL_GetTicks();
}

int graphics::add_object(graphics_obj *obj)
{
    obj_list *list = new obj_list;
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

void graphics::draw(int delay) {
    obj_list *list = NULL;
    graphics_obj *obj = NULL;

    int next_delay = delay - (SDL_GetTicks() - ticks_last_draw);
    if (next_delay > 0) {
        SDL_Delay(next_delay);
    }

    list = list_head;

    // Clear screen
    // SDL_FillRect(screen, NULL, clear_colour);
    SDL_RenderClear(renderer);

    while (list != NULL) {
        // Get object
        obj = list->obj;

        if (*obj->active) {
            offset.x = *obj->pos_x;
            offset.y = *obj->pos_y;
            offset.w = obj->size_x;
            offset.h = obj->size_y;

            // SDL_BlitSurface(obj->sprite, NULL, screen, &offset);
            SDL_RenderCopy(renderer, obj->texture, NULL, &offset);
        }

        // Get next
        list = list->next;
    }

    // SDL_Flip(screen);
    SDL_RenderPresent(renderer);
    ticks_last_draw = SDL_GetTicks();
}

graphics::~graphics()
{
    if (list_head != NULL) {
        obj_list *list = NULL;
        obj_list *prev = NULL;

        list = list_head;

        while (list != NULL) {
            prev = list;
            list = list->next;
            delete prev;
        }
    }
}

