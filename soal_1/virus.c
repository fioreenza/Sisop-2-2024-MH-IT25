#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdbool.h>

#define MAX_PATH_LENGTH 256
#define MAX_LINE_LENGTH 1000

void formatTimestamp(char *timestamp) {
    time_t now;
    struct tm *tm_info;
    time(&now);
    tm_info = localtime(&now);
    
    sprintf(timestamp, "[%02d-%02d-%04d][%02d:%02d:%02d]", 
            tm_info->tm_mday, tm_info->tm_mon + 1, tm_info->tm_year + 1900, 
            tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
}

void replaceString(char *str, const char *old, const char *new, FILE *logFile, bool *replacements) {
    char *pos, temp[MAX_LINE_LENGTH];
    int index = 0;
    int oldLen = strlen(old);
    while ((pos = strstr(str, old)) != NULL) {
        *replacements = true; 
        strcpy(temp, str);
        index = pos - str;
        str[index] = '\0';
        strcat(str, new);
        strcat(str, temp + index + oldLen);

        char timestamp[30]; 
        formatTimestamp(timestamp);
        fprintf(logFile, "%s Suspicious string at file.txt successfully replaced!\n", timestamp);

        sleep(15);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *path = argv[1];
    char virusLogPath[MAX_PATH_LENGTH];
    snprintf(virusLogPath, sizeof(virusLogPath), "%s/virus.log", path);

    FILE *virusLog = fopen(virusLogPath, "a");
    if (virusLog == NULL) {
        perror("Error opening virus.log");
        exit(EXIT_FAILURE);
    }

    if (chmod(virusLogPath, 0666) == -1) {
        perror("Error setting permission for virus.log");
        fclose(virusLog); 
        exit(EXIT_FAILURE);
    }

    pid_t pid, sid;
    pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        fclose(virusLog); 
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        fclose(virusLog); 
        exit(EXIT_SUCCESS);
    }

    sid = setsid();
    if (sid < 0) {
        perror("Failed to create new session");
        fclose(virusLog); 
        exit(EXIT_FAILURE);
    }

    if ((chdir(path)) < 0) {
        perror("Failed to change directory");
        fclose(virusLog);
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    bool replacements = false; 

    DIR *dir;
    struct dirent *entry;

    dir = opendir(".");
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { // Check if it's a regular file
            FILE *file = fopen(entry->d_name, "r+");
            if (file == NULL) {
                perror("Error opening file");
                continue;
            }

            char line[MAX_LINE_LENGTH];
            while (fgets(line, sizeof(line), file)) {
                char modifiedLine[MAX_LINE_LENGTH];
                strcpy(modifiedLine, line); 
                replaceString(modifiedLine, "m4LwAr3", "[MALWARE]", virusLog, &replacements);
                replaceString(modifiedLine, "5pYw4R3", "[SPYWARE]", virusLog, &replacements);
                replaceString(modifiedLine, "R4nS0mWaR3", "[RANSOMWARE]", virusLog, &replacements);
                fseek(file, -strlen(line), SEEK_CUR); 
                fputs(modifiedLine, file); 

                if (ferror(file)) {
                    perror("Error writing to file");
                    fclose(file);
                    fclose(virusLog);
                    exit(EXIT_FAILURE);
                }
            }

            fclose(file); 
        }
    }

    closedir(dir);
    fclose(virusLog);

    return 0;
}

