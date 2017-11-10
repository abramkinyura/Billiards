#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <stdio.h>

int main() {
    int i;
    int clocks_per_sec = sysconf(_SC_CLK_TCK);
    timeval dt;

    printf("CLK_TCK = %d\n", (int) clocks_per_sec);
    for (i = 0; i < 10; i++) {
        printf("times() = %d\n", (int) times(0));

        //... sleep(1);
        dt.tv_sec = 0;
        dt.tv_usec = 500000;    // 0.5 sec
        select(0, 0, 0, 0, &dt);
    }
    return 0;
}
