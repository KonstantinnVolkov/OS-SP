#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned long countSum(char *str);
int countDays(char *days, char *months, char *years);

int main(int argc, char *argv[]) {

    if (argc != 8) {
        printf("Wrong amount of parameters\n");
        return 0;
    }

    unsigned long s = 0;
    s = countSum(argv[1]) + countSum(argv[2]) + countSum(argv[3]);
    printf("Char sum: %d\n", s);

    unsigned long days = countDays(argv[4], argv[5], argv[6]);
    printf("days: %d\n", days);

    s *= days;
    printf("s*days = %lld\n", s);

    unsigned long copy = s;
    int len = 0;
    do {
        len++;
        copy /= 10;
    } while(copy > 0);
    int arr[len];
    int cpLen = len;
    copy = s;
    do {
        arr[--len] = copy % 10;
        copy /= 10;
    } while (copy > 0);
    int middle = arr[cpLen / 2 - 1] * 100 + arr[cpLen / 2] * 10 + arr[cpLen / 2 + 1];
    printf("middle: %d\n", middle);

    int n = (middle % atoi(argv[7])) + 1;
    printf("Result: %d\n", n);
    return 0;
}

unsigned  long countSum(char *str){
    unsigned long sum = 0;
    for (int i = 0; i < strlen(str); i += 2) {
        u_char char1 = str[i];
        u_char char2 = str[i + 1];
        uint tmp = (char1 << 8) + char2;
        sum += tmp*tmp;
        printf("%X\n", tmp);
    }
    printf("\n");
    return sum;
}

int countDays(char *days, char *months, char *years){
    int day = atoi(days);
    int month = atoi(months);
    int year = atoi(years);
    return day + 30*month + 365*year;
}


