#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main(void){
    FILE *fp;
    unsigned char a;
    int i = 0;
    fp = fopen("test.m7", "rb");
    while(fscanf(fp, "%c", &a) != EOF){
        printf("%d ", a);
        i++;
    }
    printf("Total is %d\n", i);
    return 0;
}
