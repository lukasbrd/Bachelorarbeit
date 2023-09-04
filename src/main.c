#include "communication.h"
#include "persistenceInterface.h"
#include "qthread.h"
#include "queue.h"
#include "state.h"
#include <czmq.h>
#include <stdio.h>
#include "settings.h"
#include <time.h>

int main(void) {
    struct timeval start_time, end_time;
    double elapsed_time;

    gettimeofday(&start_time, NULL);
    srand(0);
    for (int i = 1; i <= NUMBEROFSTATES; i++) {
        char *state = createState(i);
    }
    gettimeofday(&end_time, NULL);

    long seconds = end_time.tv_sec - start_time.tv_sec;
    long micros = end_time.tv_usec - (start_time.tv_usec);

    elapsed_time = (double)seconds + (double)micros / 1000000.0;

    printf("The elapsed time is %.6lf seconds\n", elapsed_time);

    return 0;
}
