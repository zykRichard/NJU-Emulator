#include <stdio.h>
#include <sys/time.h>
int main(){
    int cnt = 0;
    struct timeval tv1;
    struct timeval tv2;
    gettimeofday(&tv1, NULL);
    while(1){
        gettimeofday(&tv2, NULL);
        if(tv2.tv_sec - tv1.tv_sec >= 1){
            tv1 = tv2;
            printf("Hello world from %d\n", cnt);
            cnt ++;
        }
    }
    return 0;
}