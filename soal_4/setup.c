#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_CHILDREN 100
#define FILENAME "file.txt"
#define OUTPUT_FILENAME "file.txt2"
#define MAX_FILE_LINE_LENGTH 100
#define MAX_APPS 100

typedef struct {
    char name[MAX_FILE_LINE_LENGTH];
    int numWindows;
} App;

pid_t childPids[MAX_CHILDREN];
int numChildren = 0;

void openApplications(int argc, char *argv[]) {
    FILE *file = fopen(FILENAME, "w");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    for (int i = 2; i < argc; i += 2) {
        int numWindows = atoi(argv[i + 1]);

        for (int j = 0; j < numWindows; j++) {
            pid_t pid = fork();
            if (pid == -1) {
                perror("Error forking");
                exit(1);
            } else if (pid == 0) {
                if (execlp(argv[i], argv[i], NULL) == -1) {
                    perror("Error executing application");
                    exit(1);
                }
            } else {
                // Store child process ID in file
                fprintf(file, "%d\n", pid);
                fflush(file);
                childPids[numChildren++] = pid;
                printf("Child process created with PID: %d\n", pid);
            }
        }
    }

    fclose(file);
}

void openAppsFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    App apps[MAX_APPS];
    int numApps = 0;
    char line[MAX_FILE_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%s %d", apps[numApps].name, &apps[numApps].numWindows) == 2) {
            numApps++;
        }
    }

    fclose(file);

    FILE *outputFile = fopen(OUTPUT_FILENAME, "w");
    if (outputFile == NULL) {
        perror("Failed to open output file");
        return;
    }

    for (int i = 0; i < numApps; i++) {
        for (int j = 0; j < apps[i].numWindows; j++) {
            pid_t pid = fork();
            if (pid == -1) {
                perror("Error forking");
                exit(1);
            } else if (pid == 0) {
                if (execlp(apps[i].name, apps[i].name, NULL) == -1) {
                    perror("Error executing application");
                    exit(1);
                }
            } else {
                fprintf(outputFile, "%d\n", pid); // Write PID to file.txt2
                fflush(outputFile);
                printf("Child process created with PID: %d\n", pid);
            }
        }
    }

    fclose(outputFile);
}

void killApplications() {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    pid_t pid;
    while (fscanf(file, "%d", &pid) != EOF) {
        printf("Killing process with PID: %d\n", pid);
        if (kill(pid, SIGKILL) == -1) {
            perror("Error killing process");
        }
    }
    fclose(file);

    FILE *outputFile = fopen(OUTPUT_FILENAME, "r");
    if (outputFile == NULL) {
        perror("Error opening output file");
        exit(1);
    }

    while (fscanf(outputFile, "%d", &pid) != EOF) {
        printf("Killing process with PID: %d\n", pid);
        if (kill(pid, SIGKILL) == -1) {
            perror("Error killing process");
        }
    }
    fclose(outputFile);
}


void killApplicationsFromFile() {
    FILE *file = fopen(OUTPUT_FILENAME, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    pid_t pid;
    while (fscanf(file, "%d", &pid) != EOF) {
        printf("Killing process with PID: %d\n", pid);
        if (kill(pid, SIGKILL) == -1) {
            perror("Error killing process");
        }
    }
    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc == 3 && strcmp(argv[1], "-k") == 0) {
        killApplicationsFromFile(argv[2]);
        return 0;
    }

    if (argc == 2 && strcmp(argv[1], "-k") == 0) {
        killApplications();
        return 0;
    }

    if (argc == 3 && strcmp(argv[1], "-f") == 0) {
        openAppsFromFile(argv[2]);
        return 0;
    }

    if (argc % 2 != 0 || argc < 3) {
        printf("Usage: %s -o <app1> <num1> <app2> <num2> ... <appN> <numN>\n", argv[0]);
        printf("Or: %s -f <filename>\n", argv[0]);
        printf("Or: %s -k <filename>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-o") != 0 && strcmp(argv[1], "-k") != 0) {
        printf("Invalid option. Please use -o to specify applications or -k to kill all.\n");
        return 1;
    }

    openApplications(argc, argv);

    return 0;
}


