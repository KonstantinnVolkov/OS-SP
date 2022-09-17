#include <stdlib.h>
#include <wait.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <stdio.h>
#include <libgen.h>
#include <fcntl.h>

pid_t zero_pid;
int process_number;
char *FName;

#define CounterOfExit 5000

#define PID_1 "/tmp/file_pid_process_1.txt"
#define PID_2 "/tmp/file_pid_process_2.txt"
#define PID_3 "/tmp/file_pid_process_3.txt"
#define PID_4 "/tmp/file_pid_process_4.txt"
#define PID_5 "/tmp/file_pid_process_5.txt"
#define PID_6 "/tmp/file_pid_process_6.txt"
#define PID_7 "/tmp/file_pid_process_7.txt"
#define PID_8 "/tmp/file_pid_process_8.txt"

const char *Pids[] = {PID_1, PID_2, PID_3, PID_4, PID_5,PID_6, PID_7, PID_8};

int sigcount1 = 0;
int sigusrcount2 = 0;
int sigusrcount3 = 0;

void createHandler1();
void createHandler2();
void createHandler3();
void createHandler4();
void createHandler5();
void createHandler6();
void createHandler7();
void createHandler8();

void __sigaction(int sig, void (*handler)(int))
{
    struct sigaction action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(sig, &action, NULL);
}

pid_t read_pid_f(const char *path) {
    FILE *file = fopen(path, "r+");
    pid_t cur_pid = getpid();
    if (!file) {fprintf(stderr, "%d %s: %s: %s\n", cur_pid, FName, strerror(errno), path); exit(EXIT_FAILURE);}

    char* buf = malloc(sizeof(long)*8);
    if (!fgets(buf, sizeof(long)*8, file)) {fprintf(stderr, "%d %s: %s: %s\n", cur_pid, FName, strerror(errno), path); exit(EXIT_FAILURE);}

    pid_t pid = (pid_t) strtol(buf, NULL, 10);

    if (fclose(file)) {fprintf(stderr, "%d %s: %s: %s\n", cur_pid, FName, strerror(errno), path); exit(EXIT_FAILURE);}
    return pid;
}

void write_pid_f(const char *path, pid_t current_pid) {
    FILE *file = fopen(path, "w+");
    if (!file) {fprintf(stderr, "%d %s: %s: %s\n", current_pid, FName, strerror(errno), path);exit(EXIT_FAILURE);}
    if (fprintf(file, "%d", getpid())<0) {fprintf(stderr, "%d %s: %s: %s\n", current_pid, FName, strerror(errno), path);exit(EXIT_FAILURE);}
    if (fclose(file)) {fprintf(stderr, "%d %s: %s: %s\n", current_pid, FName, strerror(errno), path);exit(EXIT_FAILURE);}
}

void delete_f() {
    for (int i = 0; i < 8; i++)
        if (remove(Pids[i]))
            fprintf(stderr, "%d: %s: %s: %s\n", getpid(), FName, strerror(errno), Pids[i]);
}

void outputError(const char* path)
{
    char strError[200];
    sprintf(strError, "%d '%s' %s", (int)getpid(), FName, path);
    perror(strError);
}

long findTime() 
{
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    return currentTime.tv_usec %1000L;
}

void build_tree()
{
zero_pid = getpid();
process_number = 0;
	pid_t pid = fork();//1
	if(pid == 0){
		pid = fork();
		process_number = pid == 0 ? 2 : 1;
		if (pid == 0){//2
			pid = fork();//3
			process_number = pid == 0 ? 3 : 2;
			if (pid == 0){
				pid = fork();//6
				process_number = pid == 0 ? 6 : 3;
				if (pid == 0){
					pid = fork();//7
					process_number = pid == 0 ? 7 : 6;
					if (pid == 0){
						pid = fork();//8
						process_number = pid == 0 ? 8 : 7;
					}
				}
			}else {//4
			pid = fork();
			process_number = pid == 0 ? 4 : 2;
				if (pid == 0){
					pid = fork();//5
					process_number = pid == 0 ? 5 : 4;
				}
			}
		}
	}
	
	if (getpid() == -1)
		exit(-1);
	
	if(process_number == 0){
		wait(0);
		delete_f();
		return;
	}
	
	write_pid_f(Pids[process_number-1], getpid());
	
	switch(process_number)
	{
		case 1: createHandler1(); break;
		case 2: createHandler2(); break;
		case 3: createHandler3(); break;
		case 4: createHandler4(); break;
		case 5: createHandler5(); break;
		case 6: createHandler6(); break;
		case 7: createHandler7(); break;
		case 8: createHandler8(); break;
	}	
}

void handler1()
{
    sigcount1++;
    fprintf(stdout, "1 %d %d получил SIGUSR2 %lu\n", getpid(), getppid(), findTime());
    if (sigcount1 == CounterOfExit)
    {
        kill(read_pid_f(PID_2), SIGTERM);
        wait(0);
        fprintf(stdout, "1 %d %d завершился после %d SIGUSR1\n", getpid(), getppid(), sigusrcount2);
        fflush(stdout);
        exit(EXIT_SUCCESS);
    }
    else
    {
    	sigusrcount2++;
        fprintf(stdout, "1 %d %d отправил SIGUSR1 %lu\n", getpid(), getppid(), findTime());
        kill(read_pid_f(PID_8), SIGUSR1);
   
        fflush(stdout);
    }   

}

void handler2()
{
    sigusrcount3++;
    fprintf(stdout, "2 %d %d получил SIGUSR1 %lu\n", getpid(), getppid(), findTime());
    fprintf(stdout, "2 %d %d отправил SIGUSR2 %lu\n", getpid(), getppid(), findTime());
    kill(read_pid_f(PID_1), SIGUSR2);
    fflush(stdout);

}

void handler3()
{
    sigusrcount2++;
    fprintf(stdout, "3 %d %d получил SIGUSR1 %lu\n", getpid(), getppid(), findTime());
    fflush(stdout);

}

void handler4()
{
    sigusrcount2++;
    fprintf(stdout, "4 %d %d получил SIGUSR1 %lu\n", getpid(), getppid(), findTime());
    fprintf(stdout, "4 %d %d отправил SIGUSR1 %lu\n", getpid(), getppid(), findTime());
    killpg(read_pid_f(PID_2), SIGUSR1);
    fflush(stdout);

}

void handler5() {}

void handler6()
{
    sigusrcount3++;
    fprintf(stdout, "6 %d %d получил SIGUSR1 %lu\n", getpid(), getppid(), findTime());
    fprintf(stdout, "6 %d %d отправил SIGUSR2 %lu\n", getpid(), getppid(), findTime());
    kill(read_pid_f(PID_7), SIGUSR2);
    fflush(stdout);

}

void handler7()
{
    sigusrcount2++;
    fprintf(stdout, "7 %d %d получил SIGUSR2 %lu\n", getpid(), getppid(), findTime());
    fprintf(stdout, "7 %d %d отправил SIGUSR1 %lu\n", getpid(), getppid(), findTime());
    kill(read_pid_f(PID_4), SIGUSR1);
    fflush(stdout);

}

void handler8()
{
    sigusrcount2++;
    fprintf(stdout, "8 %d %d получил SIGUSR1 %lu\n", getpid(), getppid(), findTime());
    fprintf(stdout, "8 %d %d отправил SIGUSR1 %lu\n", getpid(), getppid(), findTime());
    kill(read_pid_f(PID_6), SIGUSR1);
    fflush(stdout);

}

void handlerTerm2()
{
    kill(read_pid_f(PID_3), SIGTERM);
    kill(read_pid_f(PID_4), SIGTERM);
    kill(read_pid_f(PID_5), SIGTERM);
    for (int i = 0; i < 3; ++i) 
    {
        wait(0);
    }
    fprintf(stdout, "2 %d %d завершился после %d SIGUSR2\n", getpid(), getppid(), sigusrcount3);
    fflush(stdout);
    exit(EXIT_SUCCESS);
}

void handlerTerm3()
{
    kill(read_pid_f(PID_7), SIGTERM);
    fprintf(stdout, "3 %d %d завершился после %d SIGUSR1\n", getpid(), getppid(), sigusrcount2);
    fflush(stdout);
    exit(EXIT_SUCCESS);
}

void handlerTerm4()
{
    kill(read_pid_f(PID_6), SIGTERM);
    fprintf(stdout, "4 %d %d завершился после %d SIGUSR1\n", getpid(), getppid(), sigusrcount2);
    fflush(stdout);
    exit(EXIT_SUCCESS);
}

void handlerTerm5()
{
    kill(read_pid_f(PID_8), SIGTERM);
    fprintf(stdout, "5 %d %d завершился после %d SIGUSR1\n", getpid(), getppid(), sigusrcount2);
    fflush(stdout);
    exit(EXIT_SUCCESS);
}

void handlerTerm6()
{
    fprintf(stdout, "6 %d %d завершился после %d SIGUSR2\n", getpid(), getppid(), sigusrcount3);
    fflush(stdout);
    exit(EXIT_SUCCESS);
}

void handlerTerm7()
{
    fprintf(stdout, "7 %d %d завершился после %d SIGUSR1\n", getpid(), getppid(), sigusrcount2);
    fflush(stdout);
    exit(EXIT_SUCCESS);
}

void handlerTerm8()
{
    fprintf(stdout, "8 %d %d завершился после %d SIGUSR1\n", getpid(), getppid(), sigusrcount2);
    fflush(stdout);
    exit(EXIT_SUCCESS);
}

void createHandler1()
{
    __sigaction(SIGUSR2,handler1);
    while (1)
    	pause();
}

void createHandler2()
{
    __sigaction(SIGUSR1, handler2);
    __sigaction(SIGTERM, handlerTerm2);
    if (setpgid(read_pid_f(PID_2), read_pid_f(PID_2)) == -1)
        outputError("");

    while (1) pause();
}

void createHandler3()
{
    __sigaction(SIGUSR1, handler3);
    __sigaction(SIGTERM, handlerTerm3);
    if (setpgid(read_pid_f(PID_3), read_pid_f(PID_2)) == -1)
        outputError("");

    while (1)
    	pause();
}

void createHandler4()
{
    __sigaction(SIGUSR1, handler4);
    __sigaction(SIGTERM, handlerTerm4);

    while (1)
    	pause();
}

void createHandler5()
{
    __sigaction(SIGUSR1, handler5);
    __sigaction(SIGTERM, handlerTerm5);

    while (1)
    	pause();
}

void createHandler6()
{
    __sigaction(SIGUSR1, handler6);
    __sigaction(SIGTERM, handlerTerm6);
    while (1)
    	pause();
}

void createHandler7()
{
    __sigaction(SIGUSR2, handler7);
    __sigaction(SIGTERM, handlerTerm7);
    while (1)
    	pause();
}

void createHandler8()
{
    __sigaction(SIGUSR1, handler8);
    __sigaction(SIGTERM, handlerTerm8);
        
    fprintf(stdout, "1 %d %d отправил SIGUSR1 %lu\n", (int)(read_pid_f(PID_1)), (int)(zero_pid), findTime());
    kill(read_pid_f(PID_8), SIGUSR1);
    while (1)
    	pause();	
}

int main(int argc, char *argv[])
{
	FName = basename(argv[0]);
	build_tree();
	return 0;
}

