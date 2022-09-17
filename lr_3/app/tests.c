#define _GNU_SOURCE

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

void forkexample()
{
    pid_t pid;
    // child process because return value zero
    if ((pid = fork()) == 0) {
        printf("child %d\n", getpid());
        printf("child pid %d\n", pid);
        printf("Hello from Child!\n");
    }
        // parent process because return value non-zero.
    else {
        printf("parent %d\n", getpid());
        printf("parent pid %d\n", pid);
        printf("Hello from Parent!\n");
    }
}
int main()
{
    forkexample();
    return 0;
}