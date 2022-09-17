#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX_BYTES 255
#define ARGS_AMOUNT 4
#define MIN_PROC 1
//#define BUFF_SIZE 4096 * 4096

char* catalog;
char* byte_sequence;
int proc_max_amount;
char* program_name;
int curr_process_num;
//char buffer[BUFF_SIZE];

typedef struct byte_search_result {
    int entries_count;
    ssize_t total_bytes_checked;
} byte_search_result;

int open_directory(char*);
int find_bytes_in_file(const char*, const char*, byte_search_result*);
int get_process(char*);

int main(int argc, char* argv[]) {
    program_name = basename(argv[0]);
    if (argc != ARGS_AMOUNT)
    {
        printf("%s: Wrong amount of arguments\n", program_name);
        return 1;
    }

    proc_max_amount = atoi(argv[1]);
    if(proc_max_amount < MIN_PROC)
    {
        printf("%s: Amount of processes cannot be less than one\n", program_name);
        return 1;
    }

    byte_sequence = argv[2];
    if(strlen(byte_sequence) >= MAX_BYTES)
    {
        printf("%s: Byte sequence cannot be more than 255\n", program_name);
        return 1;
    }

    catalog = argv[3];
//catalog = "/etc";
//proc_max_amount = 100;
//byte_sequence="0110001010011";

    curr_process_num = 1;

    open_directory(catalog);

    while(curr_process_num != 0) {
        if (wait(NULL) == -1) {
            return 1;
        }
        curr_process_num--;
    }
    return 0;
}

int open_directory(char* dir_name) {
    DIR* dir;
    struct dirent* dp;
    char* file_path;
    if ((dir = opendir(dir_name)) == NULL) {
        fprintf(stderr, "%s: Can't open directory %s: %s\n", program_name, dir_name, strerror(errno));
        return 1;
    }
    errno = 0;
    while ((dp = readdir(dir)) != NULL) {
        file_path = malloc(strlen(dir_name) + strlen(dp->d_name) + 2);
        strcpy(file_path, dir_name);
        strcat(file_path, "/");
        strcat(file_path, dp->d_name);
        if (((dp->d_type == DT_DIR)) && ((strcmp(dp->d_name,".")) != 0) && ((strcmp(dp->d_name,"..")) != 0)) {
//            printf("folder: %s\n", file_path);
            open_directory(file_path);
        }
        else if ((dp->d_type == DT_REG) || (dp->d_type == DT_LNK)) {
//            printf("file: %s\n", file_path);
            get_process(file_path);
        }
        free(file_path);
    }
    if(closedir((dir)) != 0) {
        fprintf(stderr, "%s: Can't close directory %s: %s\n", program_name, dir_name, strerror(errno));
        return 1;
    }
    return 0;
}

int get_process(char* file_path) {
    pid_t wait_result;
    pid_t pid;
    byte_search_result byte_search_result;

    if (curr_process_num >= proc_max_amount)
    {
        if((wait_result = wait(NULL)) > 0)
        {
            curr_process_num--;
        }
        if (wait_result == -1)
        {
            fprintf(stderr, "%s: Error in get_process func\n", program_name);
            exit(0);
        }
    }
    else
    {
        pid = fork();
        if (pid == -1)
        {
            fprintf(stderr, "%s: Pid: %d: %s %s", program_name, getpid(), strerror(errno), file_path);
            errno = 0;
            return 1;
        }
        if (pid > 0)
        {
            curr_process_num++;
        }
        if (pid == 0)
        {
            if(find_bytes_in_file(file_path, byte_sequence, &byte_search_result) != -1)
            {
                if (byte_search_result.entries_count != 0)
                {
                    printf("pid: %d, %s, %zd, %d\n",
                           getpid(), file_path, byte_search_result.total_bytes_checked, byte_search_result.entries_count);
                }
            }
            exit(0);
        }
    }
    return 0;
}

int find_bytes_in_file(const char* file_name, const char* byte_sequence, byte_search_result* search_result) {
    FILE *fileptr;
    char *buffer;
    long filelen;
    errno = 0;

    fileptr = fopen(file_name, "rb");  // Open the file in binary mode
    fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
    filelen = ftell(fileptr);             // Get the current byte offset in the file
    rewind(fileptr);                      // Jump back to the beginning of the file

    buffer = (char *)malloc(filelen * sizeof(char)); // Enough memory for the file
    fread(buffer, filelen, 1, fileptr); // Read in the entire file
    fclose(fileptr); // Close the file

    int entries_count = 0;
}
//int find_bytes_in_file(const char* file_name, const char* byte_sequence, byte_search_result* search_result)
//{
//    errno = 0;
//    int file_descriptor = open(file_name, O_RDONLY);
//    if (file_descriptor == -1)
//    {
//        fprintf(stderr, "%s: Error while opening file %s, %s\n", program_name, file_name, strerror(errno));
//        return -1;
//    }
//
//    struct stat stat_buf;
//    if(lstat(file_name, &stat_buf) == -1)
//    {
//        fprintf(stderr, "%s: Cannot read file %s\n", program_name, file_name);
//        return -1;
//    }
//
//    int entries_count = 0;
//    int tmp;
//    while(1)
//    {
//        tmp = read(file_descriptor, &buffer, BUFF_SIZE);
//        if (tmp == -1)
//        {
//            return -1;
//        }
//        if (tmp == 0)
//        {
//            break;
//        }
//
//        if (memmem(buffer, BUFF_SIZE, byte_sequence, strlen(byte_sequence)) != NULL)
//        {
//            entries_count++;
//            break;
//        }
//    }
//    if (close(file_descriptor) == -1) {
//        return -1;
//    }
//
//    search_result->entries_count = entries_count;
//    search_result->total_bytes_checked = stat_buf.st_size;
//    return 0;
//}


