#include <stdio.h>
/* 
To compile use: gcc –o lab lab1.c
To execute use: ./lab1
*/

int main(void){
    int given_number;
    scanf("%d", &given_number);
    if (given_number>1){
        for(int i=2; i*i<=given_number; i++){
            if(given_number %i == 0){
                printf("The number is not prime");
                return 0;
            }
        }
        printf("The number is prime");
    }
    else {
        printf("The number is not prime");
    }
    return 0;
}