#ifndef PHYSICS_H
#define PHYSICS_H

#include <cstddef>

struct phys_obj
{
    int type_id;

    int pos_x;
    int pos_y;

    int size_x;
    int size_y;

    int step_x;
    int step_y;
    int step_x_delay;
    int step_y_delay;
    int step_x_delay_count;
    int step_y_delay_count;

    int delay;
    int delay_count;

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

#endif

