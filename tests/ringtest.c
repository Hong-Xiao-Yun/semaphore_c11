#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "threads.h"
#include "ringbuffer.h"
#include "thread.h"
void *active(void *aegp);
ring_buffer_t rb;

static double diff_in_second(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}
double sumrelease=0.0;
int main (int argc, char** argv)
{
    int nhijos;
    threads** threadS;
    pthread_t *tid;

    ring_buffer_init(&rb, 16);

    if (argc > 1) {
        nhijos = atoi(argv[1]);
      
        threadS=(threads**)malloc(sizeof(threads*)*nhijos);
        tid=(pthread_t*)malloc(sizeof(pthread_t)*nhijos);

        for (int i = 0; i < nhijos; ++i) {
            threadS[i] = (threads *)malloc(sizeof (threads));
            if(threadS[i]!=NULL) {
                threadS[i] ->no=0;
                threadS[i] ->tid=i;
            } else {
                printf("threadS=NULL\n");
                exit(1);
            }
        }

        for (int i = 0; i < nhijos; i++) {
            pthread_create(&tid[i], NULL, &active, threadS[i] );
        }
        for (int i = 0; i < nhijos; i++) {
            pthread_join( tid[i], NULL );
        }

    }
    ring_buffer_destroy(&rb);
    printf("average  time=%1.3f ms\n",sumrelease/nhijos);
    return 0;
}
void *active(void *aegp)
{
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    int v=(rand()%100)+1;
    printf("rand number is %d\n",v);
    ring_push(&rb, (void*)(uintptr_t)v);
    int x = (int)(uintptr_t)ring_pop(&rb);
    clock_gettime(CLOCK_REALTIME, &end);
    sumrelease+=diff_in_second(start, end);
    printf("Got value: %d\n", x);
    pthread_exit(0);
}