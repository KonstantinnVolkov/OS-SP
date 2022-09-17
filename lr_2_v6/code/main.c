#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <libgen.h>

char *prog_name;
FILE *logFile;

void findLargestFile(DIR *dir, const char *path);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "%s: expected 2 arguments\n", argv[0]);
        exit;
    }
    prog_name = basename(argv[0]);

    char *dirName = malloc(strlen(argv[1]) + 1);
    strcpy(dirName, argv[1]);

    char *destFile = malloc(strlen(argv[2]) + 1);
    strcpy(destFile, argv[2]);

    logFile = fopen(destFile, "w");
    if(logFile == NULL)
    {
        fprintf(stderr, "%s: opening file \n", argv[0]);
        exit;
    }

    DIR* dir = opendir(dirName);
    if (dir == NULL)
    {
        fprintf(stderr, "%s: opendir(%s): %s\n", prog_name, dirname, strerror(errno));
        exit;
    }

    findLargestFile(dir, dirName);

    fclose(logFile);

    free(dirName);
    free(destFile);

    return 0;
}

void findLargestFile(DIR *dir, const char *path)
{
    char* fullpath = NULL;
    char* max_file_name = NULL;
    struct stat pstatus;
    struct dirent *dir_entry;
    long max_size = 0;
    long total_size = 0;
    long count = 0;

    do
    {
        dir_entry = readdir(dir);
        if (!dir_entry)
        {
            break;
        }
        if ((strcmp(dir_entry->d_name, ".") == 0) || (strcmp(dir_entry->d_name, "..") == 0))
        {
            continue;
        }
        if(fullpath != NULL)
        {
            free(fullpath);
        }

        fullpath = (char *)malloc(strlen(path) + strlen(dir_entry->d_name) + 2);
        strcpy(fullpath, path);

        if (fullpath[strlen(fullpath) - 1] != '/')
        {
            strcat(fullpath, "/");
        }

        strcat(fullpath, dir_entry->d_name);
        if (lstat(fullpath, &pstatus) == 0)
        {
            if (S_ISREG(pstatus.st_mode) && !S_ISLNK(pstatus.st_mode))
            {
                total_size += pstatus.st_size;
                count++;
                if (pstatus.st_size > max_size)
                {
                    max_file_name = (char *) malloc(strlen(dir_entry->d_name) + 2);
                    strcpy(max_file_name, dir_entry->d_name);
                }
            }
            else if (S_ISDIR(pstatus.st_mode))
            {
                DIR *dir2 = opendir(fullpath);
                if (dir2 == NULL)
                {
                    fprintf(stderr, "%s: opendir(%s): %s\n", prog_name, fullpath, strerror(errno));
                    exit(errno);
                }
                findLargestFile(dir2, fullpath);
            }
        }
        else
        {
            fprintf(stderr, "%s: lstat:%s\n", prog_name, strerror(errno));
            exit(errno);
        }
    } while (dir_entry);
    closedir(dir);

    printf("%s %ld %ld %s\n", path, count, total_size, max_file_name);
    fprintf(logFile, "%s %ld %ld %s\n", path, count, total_size, max_file_name);

    free(fullpath);
    free(max_file_name);
}

