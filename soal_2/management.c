#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <time.h>

volatile sig_atomic_t mode = 0;
volatile sig_atomic_t running = 1; // Flag to control program execution

void decryptFileName(char *fileName)
{
    int i;
    for (i = 0; fileName[i] != '\0'; ++i)
    {
        char ch = fileName[i];
        if (ch >= 'A' && ch <= 'Z')
        {
            fileName[i] = ((ch - 'A' + 7) % 26) + 'A';
        }
        else if (ch >= 'a' && ch <= 'z')
        {
            fileName[i] = ((ch - 'a' + 7) % 26) + 'a';
        }
    }
}

void moveFile(const char *src, const char *dest)
{
    // Check if source file exists
    if (access(src, F_OK) != -1)
    {
        // Perform move operation
        if (rename(src, dest) != 0)
        {
            perror("Error moving file");
            exit(EXIT_FAILURE);
        }
    }
}

void createBackupDirectory()
{
    struct stat st = {0};
    if (stat("/home/ludwigd/SisOP/Praktikum2/nomor2/library/backup", &st) == -1)
    {
        if (mkdir("/home/ludwigd/SisOP/Praktikum2/nomor2/library/backup", 0700) == -1)
        {
            perror("Error creating backup directory");
            exit(EXIT_FAILURE);
        }
    }
}

void signalHandler(int signum)
{
    if (signum == SIGRTMIN)
    {
        mode = 0;
        // Set mode to default
    }
    else if (signum == SIGUSR1)
    {
        mode = 1;
        // Set mode to backup
    }
    else if (signum == SIGUSR2)
    {
        mode = 2;
        // Set mode to restore
    }
    else if (signum == SIGTERM || signum == SIGINT)
    {
        // SIGTERM or SIGINT received, stop program execution
        running = 0;
    }
}

void logEvent(const char *user, const char *file, const char *action)
{
    // Buka file log untuk ditulis (mode "a" untuk menambahkan ke akhir file)
    FILE *logfile = fopen("/home/ludwigd/SisOP/Praktikum2/nomor2/history.log", "a");
    if (logfile == NULL)
    {
        // Jika gagal membuka file log, tampilkan pesan kesalahan dan keluar
        perror("Failed to open log file");
        exit(EXIT_FAILURE);
    }

    // Dapatkan waktu saat ini
    time_t now;
    time(&now);
    struct tm *timeinfo = localtime(&now);

    // Buat string waktu dalam format [HH:MM:SS]
    char time_str[9];
    strftime(time_str, sizeof(time_str), "%H:%M:%S", timeinfo);

    // Tulis pesan log ke file
    fprintf(logfile, "[%s][%s] - %s - %s\n", user, time_str, file, action);

    // Tutup file log
    fclose(logfile);
}

void usage()
{
    printf("Usage: ./management [-m mode]\n");
    printf("Modes:\n");
    printf("  default : Run in default mode\n");
    printf("  backup  : Run in backup mode\n");
    printf("  restore : Run in restore mode\n");
}

int main(int argc, char *argv[]) {
    // Signal handling
    signal(SIGRTMIN, signalHandler);
    signal(SIGUSR1, signalHandler);
    signal(SIGUSR2, signalHandler);
    signal(SIGTERM, signalHandler); // Handle SIGTERM (Termination signal)
    signal(SIGINT, signalHandler);  // Handle SIGINT (Interrupt signal)

    pid_t pid, sid;        // Variabel untuk menyimpan PID

    pid = fork();     // Menyimpan PID dari Child Process

    /* Keluar saat fork gagal
    * (nilai variabel pid < 0) */
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    /* Keluar saat fork berhasil
    * (nilai variabel pid adalah PID dari child process) */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    umask(0);

    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Check if mode is default (no command line arguments)
    if (argc == 1)
    {
        mode = 0;
    }
    else if (argc == 2 && strcmp(argv[1], "-m") == 0)
    {
        // Mode specified in command line argument
        // Set mode accordingly
        if (strcmp(argv[2], "default") == 0)
        {
            mode = 0;
        }
        else if (strcmp(argv[2], "backup") == 0)
        {
            mode = 1;
        }
        else if (strcmp(argv[2], "restore") == 0)
        {
            mode = 2;
        }
        else
        {
            printf("Invalid mode specified\n");
	    usage();
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        printf("Invalid arguments\n");
	usage();
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        if (mode == 0)
        {
            pid_t child_pid = fork(); // Fork untuk menjalankan child process

            if (child_pid == 0)
            { // Child Process
            char *download_args[] = {"wget", "--quiet", "--no-check-certificate", "https://docs.google.com/uc?export=download&id=1rUIZmp10lXLtCIH3LAZJzRPeRks3Crup", "-O", "/home/ludwigd/SisOP/Praktikum2/nomor2/library.zip", NULL};
            execvp("wget", download_args); // Eksekusi command wget
            exit(EXIT_SUCCESS); // Keluar dari child process
            }
            else if
            (child_pid < 0) { // Fork gagal
            syslog(LOG_ERR, "Failed to fork child process for download");
            }
            else
            { // Parent Process
                int download_status;
                waitpid(child_pid, &download_status, 0); // Menunggu child process selesai
                if (WIFEXITED(download_status) && WEXITSTATUS(download_status) == 0)
                {
                    syslog(LOG_INFO, "File downloaded successfully");
                }
                else
                {
                    syslog(LOG_ERR, "Failed to download file");
                }

                // Unzip file
                pid_t unzip_child_pid = fork(); // Fork untuk menjalankan child process untuk unzip

                if (unzip_child_pid == 0)
                { // Child Process
                    char *unzip_args[] = {"unzip", "-q", "/home/ludwigd/SisOP/Praktikum2/nomor2/library.zip", "-d", "/home/ludwigd/SisOP/Praktikum2/nomor2/", NULL};
                    execvp("unzip", unzip_args); // Eksekusi command unzip
                    exit(EXIT_SUCCESS); // Keluar dari child process
                }
                else if (unzip_child_pid < 0)
                { // Fork gagal
                    syslog(LOG_ERR, "Failed to fork child process for unzip");
                }
                else
                { // Parent Process
                    int unzip_status;
                    waitpid(unzip_child_pid, &unzip_status, 0); // Menunggu child process selesai

                    if (WIFEXITED(unzip_status) && WEXITSTATUS(unzip_status) == 0)
                    {
                    syslog(LOG_INFO, "File unzipped successfully");
                    }
                    else
                    {
                    syslog(LOG_ERR, "Failed to unzip file");
                    }
                    // Decrypt file names
                    pid_t decrypt_child_pid = fork(); // Fork untuk menjalankan child process untuk dekripsi nama file

                    if (decrypt_child_pid == 0)
                    { // Child Process
                    // Mendapatkan daftar file di direktori
                    DIR *dir;
                    struct dirent *ent;
                    if ((dir = opendir ("/home/ludwigd/SisOP/Praktikum2/nomor2/library/")) != NULL)
                    {
                        while ((ent = readdir(dir)) != NULL)
                        {
                        // Hanya dekripsi nama file ke-7 hingga terakhir
                        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
                        {
                            continue;
                        }
                        if (isdigit(ent->d_name[0]))
                        {
                            continue;
                        }
                        char old_name[256];
                        char new_name[256];

                        strcpy(old_name, "/home/ludwigd/SisOP/Praktikum2/nomor2/library/");
                        strcat(old_name, ent->d_name);

                        decryptFileName(ent->d_name);

                        strcpy(new_name, "/home/ludwigd/SisOP/Praktikum2/nomor2/library/");
                        strcat(new_name, ent->d_name);

                        rename(old_name, new_name);
                        }
                        closedir (dir);
                    }
                    else
                    {
                        syslog(LOG_ERR, "Failed to open directory");
                        exit(EXIT_FAILURE);
                    }
                    exit(EXIT_SUCCESS); // Keluar dari child process
                    }

                    else if (decrypt_child_pid < 0)
                    { // Fork gagal
                    syslog(LOG_ERR, "Failed to fork child process for decrypting file names");
                    }
                    else
                    { // Parent Process
                        int decrypt_status;
                        waitpid(decrypt_child_pid, &decrypt_status, 0); // Menunggu child process selesai

                        if (WIFEXITED(decrypt_status) && WEXITSTATUS(decrypt_status) == 0)
                        {
                            syslog(LOG_INFO, "File names decrypted successfully");
                        }
                        else
                        {
                            syslog(LOG_ERR, "Failed to decrypt file names");
                        }
                                    // Handle file dengan nama yang memuat kode r3N4mE
                                    DIR *dir_r3N4mE;
                                    struct dirent *ent_r3N4mE;
                                    if ((dir_r3N4mE = opendir("/home/ludwigd/SisOP/Praktikum2/nomor2/library/")) != NULL)
                                    {
                                        while ((ent_r3N4mE = readdir(dir_r3N4mE)) != NULL)
                                        {
                                            // Cek hanya file dengan kode r3N4mE
					    char *filename = ent_r3N4mE->d_name;
                                            if (strstr(filename, "r3N4mE") != NULL)
                                            {
                                                char old_path[256];
                                                char new_path[256];
                                                strcpy(old_path, "/home/ludwigd/SisOP/Praktikum2/nomor2/library/");
                                                strcat(old_path, ent_r3N4mE->d_name);

                                                char *ext = strrchr(ent_r3N4mE->d_name, '.'); // Mendapatkan ekstensi file
                                                if (ext == NULL)
                                                {
                                                    // Jika tidak ada ekstensi, rename menjadi "renamed.file"
                                                    strcpy(new_path, "/home/ludwigd/SisOP/Praktikum2/nomor2/library/renamed.file");
                                                }
                                                else if (strcmp(ext, ".ts") == 0)
                                                {
                                                    // Jika ekstensi adalah ".ts", rename menjadi "helper.ts"
                                                    strcpy(new_path, "/home/ludwigd/SisOP/Praktikum2/nomor2/library/helper.ts");
                                                }
                                                else if (strcmp(ext, ".py") == 0)
                                                {
                                                    // Jika ekstensi adalah ".py", rename menjadi "calculator.py"
                                                    strcpy(new_path, "/home/ludwigd/SisOP/Praktikum2/nomor2/library/calculator.py");
                                                }
                                                else if (strcmp(ext, ".go") == 0)
                                                {
                                                    // Jika ekstensi adalah ".go", rename menjadi "server.go"
                                                    strcpy(new_path, "/home/ludwigd/SisOP/Praktikum2/nomor2/library/server.go");
                                                }
                                                else
                                                {
                                                    // Jika ekstensi tidak dikenali, rename menjadi "renamed.file"
                                                    strcpy(new_path, "/home/ludwigd/SisOP/Praktikum2/nomor2/library/renamed.file");
                                                }

                                                // Rename file
                                                rename(old_path, new_path);
						logEvent("ludwigd", filename, "Successfully renamed");
                                            }
                                        }
                                        closedir(dir_r3N4mE);
                                    }
                                    else
                                    {
                                        syslog(LOG_ERR, "Failed to open directory for r3N4mE files");
                                        exit(EXIT_FAILURE);
                                    }
                        // Hapus file dengan nama yang mengandung "d3Let3"
                        pid_t remove_child_pid = fork();
                        if (remove_child_pid == 0)
                        {
                            DIR *dir;
                            struct dirent *ent;
                            if ((dir = opendir("/home/ludwigd/SisOP/Praktikum2/nomor2/library/")) != NULL)
                            {
                                while ((ent = readdir(dir)) != NULL)
                                {
				    char *filename = ent->d_name;
                                    if (strstr(filename, "d3Let3") != NULL)
                                    {
                                        char file_path[512];
                                        snprintf(file_path, sizeof(file_path), "/home/ludwigd/SisOP/Praktikum2/nomor2/library/%s", ent->d_name);
                                        remove(file_path);
					logEvent("ludwigd", filename, "Successfully deleted");
                                    }
                                }
                                closedir(dir);
                            }
                            else
                            {
                                syslog(LOG_ERR, "Failed to open directory for file removal");
                                exit(EXIT_FAILURE);
                            }
                            exit(EXIT_SUCCESS);
                        }
                        else if (remove_child_pid < 0)
                        {
                            syslog(LOG_ERR, "Failed to fork child process for file removal");
                        }
                        else
                        {
                            int remove_status;
                            waitpid(remove_child_pid, &remove_status, 0);

                            if (WIFEXITED(remove_status) && WEXITSTATUS(remove_status) == 0)
                            {
                                syslog(LOG_INFO, "Files containing 'd3Let3' removed successfully");
                            }
                            else
                            {
                                syslog(LOG_ERR, "Failed to remove files containing 'd3Let3'");
                            }
                        }
                    }
                }
            }
        }

        else if (mode == 1)
        {
            // Create backup directory if not exists
            createBackupDirectory();
            // Backup mode: Move files with code m0V3 to "backup" folder
            DIR *dir;
            struct dirent *ent;
            if ((dir = opendir("/home/ludwigd/SisOP/Praktikum2/nomor2/library/")) != NULL)
            {
                while ((ent = readdir(dir)) != NULL)
                {
                    if (strstr(ent->d_name, "m0V3") != NULL)
                    {
                        char src_path[512];
                        char dest_path[512];
                        snprintf(src_path, sizeof(src_path), "/home/ludwigd/SisOP/Praktikum2/nomor2/library/%s", ent->d_name);
                        snprintf(dest_path, sizeof(dest_path), "/home/ludwigd/SisOP/Praktikum2/nomor2/library/backup/%s", ent->d_name);
                        moveFile(src_path, dest_path);
			logEvent("ludwigd", ent->d_name, "Successfully moved to backup.");
                    }
                }
                closedir(dir);
            }
            else
            {
                syslog(LOG_ERR, "Failed to open directory for backup");
                exit(EXIT_FAILURE);
            }
        }
        else if (mode == 2)
        {
            // Restore mode: Move files with code m0V3 from "backup" folder back to library folder
            DIR *dir;
            struct dirent *ent;
            if ((dir = opendir("/home/ludwigd/SisOP/Praktikum2/nomor2/library/backup/")) != NULL)
            {
                while ((ent = readdir(dir)) != NULL)
                {
                    if (strstr(ent->d_name, "m0V3") != NULL)
                    {
                        char src_path[512];
                        char dest_path[512];
                        snprintf(src_path, sizeof(src_path), "/home/ludwigd/SisOP/Praktikum2/nomor2/library/backup/%s", ent->d_name);
                        snprintf(dest_path, sizeof(dest_path), "/home/ludwigd/SisOP/Praktikum2/nomor2/library/%s", ent->d_name);
                        moveFile(src_path, dest_path);
			logEvent("ludwigd", ent->d_name, "Successfully restored from backup.");
                    }
                }
                closedir(dir);
            }
            else
            {
                syslog(LOG_ERR, "Failed to open directory for restore");
                exit(EXIT_FAILURE);
            }
        }
        sleep(30);
    }
    printf("Program terminated.\n");
    return 0;
}
