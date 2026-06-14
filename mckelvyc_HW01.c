#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>

    //Problem 1 sumOfDigits
    int sumOfDigits(int n){
        if (n <= 0) {
            return -1;
        }
        int sum = 0;
        
        while (n > 0){
            int nums = n % 10;
            sum = sum + nums;
            n = n/10;
        }
        return sum;
    }

    //Problem 2 UABMaxMinDiff
    int UABMaxMinDiff(int arr[],int size){
        int max = arr[0];
        int min = arr[0];

        for (int i = 1; i < size; i++) {
            if (arr[i] > max) {
                max = arr[i];
            }
            if (arr[i] < min) {
                min = arr[i];
            }
        }
        return max - min;
    }

    //Problem 3 replaceEvenWithZero
    int* replaceEvenWithZero(int arr[], int size) {
    int* newArr = malloc(size * sizeof(int));

    for (int i = 0; i < size; i++) {
        if (arr[i] % 2 == 0) {
            newArr[i] = 0;
        } else {
            newArr[i] = arr[i];
        }
    }

    return newArr;
    }

    //Problem 4 perfectSquare
    bool perfectSquare(int n){
        if (n < 0){
            return false;
        }
        for (int i = 0; i * i <= n; i++) {
            if (i * i == n) {
                return true;
            }
       } 
       return false;
    }

    //Problem 5 countVowels
    int countVowels(char s[]){  
        int count = 0;

        for (int i = 0; s[i] != '\0'; i++){
            char c = tolower(s[i]);

            if (c =='a' || c == 'e' || c == 'i' || c == 'o' || c == 'u') {
                count++;
            }
        }
        return count;
    }

    int main() {
        //Problem 1 print
        printf("Sum of the digits in 999: %d\n", sumOfDigits(999));
        printf("Sum of the digits in 123: %d\n", sumOfDigits(123));
        printf("Sum of the digits in 1000: %d\n", sumOfDigits(1000));
        printf("Sum of the digits in 246: %d\n", sumOfDigits(246));
        printf("Sum of the digits in 8906: %d\n", sumOfDigits(8906));

        //Problem 2 print
        int arr1[] = {6,3,8,1,9};
        printf("Difference between max and min number: %d\n", UABMaxMinDiff(arr1, 5));
        int arr2[] = {1,6,3,8,2};
        int* result = replaceEvenWithZero(arr2, 5);

        //Problem 3 print
        printf("Even numbers replaced with 0: ");
        for (int i = 0; i < 5; i++) {
            printf("%d ", result[i]);
        }
        printf("\n");
        free(result);

        //Problem 4 print
        printf("Perfect Square of 36: %s\n", perfectSquare(36) ? "True" : "False");
        printf("Perfect Square of 15: %s\n", perfectSquare(15) ? "True" : "False");
        printf("Perfect Square of 25: %s\n", perfectSquare(25) ? "True" : "False");
        printf("Perfect Square of 16: %s\n", perfectSquare(16) ? "True" : "False");

        //Problem 5 print
        printf("Hello world has: %d vowels.\n", countVowels("Hello World"));
        printf("UAB CS has: %d vowels.\n", countVowels("UAB CS"));
        printf("Alabama has: %d vowels.\n", countVowels("Alabama"));
        printf("aeiou has: %d vowels.\n", countVowels("aeiou"));

        return 0;
    }