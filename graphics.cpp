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
    list_curr = NULL;
}

int graphics::add_object(graphics_obj *obj)
{
    obj_list *list = new obj_list;
    list->obj = obj;
    list->next = NULL;

    // First item
    if (list_head == NULL) {
        list_head = list;
        list_curr = list;
    } else {
        // Append to list
        list_curr->next = list;
        list_curr = list;
    }

    return ++list_len;
}

void graphics::draw() {
    obj_list *list = NULL;
    graphics_obj *obj = NULL;

    list = list_head;

    SDL_RenderClear(renderer);

    while (list != NULL) {
        // Get object
        obj = list->obj;

        if (*obj->active) {
            offset.x = *obj->pos_x;
            offset.y = *obj->pos_y;
            offset.w = obj->size_x;
            offset.h = obj->size_y;

            SDL_RenderCopy(renderer, obj->texture, NULL, &offset);
        }

        // Get next
        list = list->next;
    }

    SDL_RenderPresent(renderer);
}

graphics::~graphics()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

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

