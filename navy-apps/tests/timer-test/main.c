#include <stdio.h>
#include <sys/time.h>
#include <NDL.h>

int main(){
    NDL_Init(0);
    int cnt = 0;
    uint32_t timer = NDL_GetTicks();
    while(1){
        uint32_t new_timer = NDL_GetTicks();
        if(new_timer - timer >= 1000) {
        printf("Hello World from %d\n", cnt);
        cnt ++;
        timer = new_timer;
       } 
    }
    return 0;
}