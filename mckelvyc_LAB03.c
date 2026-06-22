#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
To compile:
gcc mckelvyc_LAB03.c -o lab3

To run:
./lab3
*/

void readStrings(char *arr[], int size);
void sortStrings(char *arr[], int size);
void displayStrings(char *arr[], int size);
void freeStrings (char *arr[], int size);

int main(void) {
    int n;
    char **arr;


    printf("Please enter the number of strings: ");
    scanf("%d", &n);

    arr = (char **) malloc (n * sizeof(char *));

    if (arr == NULL){
        printf("Memory allocation failed! \n");
        return 1;
    }

    readStrings(arr,n);
    sortStrings(arr,n);
    printf("Sorted stirngs: \n");
    displayStrings(arr,n);
    freeStrings(arr, n);
    free(arr);

    return 0;
}

void readStrings(char *arr[], int size) {
    char temp[100];

    for(int i = 0; i < size; i++){
        printf("Please enter the stirng %d: ", i+1);
        scanf("%99s", temp);

        arr[i] = (char*) malloc((strlen(temp) +1) *sizeof(char));
        if(arr[i] == NULL) {
            printf("Memory allocaion has failed!\n");
            return;
        }
        strcpy(arr[i], temp); 
    }
}


void sortStrings(char *arr[], int size) {
    int i;
    int j;
    char *key;

    for (i =1; i< size;i++){
        key = arr[i];
        j = i-1;

        while (j >= 0 && strcmp(arr[j], key)>0) {
            arr[j+1] = arr[j];
            j--;
        }
        arr[j+1] = key;
    }
}

void displayStrings(char *arr[], int size){
    int i;

    printf("[");
    for(i =0; i < size; i ++) {
        printf("%s", arr[i]);
        if(i < size -1) {
            printf(", ");
        }
    }
    printf("]\n");
}

void freeStrings(char *arr[], int size) {
    int i ;
    for(i = 0; i<size; i++){
        free(arr[i]);
    }
}