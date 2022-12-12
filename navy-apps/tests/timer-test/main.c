#include <stdio.h>
#include <unistd.h>
int main(){
    int cnt = 0;
    while(++cnt){
        printf("Hello world from time %d\n", cnt);
        sleep(1);
    }
    return 0;
}