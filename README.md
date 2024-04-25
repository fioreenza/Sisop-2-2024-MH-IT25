# Laporan Resmi SisOp Modul 2 Kelompok IT25

**Anggota kelompok IT25**:
* Fiorenza Adelia Nalle (5027231053)
* Tio Axellino Irin (5027231065)
* Wira Samudra Siregar (5027231041)

#  Tujuan Praktikum Modul 2

# Deskripsi Praktikum Modul 2

Pada Praktikum Modul 1 ini, kami diberikan tugas untuk mengerjakan Soal Shift Modul 2 yang dilaksanakan mulai hari Rabu (20 Maret 2024) setelah sesi lab hingga hari Senin (25 Maret 2024). Berikut adalah pembagian pengerjaan Soal Shift Modul 2:
* Soal 1 yang dikerjakan oleh Fiorenza Adelia Nalle (5027231053)
* Soal 2 yang dikerjakan oleh Tio Axellino Irin (5027231065)
* Soal 3 yang dikerjakan oleh Wira Samudra Siregar (5027231041)
* Soal 4 yang dikerjakan oleh Fiorenza Adelia Nalle (5027231053)

# Proses dan Hasil Praktikum Modul 2

## Soal 1
**oleh Fiorenza Adelia Nalle (5027231053)**
### Deskripsi Soal 1
1. Gavriel adalah seorang cyber security enthusiast. Suatu hari, ia dikontrak oleh sebuah perusahaan ternama untuk membuat sebuah program yang cukup rumit dengan bayaran jutaan rupiah. Karena tergoda dengan nominal bayaran tersebut, Gavriel langsung menerima tawaran tersebut. Setelah mencoba membuat dan mengembangkan program tersebut selama seminggu, laptop yang digunakan Gavriel mengalami overheat dan mati total sehingga harus dilarikan ke tukang servis terdekat. Karena keterbatasan waktu dalam pembuatan program dan tidak ingin mengecewakan perusahaan, Gavriel meminta bantuan kalian untuk membuat program tersebut dengan ketentuan sebagai berikut:
    1. Program dapat menerima input path berupa ‘argv’ untuk mengatur folder dimana file akan dieksekusi
    2. Program tersebut berfungsi untuk mereplace string dengan ketentuan sebagai berikut:
       * String m4LwAr3 direplace dengan string [MALWARE]
       * String 5pYw4R3 direplace dengan string [SPYWARE]
       * String R4nS0mWaR3 direplace dengan string [RANSOMWARE]
    3. Program harus berjalan secara daemon, dan tidak diperbolehkan menggunakan command system() dalam pembuatan program
    4. Program akan secara terus menerus berjalan di background dengan jeda 15 detik
    5. Catat setiap penghapusan string yang dilakukan oleh program pada sebuah file bernama virus.log dengan format: [dd-mm-YYYY][HH:MM:SS] Suspicious string at <file_name> successfully replaced!

### Penyelesaian Soal 1
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

* Penjelasan
  
          void formatTimestamp(char *timestamp) {
                time_t now;
                struct tm *tm_info;
                time(&now);
                tm_info = localtime(&now);
                
                sprintf(timestamp, "[%02d-%02d-%04d][%02d:%02d:%02d]", 
                        tm_info->tm_mday, tm_info->tm_mon + 1, tm_info->tm_year + 1900, 
                        tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
            }
  Fungsi formatTimestamp bertanggung jawab untuk memformat waktu saat ini ke dalam string dengan format
  yang spesifik. Pertama, fungsi ini mendeklarasikan variabel now bertipe time_t untuk menyimpan waktu
  saat ini. Kemudian, menggunakan fungsi time, waktu saat ini diambil dan disimpan dalam variabel now.

  Langkah selanjutnya adalah menggunakan fungsi localtime untuk mengonversi waktu dalam format time_t
  ke struktur tm. Hasil konversi ini disimpan dalam variabel tm_info. Setelah mendapatkan informasi
  waktu dalam bentuk struktur tm, fungsi sprintf digunakan untuk memformat string timestamp. Format
  string yang digunakan adalah "[%02d-%02d-%04d][%02d:%02d:%02d]".

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
  Fungsi replaceString bertujuan untuk menggantikan setiap kemunculan string old dalam string
  str dengan string new. Selama masih ada kemunculan old dalam str, fungsi akan terus melakukan
  penggantian.

  Pada setiap penggantian, fungsi akan menandai bahwa penggantian telah dilakukan dengan mengubah
  nilai boolean replacements menjadi true. Kemudian, isi dari str akan disalin ke dalam array temp, dan
  posisi kemunculan old dalam str akan dihitung. String old pada posisi tersebut akan digantikan dengan
  karakter null, dan string new akan digabungkan ke dalam str. Sisa dari string temp, setelah posisi
  kemunculan old, juga akan digabungkan ke dalam str.

  Selain melakukan penggantian, fungsi juga mencatat setiap penggantian yang dilakukan ke dalam file
  log yang diberikan (logFile). Catatan tersebut mencakup waktu saat penggantian dilakukan, ditandai
  dengan timestamp yang diformat menggunakan fungsi formatTimestamp. Setelah mencatat penggantian,
  fungsi akan menunda proses selama 15 detik menggunakan fungsi sleep(15).

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
  Pada fungsi main, pertama-tama dilakukan pemeriksaan terhadap jumlah argumen yang diberikan melalui
  baris perintah. Jika jumlah argumen tidak sesuai, program akan menampilkan pesan penggunaan yang
  menjelaskan cara penggunaan program dan kemudian keluar dengan status kegagalan.
    
  Jalur file yang diberikan melalui argumen baris perintah diambil dan disimpan dalam variabel path.
  Selanjutnya, jalur lengkap untuk file log virus dibangun menggunakan fungsi snprintf, yang kemudian
  disimpan dalam buffer virusLogPath.
    
  Setelah itu, file log virus dibuka dalam mode tambah ("a") dengan menggunakan fungsi fopen. Jika
  pembukaan file gagal, program akan menampilkan pesan kesalahan dan keluar dengan status kegagalan.
    
  Kemudian, izin untuk file log virus diubah menggunakan fungsi chmod. Jika perubahan izin gagal,
  program akan menampilkan pesan kesalahan, menutup file log virus, dan keluar dengan status kegagalan.

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
  Setelah inisialisasi, program melakukan pemanggilan fork(), yang menghasilkan dua proses: satu
  proses induk dan satu proses anak. Pemeriksaan dilakukan untuk memastikan bahwa fork() berhasil
  dilakukan. Jika fork() mengembalikan nilai negatif, artinya gagal, maka pesan kesalahan akan
  ditampilkan, file log virus akan ditutup, dan program akan keluar dengan status kegagalan.

  Jika proses berjalan dengan sukses, proses induk akan menutup file log virus yang telah dibuka
  sebelumnya dan keluar dengan status keberhasilan menggunakan exit(EXIT_SUCCESS). Proses anak akan
  melanjutkan eksekusi kode selanjutnya setelah titik ini.

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
  Pada bagian ini, program membuat sesi baru dengan memanggil fungsi setsid(). Jika pembuatan sesi
  baru gagal, maka program akan menampilkan pesan kesalahan, menutup file log virus, dan keluar dengan
  status kegagalan.

  Selanjutnya, program mengubah direktori kerja saat ini ke jalur yang telah ditentukan sebelumnya.
  Jika pengubahan direktori kerja gagal, maka pesan kesalahan akan ditampilkan, file log virus akan
  ditutup, dan program akan keluar dengan status kegagalan.

  Setelah itu, program menutup tiga file descriptor standar: standar masukan (STDIN_FILENO), standar
  keluaran (STDOUT_FILENO), dan standar error (STDERR_FILENO). Tindakan ini dilakukan untuk
  membersihkan lingkungan proses dan memastikan bahwa proses tidak memilikinya, karena program mungkin
  berjalan sebagai daemon tanpa koneksi dengan terminal.

          bool replacements = false; 
    
        DIR *dir;
        struct dirent *entry;
    
        dir = opendir(".");
        if (dir == NULL) {
            perror("Error opening directory");
            exit(EXIT_FAILURE);
        }
  Pada bagian ini, sebuah variabel boolean replacements dideklarasikan untuk menandai apakah
  penggantian sudah dilakukan. Nilainya diatur awalnya menjadi false.

  Selanjutnya, program membuka direktori saat ini dengan memanggil fungsi opendir("."). Jika pembukaan
  direktori gagal, maka program akan menampilkan pesan kesalahan, menutup file log virus, dan keluar
  dengan status kegagalan.

        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_REG) { // Check if it's a regular file
                FILE *file = fopen(entry->d_name, "r+");
                if (file == NULL) {
                    perror("Error opening file");
                    continue;
                }
  Langkah ini akan melanjutkan untuk membaca file yang ada di dalam direktori saat ini dan melakukan
  operasi penggantian string pada setiap file yang ditemukan. Di sini, program melakukan iterasi
  melalui setiap entri dalam direktori yang telah dibuka sebelumnya. Selama masih ada entri yang
  tersedia, proses akan terus berlanjut.
    
  Setiap entri diperiksa untuk memastikan bahwa itu adalah file reguler. Jika ya, maka program membuka
  file tersebut dalam mode baca dan tulis menggunakan fungsi fopen(). Jika pembukaan file gagal, pesan
  kesalahan akan ditampilkan, dan program akan melanjutkan ke file berikutnya dengan menggunakan
  pernyataan continue.

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
  Di sini, program membaca setiap baris dari file yang telah dibuka sebelumnya menggunakan fungsi
  fgets(). Setiap baris yang terbaca disalin ke dalam buffer modifiedLine. Kemudian, setiap baris dalam
  modifiedLine diperiksa dan string-string yang mencurigakan, seperti "m4LwAr3", "5pYw4R3", dan
  "R4nS0mWaR3", diganti dengan string yang lebih jelas dan jujur seperti "[MALWARE]", "[SPYWARE]", dan
  [RANSOMWARE]". Penggantian ini dilakukan dengan menggunakan fungsi replaceString(). Setiap kali
  penggantian dilakukan, variabel replacements akan diberi nilai true jika ada setidaknya satu
  penggantian yang dilakukan.

  Setelah penggantian selesai, penunjuk posisi dalam file dikembalikan ke awal baris saat ini
  menggunakan fseek() dengan menggunakan parameter SEEK_CUR, kemudian baris yang dimodifikasi ditulis
  kembali ke file menggunakan fputs().

  Langkah-langkah ini dilakukan untuk setiap baris dalam file yang sedang diproses, sehingga setiap
  kemunculan string yang mencurigakan diganti dengan string yang lebih tepat dan kemudian ditulis
  kembali ke file. Ini adalah bagian penting dari proses deteksi dan penanganan malware dalam file-file
  yang ditemukan dalam direktori yang sedang diproses.

  Setelah setiap baris dalam file diproses dan dimodifikasi, program memeriksa apakah terjadi kesalahan
  saat menulis ke file dengan menggunakan fungsi ferror(). Jika terjadi kesalahan, pesan kesalahan akan
  ditampilkan, file yang sedang diproses dan file log virus akan ditutup, dan program akan keluar
  dengan status kegagalan.

  Setelah selesai memproses semua baris dalam file yang bersangkutan, file tersebut ditutup menggunakan
  fungsi fclose(). Proses ini terjadi di dalam loop yang iterasi melalui semua file reguler dalam
  direktori yang sedang diproses.

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

  Pada bagian akhir fungsi main, setelah setiap baris dalam file diproses dan dimodifikasi, program
  memeriksa apakah terjadi kesalahan saat menulis ke file dengan menggunakan fungsi ferror(). Jika
  terjadi kesalahan, pesan kesalahan akan ditampilkan, file yang sedang diproses dan file log virus
  akan ditutup, dan program akan keluar dengan status kegagalan.
    
  Setelah selesai memproses semua file dalam direktori, program menutup direktori yang telah dibuka
  sebelumnya dengan menggunakan fungsi closedir(), dan juga menutup file log virus dengan fclose()
  untuk membersihkan sumber daya yang telah digunakan.
    
  Terakhir, program mengembalikan status keluaran 0 dengan return 0;, menandakan bahwa proses telah
  selesai dengan sukses. Ini adalah langkah terakhir dari program dan menunjukkan bahwa semua tugas
  telah diselesaikan dengan baik.


### Kendala Pengerjaan Soal 1
### Screenshot Hasil Pengerjaan Soal 1


## Soal 2
**oleh Tio Axellino Irin (5027231065)**
### Deskripsi Soal 2
### Penyelesaian Soal 2
### Kendala Pengerjaan Soal 2
### Screenshot Hasil Pengerjaan Soal 2

## Soal 3
**Wira Samudra Siregar (5027231041)**
### Deskripsi Soal 3
### Penyelesaian Soal 3
### Kendala Pengerjaan Soal 3
### Screenshot Hasil Pengerjaan Soal 3

## Soal 4
**oleh Fiorenza Adelia Nalle (5027231053)**
### Deskripsi Soal 4
4. Salomo memiliki passion yang sangat dalam di bidang sistem operasi. Saat ini, dia ingin mengotomasi kegiatan-kegiatan yang ia lakukan agar dapat bekerja secara efisien. Bantulah Salomo untuk membuat program yang dapat mengotomasi kegiatan dia! (NB: Soal di uji coba dan berhasil di sistem yang menggunakan MacOS dan linux yang menggunakan dual boot dan VM. Untuk teman-teman yang menggunakan WSL bisa mengerjakan soal yang lain ya)
    1. Salomo ingin dapat membuka berbagai macam aplikasi dengan banyak jendela aplikasi dalam satu
       command. Namai file program tersebut setup.c
    2. Program dapat membuka berbagai macam aplikasi dan banyak jendela aplikasi sekaligus (bisa
       membuka 1 atau lebih aplikasi dengan 1 atau lebih window (kecuali aplikasi yang tidak bisa
       dibuka dengan banyak window seperti discord)) dengan menjalankan:
       ./setup -o <app1> <num1> <app2> <num2>.....<appN> <numN>
       Contoh penggunaannya adalah sebagai berikut:
       ./setup -o firefox 2 wireshark 2 Program akan membuka 2 jendela aplikasi firefox dan 2 jendela
       aplikasi wireshark.
    3. Program juga dapat membuka aplikasi dengan menggunakan file konfigurasi dengan menggunakan
       ./setup -f file.conf
       Format file konfigurasi dibebaskan, namun pastikan dapat menjalankan fitur dari poin 2.
       Contoh isi file.conf:
       Firefox 2
       Wireshark 3
       Ketika menjalankan command contoh, program akan membuka 2 jendela aplikasi firefox dan 3 jendela
       aplikasi wireshark.
    4. Program dapat mematikan semua aplikasi yg dijalankan oleh program tersebut dengan:  ./setup -k
    5. Program juga dapat mematikan aplikasi yang dijalankan sesuai dengan file konfigurasi.
       Contohnya:
       ./setup -k file.conf
       Command ini hanya mematikan aplikasi yang dijalankan dengan
       ./setup -f file.conf


### Penyelesaian Soal 4

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
* Penjelasan

        #define MAX_CHILDREN 100
        #define FILENAME "file.txt"
        #define OUTPUT_FILENAME "file.txt2"
        #define MAX_FILE_LINE_LENGTH 100
        #define MAX_APPS 100
  Program menggunakan beberapa konstanta praprosesor untuk mengatur parameter-parameter penting. MAX_CHILDREN menentukan jumlah maksimum
  proses anak yang dapat dibuat. FILENAME dan OUTPUT_FILENAME adalah nama file untuk menyimpan PID aplikasi dalam mode standar dan file.
  MAX_FILE_LINE_LENGTH menentukan panjang maksimum setiap baris dalam file. Terakhir, MAX_APPS menentukan jumlah maksimum aplikasi yang
  dapat disimpan dalam mode file. Konstanta-konstanta ini memudahkan penyesuaian dan meningkatkan keterbacaan program.

        typedef struct {
            char name[MAX_FILE_LINE_LENGTH];
            int numWindows;
        } App;
  Struktur data ini, bernama App, digunakan untuk menyimpan informasi tentang sebuah aplikasi. Setiap App memiliki dua anggota data: name
  yang merupakan array karakter untuk menyimpan nama aplikasi, dan numWindows yang merupakan bilangan bulat untuk menyimpan jumlah jendela
  dari aplikasi tersebut yang akan dibuka. 

        pid_t childPids[MAX_CHILDREN];
        int numChildren = 0;
  Array childPids digunakan untuk menyimpan PID (Process ID) dari setiap proses anak yang dibuat oleh program. Kapasitas arraynya
  ditentukan oleh konstanta MAX_CHILDREN. Variabel numChildren digunakan untuk melacak jumlah proses anak yang telah dibuat. 

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
  Fungsi openApplications berfungsi untuk membuka aplikasi-aplikasi yang diberikan sebagai argumen dalam mode "-o". Pertama-tama,
  fungsi ini membuka file dengan nama yang telah ditentukan sebelumnya (FILENAME) untuk menulis PID (Process ID) dari setiap aplikasi yang
  berhasil dibuka. Jika pembukaan file gagal, program akan menampilkan pesan kesalahan dan keluar dengan status error.

  Selanjutnya, fungsi melakukan iterasi melalui argumen yang diberikan pada baris perintah, dimulai dari indeks 2 karena argumen 0 dan 1
  biasanya berisi nama program dan opsi "-o". Untuk setiap aplikasi yang diberikan, fungsi membaca jumlah jendela yang harus dibuka dari
  argumen berikutnya.

  Selama iterasi, fungsi melakukan loop untuk membuat proses anak menggunakan fork(). Jika pembuatan proses anak gagal, fungsi akan
  menampilkan pesan kesalahan dan keluar dengan status error. Jika proses berhasil dibuat, maka proses anak akan menjalankan aplikasi yang
  sesuai dengan argumen yang diberikan menggunakan execlp(). Jika eksekusi aplikasi gagal, pesan kesalahan akan ditampilkan dan proses akan
  keluar dengan status error.

  Di sisi proses induk, fungsi menyimpan PID dari proses anak yang baru dibuat ke dalam file yang telah dibuka sebelumnya, kemudian
  menyimpan PID tersebut juga dalam array childPids untuk referensi selanjutnya. Setelah selesai membuka semua aplikasi, fungsi menutup
  file yang telah digunakan untuk menyimpan PID. Dengan demikian, fungsi ini bertanggung jawab untuk mengatur pembukaan aplikasi dan m
  menyimpan informasi PID yang diperlukan.

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
  Fungsi openAppsFromFile bertugas membuka aplikasi-aplikasi yang telah ditentukan dalam sebuah file yang ditentukan pula. Pertama,
  fungsi membuka file dengan mode baca ("r") untuk membaca setiap baris dari file konfigurasi. Jika pembukaan file gagal, fungsi
  menampilkan pesan kesalahan dan langsung kembali.
    
  Selanjutnya, fungsi membaca baris-baris dari file tersebut. Fungsi akan membaca nama aplikasi dan jumlah jendela dari baris tersebut.
  Informasi tersebut disimpan dalam array apps. Setelah selesai membaca semua aplikasi dari file, fungsi menutup file yang telah digunakan.
  Kemudian, fungsi membuka file output (OUTPUT_FILENAME) dengan mode tulis ("w") untuk menyimpan PID dari setiap proses anak yang berhasil
  dibuat. Jika pembukaan file gagal, fungsi menampilkan pesan kesalahan dan langsung kembali.
    
  Setelah itu, fungsi melakukan loop untuk membuat proses anak untuk setiap aplikasi yang telah dibaca. Setiap proses anak akan
  dijalankan sesuai dengan nama aplikasi yang sesuai dengan informasi yang telah dibaca. PID dari setiap proses anak yang berhasil dibuat
  akan ditulis ke dalam file output (OUTPUT_FILENAME).

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
  Fungsi killApplications bertugas untuk menghentikan semua aplikasi yang sedang berjalan. Pertama, fungsi membuka file dengan nama yang
  telah ditentukan sebelumnya (FILENAME) untuk membaca PID (Process ID) dari setiap aplikasi yang sedang berjalan. Jika pembukaan file
  gagal, fungsi menampilkan pesan kesalahan dan keluar dengan status error.

  Selanjutnya, fungsi membaca PID dari setiap aplikasi yang sedang berjalan dari file tersebut menggunakan fscanf, dan kemudian
  menggunakan fungsi kill untuk mengirim sinyal SIGKILL kepada setiap aplikasi yang diidentifikasi. Jika pembunuhan aplikasi gagal, fungsi
  menampilkan pesan kesalahan.

  Setelah semua aplikasi yang terdaftar dalam file utama (FILENAME) telah dihentikan, fungsi membuka file dengan nama yang telah
  ditentukan sebelumnya untuk menyimpan PID aplikasi dari mode file (OUTPUT_FILENAME). Kemudian, proses yang sama diulangi untuk file ini,
  menghentikan setiap aplikasi yang terdaftar di dalamnya.

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
  Fungsi killApplicationsFromFile bertujuan untuk menghentikan semua aplikasi yang dijalankan oleh command -f file.conf. Pertama, fungsi
  membuka file (OUTPUT_FILENAME) diberikan dalam mode baca ("r") untuk membaca PID dari setiap aplikasi yang harus dihentikan. Jika
  pembukaan file gagal, fungsi menampilkan pesan kesalahan dan keluar dengan status error.

  Selanjutnya, fungsi membaca PID dari setiap aplikasi yang sedang berjalan dari file tersebut menggunakan fscanf, dan kemudian
  menggunakan fungsi kill untuk mengirim sinyal SIGKILL kepada setiap aplikasi yang diidentifikasi. Jika pembunuhan aplikasi gagal, fungsi
  menampilkan pesan kesalahan. Setelah semua aplikasi yang terdaftar dalam file tersebut telah dihentikan, fungsi menutup file yang telah
  digunakan untuk membaca PID aplikasi.

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


  Fungsi utama dari program ini bertanggung jawab untuk mengatur jalannya program berdasarkan argumen yang diberikan pada baris perintah
  saat menjalankan program. Pertama, fungsi memeriksa argumen yang diberikan untuk menentukan tindakan yang harus dilakukan.

  Jika argumen berjumlah 3 dan argumen pertama adalah "-k", maka program akan menggunakan mode "-k" untuk menghentikan semua aplikasi yang
  sedang berjalan berdasarkan informasi yang disimpan dalam file yang ditentukan dalam argumen kedua. Ini dicapai dengan memanggil fungsi
  killApplicationsFromFile.

  Jika argumen berjumlah 2 dan argumen pertama adalah "-k", maka program juga akan menggunakan mode "-k", tetapi untuk menghentikan semua
  aplikasi yang sedang berjalan berdasarkan informasi yang disimpan dalam file default. Ini dilakukan dengan memanggil fungsi
  killApplications.

  Jika argumen berjumlah 3 dan argumen pertama adalah "-f", maka program akan menggunakan mode "-f" untuk membuka dan menjalankan aplikasi-
  aplikasi yang terdaftar dalam file yang ditentukan dalam argumen kedua. Ini dicapai dengan memanggil fungsi openAppsFromFile.

  Selanjutnya, program memeriksa kesalahan pada argumen baris perintah. Jika jumlah argumen ganjil atau kurang dari 3, program akan
  mencetak pesan tentang cara penggunaan yang benar dan mengembalikan status error.

  Jika argumen tidak sesuai dengan opsi yang valid ("-o" untuk menjalankan aplikasi, "-k" untuk menghentikan aplikasi), program
  akan mencetak pesan kesalahan dan mengembalikan status error.

  Terakhir, fungsi openApplications(argc, argv) akan dipanggil setelah semua kondisi telah dinilai. Ini berarti bahwa jika tidak ada
  argumen atau opsi yang sesuai dengan kasus yang diuji sebelumnya, fungsi openApplications akan dieksekusi sebagai jalur standar dalam
  program.

### Kendala Pengerjaan Soal 4

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <sys/wait.h>
    #include <signal.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    
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
                    fprintf(outputFile, "%d\n", pid);
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
    
        pid_t pid = fork();
        if (pid < 0) {
            perror("Error forking");
            exit(EXIT_FAILURE);
        }
    
        if (pid > 0) {
            exit(EXIT_SUCCESS);
        }
    
        umask(0);
    
        if (setsid() < 0) {
            perror("Error creating new session");
            exit(EXIT_FAILURE);
        }
    
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
        open("/dev/null", O_RDONLY);
        open("/dev/null", O_WRONLY);
        open("/dev/null", O_WRONLY);
    
        openApplications(argc, argv);
    
        return 0;
    }

* Penjelasan

  Program tidak berjalan secara daemon, untuk mengubah program menjadi sebuah daemon, code direvisi menjadi yang tertera di atas. Terdapat
  penambahan di bagian int main sebagai berikut :

        pid_t pid = fork();
        if (pid < 0) {
            perror("Error forking");
            exit(EXIT_FAILURE);
        }
    
        if (pid > 0) {
            exit(EXIT_SUCCESS);
        }
    
        umask(0);
    
        if (setsid() < 0) {
            perror("Error creating new session");
            exit(EXIT_FAILURE);
        }
    
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
        open("/dev/null", O_RDONLY);
        open("/dev/null", O_WRONLY);
        open("/dev/null", O_WRONLY);

  Pertama, komentar menjelaskan bahwa program akan diubah menjadi sebuah daemon. Kemudian, langkah pertama adalah membuat proses baru menggunakan
  fungsi fork(). Komentar selanjutnya menjelaskan bahwa jika proses baru gagal dibuat, pesan kesalahan akan ditampilkan dan program akan keluar
  dengan status kegagalan.

  Langkah selanjutnya adalah mengecek apakah proses yang sedang berjalan adalah parent process. Jika iya, komentar menjelaskan bahwa parent process
  akan berakhir dengan keluar dari program dengan status keberhasilan.

  Kemudian, komentar menjelaskan bahwa mode file diatur agar proses baru memiliki akses penuh menggunakan fungsi umask(0). Langkah berikutnya
  adalah membuat sebuah sesi baru dengan fungsi setsid(). Komentar memberikan penjelasan bahwa jika pembuatan sesi baru gagal, pesan kesalahan akan
  ditampilkan dan program akan keluar dengan status kegagalan.

  Langkah terakhir dalam komentar menjelaskan bahwa input/output standar (STDIN, STDOUT, dan STDERR) dialihkan ke /dev/null. Komentar tersebut
  menjelaskan bahwa file descriptor untuk STDIN, STDOUT, dan STDERR ditutup menggunakan fungsi close(), dan kemudian file /dev/null dibuka untuk
  masing-masing file descriptor menggunakan fungsi open(). Hal ini dilakukan untuk memisahkan proses dari terminal dan mengarahkannya ke /dev/null
  agar proses dapat berjalan di background secara independen tanpa mempengaruhi terminal.
    

### Screenshot Hasil Pengerjaan Soal 4
