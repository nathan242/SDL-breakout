#ifndef PHYSICS_H
#define PHYSICS_H

#include <cstddef>
#include <cstdint>
#include <time.h>

struct phys_obj
{
    int type_id;

    int pos_x;
    int pos_y;

    int size_x;
    int size_y;

    int step_x;
    int step_y;

    time_t move_x_every;
    time_t move_y_every;
    timespec move_x_last;
    timespec move_y_last;

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
        obj_list *list_curr;

        void check_collide(phys_obj *obj, int id);
    public:
        phys(int x, int y);
        int add_object(phys_obj *obj);
        void advance();
        void reset_timings();
        ~phys();
};

#endif

