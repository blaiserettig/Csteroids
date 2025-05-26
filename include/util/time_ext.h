
#ifndef TIME_EXT_H
#define TIME_EXT_H

typedef struct {
    double dt;
    double elapsed;
    float scale;
} global_time;

void init_time(global_time *time);
void update_time(global_time *time);


#endif //TIME_EXT_H
