// taptempo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TEMPO_JITTER 0.1

#define BUF_LEN 4
typedef struct {
    int interval[BUF_LEN];
    int wp;
    int count;
}RING;

RING buff;

void put_data(int d) {
    buff.interval[buff.wp++] = d;
    buff.count++;
    if (buff.wp >= BUF_LEN) {
        buff.wp = 0;
    }
    if (buff.count >= BUF_LEN) {
        buff.count = BUF_LEN;
    }
}

int sum_data() {
    int i, sum = 0;
    for(i=0;i< BUF_LEN;i++)
        sum+=buff.interval[i];
    return sum;
}

#define K 0.03 // 0~1.0

double exp_filter(double z) {
    static double x = z;

    x = z * (1.0 - K) + x * K;

    return x;
}

void buff_init() {
    int i;
    for(i=0;i<BUF_LEN;i++)
        buff.interval[i] = 0;
    buff.wp = buff.count = 0;
}

int tap_tempo(int time, int tap_count) {
    static int last_interval;
    int bpm = 0;
    int interval = 0;

    if (tap_count == 1)
        last_interval = time;

    if ((time > 2 * (1 - TEMPO_JITTER) * last_interval) && (time < 2 * (1 + TEMPO_JITTER) * last_interval)) {
        interval = last_interval;
        last_interval = time;
    }
    else if ((time > 3 * (1 - TEMPO_JITTER) * last_interval) && (time < 3 * (1 + TEMPO_JITTER) * last_interval)) {
        interval = last_interval;
        last_interval = time;
    }
    else {
        last_interval = interval = time;
    }

    double z = CLOCKS_PER_SEC * 60 / interval;



    bpm = (int)(exp_filter(z) + 0.5);


    put_data(bpm);
    bpm = sum_data() / buff.count;

    return bpm;
}

int main(void) {
    int taps = 0;
    int i,ch;
    int bpm = 0;
    int last_tap = 0, tmp;

    printf("Tap the beat!\n");
    buff_init();
    while (1) {
        ch = getchar();
        switch (ch) {
        case 'q':
            break;
        case '\n':
 
            if (taps == 0) {
                last_tap = clock();
                break;
            }
            tmp = clock() - last_tap;
            last_tap = clock();
            bpm = tap_tempo(tmp, taps);
            break;
        }
        taps++;
        printf("Last BPM: %d\n", bpm);
    }

    return EXIT_SUCCESS;
}
