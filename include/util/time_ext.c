#include "time_ext.h"

#include <stdio.h>
#include <sys/time.h>
#include <_timeval.h>

struct timeval last_frame, current_frame;

void init_time(global_time *time) {
    time->dt = 0;
    time->elapsed = 0;
    time->scale = 1.0f;
    gettimeofday(&last_frame, NULL);
}

void update_time(global_time *time) {
    gettimeofday(&current_frame, NULL);
    const double elapsed_sec = current_frame.tv_sec - last_frame.tv_sec;
    const double elapsed_usec = current_frame.tv_usec - last_frame.tv_usec;
    time->elapsed += elapsed_sec * 1000.0 + elapsed_usec / 1000.0;

    // Calculate delta time in milliseconds
    const double dt_sec = current_frame.tv_sec - last_frame.tv_sec;
    const double dt_usec = current_frame.tv_usec - last_frame.tv_usec;
    time->dt = dt_sec * 1000.0 + dt_usec / 1000.0;

    last_frame = current_frame;

    printf("Total elapsed time: %f \n",  time->elapsed);
    printf("Delta time: %f \n", time->dt);
}