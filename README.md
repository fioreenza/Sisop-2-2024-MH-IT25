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
Gavriel adalah seorang cyber security enthusiast. Suatu hari, ia dikontrak oleh sebuah perusahaan ternama untuk membuat sebuah program yang cukup rumit dengan bayaran jutaan rupiah. Karena tergoda dengan nominal bayaran tersebut, Gavriel langsung menerima tawaran tersebut. Setelah mencoba membuat dan mengembangkan program tersebut selama seminggu, laptop yang digunakan Gavriel mengalami overheat dan mati total sehingga harus dilarikan ke tukang servis terdekat. Karena keterbatasan waktu dalam pembuatan program dan tidak ingin mengecewakan perusahaan, Gavriel meminta bantuan kalian untuk membuat program tersebut dengan ketentuan sebagai berikut:
Program dapat menerima input path berupa ‘argv’ untuk mengatur folder dimana file akan dieksekusi
Program tersebut berfungsi untuk mereplace string dengan ketentuan sebagai berikut:
String m4LwAr3 direplace dengan string [MALWARE]
String 5pYw4R3 direplace dengan string [SPYWARE]
String R4nS0mWaR3 direplace dengan string [RANSOMWARE]
Program harus berjalan secara daemon, dan tidak diperbolehkan menggunakan command system() dalam pembuatan program
Program akan secara terus menerus berjalan di background dengan jeda 15 detik
Catat setiap penghapusan string yang dilakukan oleh program pada sebuah file bernama virus.log dengan format: [dd-mm-YYYY][HH:MM:SS] Suspicious string at <file_name> successfully replaced!
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
### Penyelesaian Soal 4
### Kendala Pengerjaan Soal 4
### Screenshot Hasil Pengerjaan Soal 4
