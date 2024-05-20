#include "physics.h"

phys::phys(int x, int y)
{
    area_x = x;
    area_y = y;

    list_len = 0;
    list_head = NULL;
    list_curr = NULL;
}

int phys::add_object(phys_obj *obj)
{
    obj_list *list = new obj_list;
    list->id = list_len;
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

void phys::advance()
{
    obj_list *list = NULL;
    phys_obj *obj = NULL;
    timespec now;
    uint64_t timediff;
    int iterations;
    bool run_loop = true;

    clock_gettime(CLOCK_MONOTONIC, &now);

    while (run_loop) {
        run_loop = false;

        list = list_head;

        while (list != NULL) {
            // Get object
            obj = list->obj;

            if (obj->active) {
                // Check if object is colliding with another
                check_collide(obj, list->id);
            }

            // Get next
            list = list->next;
        }

        list = list_head;

        while (list != NULL) {
            // Get object
            obj = list->obj;

            if (obj->active) {
                if (obj->move_x_every == 0) {
                    iterations = 0;
                } else if (obj->move_x_last.tv_sec == 0) {
                    iterations = 1;
                } else {
                    timediff = ((now.tv_sec - obj->move_x_last.tv_sec) * 1000000000) + (now.tv_nsec - obj->move_x_last.tv_nsec);
                    iterations = timediff / obj->move_x_every;
                }

                if (iterations > 0) {
                    obj->pos_x += obj->step_x;

                    if (iterations > 1) {
                        run_loop = true;

                        obj->move_x_last.tv_nsec += obj->move_x_every;

                        if (obj->move_x_last.tv_nsec > 1000000000) {
                            obj->move_x_last.tv_sec++;
                            obj->move_x_last.tv_nsec -= 1000000000;
                        }
                    } else {
                        obj->move_x_last = now;
                    }
                }

                if (obj->move_y_every == 0) {
                    iterations = 0;
                } else if (obj->move_y_last.tv_sec == 0) {
                    iterations = 1;
                } else {
                    timediff = ((now.tv_sec - obj->move_y_last.tv_sec) * 1000000000) + (now.tv_nsec - obj->move_y_last.tv_nsec);
                    iterations = timediff / obj->move_y_every;
                }

                if (iterations > 0) {
                    obj->pos_y += obj->step_y;

                    if (iterations > 1) {
                        run_loop = true;

                        obj->move_y_last.tv_nsec += obj->move_y_every;

                        if (obj->move_y_last.tv_nsec > 1000000000) {
                            obj->move_y_last.tv_sec++;
                            obj->move_y_last.tv_nsec -= 1000000000;
                        }
                    } else {
                        obj->move_y_last = now;
                    }
                }
            }

            // Get next
            list = list->next;
        }
    }
}

void phys::reset_timings()
{
    obj_list *list = list_head;
    timespec inittime {0, 0};

    while (list != NULL) {
        list->obj->move_x_last = inittime;
        list->obj->move_y_last = inittime;

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
                        } else if (obj->bounce == 0) {
                            obj->step_x = 0;
                            obj->step_y = 0;
                        }
                    } else {
                        if (obj->callback != NULL) { obj->callback(obj, obj2, 2, area_x, area_y); }
                        if (obj->bounce > 0) {
                            if ((obj->step_y > 0 && obj->pos_y < obj2->pos_y) || (obj->step_y < 0 && obj->pos_y > obj2->pos_y)) {
                                obj->step_y = obj->step_y*-1;
                            }
                        } else if (obj->bounce == 0) {
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
    if ((obj->pos_x >= area_x-obj->size_x && obj->step_x > 0) || (obj->pos_x <= 0 && obj->step_x < 0)) {
        if (obj->callback != NULL) { obj->callback(obj, NULL, 1, area_x, area_y); }
        if (obj->bounce > 0) {
            obj->step_x = obj->step_x*-1;
        } else if (obj->bounce == 0 && (obj->pos_x >= area_x-obj->size_x && obj->step_x > 0 || obj->pos_x <= 0 && obj->step_x < 0)) {
            obj->step_x = 0;
            obj->step_y = 0;
        }
    }
    if ((obj->pos_y >= area_y-obj->size_y && obj->step_y > 0) || (obj->pos_y <= 0 && obj->step_y < 0)) {
        if (obj->callback != NULL) { obj->callback(obj, NULL, 2, area_x, area_y); }
        if (obj->bounce > 0) {
            obj->step_y = obj->step_y*-1;
        } else if (obj->bounce == 0 && (obj->pos_y >= area_y-obj->size_y && obj->step_y > 0 || obj->pos_y <= 0 && obj->step_y < 0)) {
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

        list = list_head;

        while (list != NULL) {
            prev = list;
            list = list->next;
            delete prev;
        }
    }
}

