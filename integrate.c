#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include "include/lualib.h"
#include "include/lauxlib.h"

#define MAX_THREAD_NUM  (1)
#define DIVISION_COUNT 1000

typedef struct segment_t {
    double start;
    double end;
    double interval;
    char*  lua_file;
} segment_t;

/* グローバル変数 (全スレッド共有) */
sem_t sem;
double sum = 0;

void init_segment(segment_t** segments, double start, double end, int thread_num, char* lua_file) {
    *segments = (segment_t*)malloc(sizeof(segment_t) * thread_num);
    int i;
    double interval = (end - start) / DIVISION_COUNT;
    double width    = (end - start) / thread_num;

    segment_t* array = *segments;
    for ( i = 0; i < thread_num; i++ ) {
        array[i].start    = ( i > 0 ) ? array[i - 1].end : start;
        array[i].end      = ( i < thread_num - 1 ) ? array[i].start + width : end;
        array[i].interval = interval;
        array[i].lua_file = lua_file;
    }
}

void thread_func(void *arg) {
    segment_t* segment = (segment_t*)arg;

    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    (void)luaL_dofile(L, segment->lua_file);

    double result = 0;
    double x;
    int i;

    for( i = 0, x = segment->start + segment->interval / 2; x < segment->end; i++, x += segment->interval) {
        lua_getglobal(L, "calc");
        lua_pushnumber(L, x);
        lua_call(L, 1, 1);
        double y = lua_tonumber(L, -1);
        result += y * segment->interval;
    }

    /* セマフォによる排他制御の開始と終了 */
    sem_wait(&sem);
    sum += result;
    sem_post(&sem);

    lua_close(L);
}

int main(int argc, char* argv[]) {
    int i;
    if( argc < 5 ) {
        printf("usage: integrate START END THREAD_NUM LUAFILE\n");
        return 0;
    }
    double start   = atof(argv[1]);
    double end     = atof(argv[2]);
    int thread_num = atoi(argv[3]);
    char* lua_file = argv[4];
    segment_t* segments;

    init_segment(&segments, start, end, thread_num, lua_file);
    pthread_t* handle = (pthread_t*)malloc(sizeof(pthread_t) * thread_num);
    /* セマフォ変数の初期化 */
    sem_init(&sem, 0, MAX_THREAD_NUM);

    for (i = 0; i < thread_num; ++i)
        pthread_create(&handle[i], NULL, (void *)thread_func, (void *)&segments[i]);

    for (i = 0; i < thread_num; ++i) 
        pthread_join(handle[i], NULL);

    printf("result: %.16f\n", sum);

    /* セマフォ変数の破棄 */
    sem_destroy(&sem);

    free(handle);
    free(segments);

    return 0;
}
