#include<stdio.h>
#include<stdlib.h>
int main(){
    union{
        short s;
        char c[2];
    }un;
    un.s = 0x0102;
    if(un.c[0]==1 && un.c[1]==2)printf("big endian\n");
    else if(un.c[0]==2 && un.c[1]==1)printf("little endian\n");
    else printf("unknown\n");
    return 0;
}
