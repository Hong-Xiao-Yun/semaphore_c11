#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "threads.h"
#include "ringbuffer.h"
#include "thread.h"
void *active(void *aegp);
ring_buffer_t rb;
static int REASONABLE_THREAD_MAX=500;
int main (int argc, char** argv)
{
    int nhijos;
    threads** threadS;
    pthread_t *tid;

    ring_buffer_init(&rb, 16);

    if (argc > 1) {
        nhijos = atoi(argv[1]);
        if ((nhijos <= 0) || (nhijos > REASONABLE_THREAD_MAX)) {
            printf("invalid argument for thread count\n");
            exit(EXIT_FAILURE);
        }
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

    return 0;
}
void *active(void *aegp)
{
    int v=(rand()%100)+1;
    printf("rand number is %d\n",v);
    ring_push(&rb, (void*)(uintptr_t)v);
    int x = (int)(uintptr_t)ring_pop(&rb);
    printf("Got value: %d\n", x);
    pthread_exit(0);
}