#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <dirent.h>

void showProcesses(char *user) {
    char *args[] = {"ps", "-u", user, NULL};
    pid_t pid = fork();

    if (pid == 0) {
        execvp("ps", args);
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork");
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
}

void daemonProcess(char *user) {
    pid_t pid, sid;
    pid = fork();

    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    sigmask(0);
    sid = setsid();

    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    char logfile[100];
    sprintf(logfile, "%s.log", user);
    int log_fd = open(logfile, O_WRONLY | O_CREAT | O_APPEND, 0644);

    while (1) {
        char *args[] = {"ps", "-u", user, NULL};
        pid_t pid = fork();

        if (pid == 0) {
            int fd = open("/dev/null", O_RDWR);
            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
            close(fd);
            execvp("ps", args);
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            perror("fork");
        } else {
            int status;
            waitpid(pid, &status, 0);

            time_t rawtime;
            struct tm * timeinfo;
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            char timestamp[20];
            strftime(timestamp, sizeof(timestamp), "%d:%m:%Y-%H:%M:%S", timeinfo);

            char log_entry[256];
            sprintf(log_entry, "[%s]-%d-%s_%s\n", timestamp, pid, args[2], (WIFEXITED(status) && WEXITSTATUS(status) == 0) ? "JALAN" : "GAGAL");
            write(log_fd, log_entry, strlen(log_entry));
        }

        sleep(1);
    }

    close(log_fd);
}


char *allowedProcesses[] = {"init", "systemd", "bash", "pkill", NULL}; // Daftar proses yang diizinkan

void killUserProcessesContinuously(char *user) {
    pid_t pid;
    
    while (1) {
        char *args[] = {"pgrep", "-u", user, NULL};
        FILE *pipe = popen("pgrep -u user | awk '{print $1}'", "r");
        char pid_str[10];
        
        while (fgets(pid_str, sizeof(pid_str), pipe) != NULL) {
            pid_t pid = atoi(pid_str);
            char cmd[256];
            sprintf(cmd, "ps -p %d -o cmd= | tr -d ' '", pid);
            FILE *cmd_pipe = popen(cmd, "r");
            char process_name[256];
            fgets(process_name, sizeof(process_name), cmd_pipe);
            pclose(cmd_pipe);
            
            int allowed = 0;
            for (int i = 0; allowedProcesses[i] != NULL; i++) {
                if (strcmp(process_name, allowedProcesses[i]) == 0) {
                    allowed = 1;
                    break;
                }
            }
            
            if (!allowed) {
                kill(pid, SIGKILL);
            }
        }
        
        pclose(pipe);
        sleep(1);
    }
}

void stopKillUserProcessesContinuously(char *user) {
    char *args[] = {"pgrep", "-f", "killUserProcessesContinuously", user, NULL};
    FILE *pipe = popen("pgrep -f 'killUserProcessesContinuously USER'", "r");
    char pid_str[10];

    while (fgets(pid_str, sizeof(pid_str), pipe) != NULL) {
        pid_t pid = atoi(pid_str);
        kill(pid, SIGKILL);
    }

    pclose(pipe);
}

void stopKillUserProcesses(char *user) {
    stopKillUserProcessesContinuously(user);

    char *args[] = {"pkill", "-U", user, "-f", "pkill", NULL};
    pid_t pid = fork();
    if (pid == 0) {
        execvp("pkill", args);
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork");
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <option> <user>\n", argv[0]);
        return 1;
    }

    char *user = argv[2];

    if (strcmp(argv[1], "-m") == 0) {
        daemonProcess(user);
    } else if (strcmp(argv[1], "-s") == 0) {
        // Implementasi untuk mematikan daemon process
    } else if (strcmp(argv[1], "-c") == 0) {
        killUserProcessesContinuously(user);
    } else if (strcmp(argv[1], "-a") == 0) {
        stopKillUserProcesses(user);
    } else if (argc == 2) {
        showProcesses(user);
    } else {
        printf("Invalid option\n");
        return 1;
    }

    return 0;
}
