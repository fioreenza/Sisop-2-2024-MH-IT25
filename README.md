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
  Tidak ada kendala dalam pengerjaan nomor 1.

### Screenshot Hasil Pengerjaan Soal 1
* Masuk ke direktori yang berisi file txt yang ingin direplace stringnya
  <img width="595" alt="Screenshot 2024-04-25 at 18 23 12" src="https://github.com/fioreenza/Sisop-2-2024-MH-IT25/assets/147926732/77be6988-f9e7-4dd6-b6d2-4bf74bd748d7">
* Menjalankan program virus dengan compile terlebih dahulu lalu dilanjutkan dengan command ./virus
  <img width="529" alt="Screenshot 2024-04-25 at 18 44 41" src="https://github.com/fioreenza/Sisop-2-2024-MH-IT25/assets/147926732/de59d370-c199-4609-aecf-f8dd0060da9d">
* Masuk kembali ke direktori yang berisi file txt untuk mengecek apakah stringnya sudah direplace atau belum
  <img width="714" alt="Screenshot 2024-04-25 at 18 31 32" src="https://github.com/fioreenza/Sisop-2-2024-MH-IT25/assets/147926732/5298cce6-3bfb-430d-9aea-e3b16a61afdb">
* Lalu mengecek virus.log untuk memastikan string diganti tiap 15 detik
  <img width="691" alt="Screenshot 2024-04-25 at 18 42 56" src="https://github.com/fioreenza/Sisop-2-2024-MH-IT25/assets/147926732/151eaff4-9729-4e52-9978-87a32add94ba">

## Soal 2

**oleh Tio Axellino Irin (5027231065)**

### Deskripsi Soal 2

Paul adalah seorang mahasiswa semester 4 yang diterima magang di perusahaan XYZ. Pada hari pertama magang, ia diberi tugas oleh atasannya untuk membuat program manajemen file sederhana. Karena kurang terbiasa dengan bahasa C dan environment Linux, ia meminta bantuan kalian untuk mengembangkan program tersebut.

a. Atasannya meminta agar program tersebut dapat berjalan secara daemon dan dapat mengunduh serta melakukan unzip terhadap file [berikut](https://drive.google.com/file/d/1rUIZmp10lXLtCIH3LAZJzRPeRks3Crup/view?usp=sharing). Atasannya juga meminta program ini dibuat **tanpa menggunakan command system()**

b. Setelah ditelusuri, ternyata hanya 6 file teratas yang nama filenya tidak dienkripsi. Oleh karena itu, bantulah Paul untuk melakukan dekripsi terhadap nama file ke-7 hingga terakhir menggunakan algoritma ROT19

c. Setelah dekripsi selesai, akan terlihat bahwa setiap file memuat salah satu dari kode berikut: **r3N4mE**, **d3Let3**, dan **m0V3**. Untuk setiap file dengan nama yang memuat kode **d3Let3**, hapus file tersebut. Sementara itu, untuk setiap file dengan nama yang memuat kode **r3N4mE**, lakukan hal berikut:

* Jika ekstensi file adalah “.ts”, rename filenya menjadi “helper.ts”

* Jika ekstensi file adalah “.py”, rename filenya menjadi “calculator.py”

* Jika ekstensi file adalah “.go”, rename filenya menjadi “server.go”

* Jika file tidak memuat salah satu dari ekstensi diatas, rename filenya menjadi “renamed.file”

d. Atasan Paul juga meminta agar program ini dapat membackup dan merestore file. Oleh karena itu, bantulah Paul untuk membuat program ini menjadi 3 mode, yaitu:

* `default`: program berjalan seperti biasa untuk me-rename dan menghapus file. Mode ini dieksekusi ketika program dijalankan tanpa argumen tambahan, yaitu dengan command **./management** saja

* `backup`: program memindahkan file dengan kode **m0V3** ke sebuah folder bernama “backup”

* `restore`: program mengembalikan file dengan kode **m0V3** ke folder sebelum file tersebut dipindahkan

* Contoh penggunaan: ./management -m backup

e. Terkadang, Paul perlu mengganti mode dari program ini tanpa menghentikannya terlebih dahulu. Oleh karena itu, bantulan Paul untuk mengintegrasikan kemampuan untuk mengganti mode ini dengan mengirim sinyal ke daemon, dengan ketentuan:

* SIGRTMIN untuk mode `default`

* SIGUSR1 untuk mode `backup`

* SIGUSR2 untuk mode `restore`

* Contoh penggunaan: kill -SIGUSR2 <pid_program>

f. Program yang telah dibuat ini tidak mungkin akan dijalankan secara terus-menerus karena akan membebani sistem. Maka dari itu, bantulah Paul untuk membuat program ini dapat dimatikan dengan aman dan efisien

g. Terakhir, program ini harus berjalan setiap detik dan mampu mencatat setiap peristiwa yang terjadi ke dalam file .log yang bernama “history.log” dengan ketentuan:

* Format: **[nama_user][HH:MM:SS] - <nama_file> - <action>**

* nama_user adalah username yang melakukan action terhadap file

* Format `action` untuk setiap kode:

    1. kode `r3N4mE`: Successfully renamed.

    2. kode `d3Let3`: Successfully deleted.

    3. mode `backup`: Successfully moved to backup.

    4. mode `restore`: Successfully restored from backup.

* Contoh pesan log:

    [paul][00:00:00] - r3N4mE.ts - Successfully renamed.

    [paul][00:00:00] - m0V3.xk1 - Successfully restored from backup.

Berikut adalah struktur folder untuk pengerjaan nomor 2:

    soal_2/
    ├── history.log
    ├── management.c
    └── library/
        └── backup/

### Penyelesaian Soal 2

* **management.c**

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

- **Penjelasan**

- Fungsi decryptFileName

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

Fungsi `decryptFileName` bertanggung jawab untuk mendekripsi nama file dengan menggunakan algoritma Caesar cipher. Caesar cipher adalah metode enkripsi sederhana yang menggeser setiap karakter dalam teks sejauh sejumlah langkah tertentu dalam abjad. Fungsi ini menerima parameter `fileName`, yang merupakan string yang akan didekripsi. Dalam loop `for`, setiap karakter dalam `fileName` diperiksa satu per satu. Jika karakter adalah huruf kapital (`A` sampai `Z`), maka karakter tersebut didekripsi dengan menggeser mundur 7 langkah dalam abjad. Hasilnya akan tetap dalam rentang huruf kapital. Jika karakter adalah huruf kecil (`a` sampai `z`), maka karakter tersebut didekripsi dengan menggeser mundur 7 langkah dalam abjad. Hasilnya akan tetap dalam rentang huruf kecil.

- Fungsi moveFile

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

`void moveFile(const char *src, const char *dest)`: Deklarasi fungsi `moveFile` yang menerima dua parameter, yaitu `src` (lokasi file sumber) dan `dest` (lokasi tujuan).

`if (access(src, F_OK) != -1)`: Periksa apakah file sumber (`src`) ada dengan menggunakan fungsi `access`. Fungsi ini mengembalikan `-1` jika file tidak ada dan `0` jika ada.

`if (rename(src, dest) != 0)`: Jika file sumber ada, maka coba untuk memindahkan file menggunakan fungsi `rename`. Fungsi ini akan mengembalikan `0` jika pemindahan berhasil.

`perror("Error moving file");`: Jika pemindahan gagal, cetak pesan kesalahan yang dihasilkan oleh sistem menggunakan `perror`.

`exit(EXIT_FAILURE);`: Keluar dari program dengan kode kesalahan (`EXIT_FAILURE`).                                 

- Fungsi createBackupDirectory

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

`void createBackupDirectory()`: Deklarasi fungsi `createBackupDirectory` tanpa parameter.

`struct stat st = {0};`: Deklarasi struktur `st` dari tipe `struct stat` yang digunakan untuk menyimpan informasi tentang direktori.

`if (stat("/home/ludwigd/SisOP/Praktikum2/nomor2/library/backup", &st) == -1)`: Memeriksa apakah direktori backup sudah ada dengan menggunakan fungsi `stat`. Fungsi ini mengembalikan `-1` jika direktori tidak ada.

`if (mkdir("/home/ludwigd/SisOP/Praktikum2/nomor2/library/backup", 0700) == -1)`: Jika direktori backup belum ada, maka buat direktori baru dengan menggunakan fungsi `mkdir`. Mode akses `0700` digunakan untuk memberikan hak akses penuh kepada pemilik direktori.

`perror("Error creating backup directory");`: Jika pembuatan direktori gagal, cetak pesan kesalahan yang dihasilkan oleh sistem menggunakan `perror`.

`exit(EXIT_FAILURE);`: Keluar dari program dengan kode kesalahan (`EXIT_FAILURE`).

- Fungsi signalHandler

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

`void signalHandler(int signum)`: Deklarasi fungsi `signalHandler` dengan satu parameter, yaitu `signum` yang menunjukkan nomor sinyal yang diterima.

`if (signum == SIGRTMIN)`: Periksa apakah sinyal yang diterima adalah `SIGRTMIN` (Real-time signal minimum).

`mode = 0;`: Jika sinyal adalah `SIGRTMIN`, atur mode program ke mode default (0).
else if (signum == SIGUSR1): Periksa apakah sinyal yang diterima adalah SIGUSR1 (User-defined signal 1).

`mode = 1;`: Jika sinyal adalah `SIGUSR1`, atur mode program ke mode backup (1).

`else if (signum == SIGUSR2)`: Periksa apakah sinyal yang diterima adalah `SIGUSR2` (User-defined signal 2).

`mode = 2;`: Jika sinyal adalah `SIGUSR2`, atur mode program ke mode restore (2).

`else if (signum == SIGTERM || signum == SIGINT)`: Periksa apakah sinyal yang diterima adalah `SIGTERM` (Termination signal) atau `SIGINT` (Interrupt signal).

`running = 0;`: Jika sinyal adalah `SIGTERM` atau `SIGINT`, atur variabel `running` menjadi `0` untuk menghentikan eksekusi program.

- Fungsi logEvent

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

`void logEvent(const char *user, const char *file, const char *action)`: Deklarasi fungsi `logEvent` dengan tiga parameter, yaitu `user` (nama pengguna), `file` (nama file), dan `action` (tindakan yang dilakukan).

`FILE *logfile = fopen("/home/ludwigd/SisOP/Praktikum2/nomor2/history.log", "a");`: Membuka file log untuk ditulis dengan mode "a" (append), yang berarti menambahkan teks baru ke akhir file.

`if (logfile == NULL)`: Memeriksa apakah file log berhasil dibuka.

`perror("Failed to open log file");`: Jika file log gagal dibuka, cetak pesan kesalahan yang dihasilkan oleh sistem menggunakan `perror`.

`exit(EXIT_FAILURE);`: Keluar dari program dengan kode kesalahan (`EXIT_FAILURE`).

`time_t now;`: Deklarasi variabel `now` dari tipe `time_t` untuk menyimpan waktu saat ini.

`time(&now);`: Mendapatkan waktu saat ini dan menyimpannya ke dalam variabel `now`.

`struct tm *timeinfo = localtime(&now);`: Mengonversi waktu dalam bentuk `time_t` ke dalam struktur `tm` menggunakan fungsi `localtime`, yang menyediakan informasi waktu lokal.

`char time_str[9];`: Deklarasi array `time_str` untuk menyimpan string waktu dengan panjang 9 karakter.

`strftime(time_str, sizeof(time_str), "%H:%M:%S", timeinfo);`: Mengonversi waktu ke dalam format string dengan format [HH:MM:SS] menggunakan fungsi `strftime`.

`fprintf(logfile, "[%s][%s] - %s - %s\n", user, time_str, file, action);`: Menulis pesan log ke file menggunakan fungsi `fprintf`, dengan format [user][waktu] - file - action.

`fclose(logfile);`: Menutup file log setelah selesai menulis pesan log.

- Fungsi usage

        void usage()
        {
            printf("Usage: ./management [-m mode]\n");
            printf("Modes:\n");
            printf("  default : Run in default mode\n");
            printf("  backup  : Run in backup mode\n");
            printf("  restore : Run in restore mode\n");
        }

`void usage()`: Deklarasi fungsi `usage` tanpa parameter.

`printf("Usage: ./management [-m mode]\n");`: Menampilkan pesan untuk memberikan penggunaan program kepada pengguna. Pesan ini menjelaskan bagaimana cara menggunakan program, khususnya argumen baris perintah `-m`.

`printf("Modes:\n");`: Menampilkan pesan untuk memberikan informasi tentang mode-mode yang dapat digunakan dalam program.

`printf(" default : Run in default mode\n");`: Menampilkan pesan yang menjelaskan bahwa mode default adalah mode di mana program berjalan tanpa argumen baris perintah.

`printf(" backup : Run in backup mode\n");`: Menampilkan pesan yang menjelaskan bahwa mode backup adalah mode di mana program berjalan dengan argumen baris perintah `-m backup`.

`printf(" restore : Run in restore mode\n");`: Menampilkan pesan yang menjelaskan bahwa mode restore adalah mode di mana program berjalan dengan argumen baris perintah `-m restore`.

- Fungsi main

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

**Signal Handling**: Pada bagian ini, signal handling ditetapkan untuk menangani beberapa jenis sinyal, yaitu `SIGRTMIN`, `SIGUSR1`, `SIGUSR2`, `SIGTERM`, dan `SIGINT`. Ketika sinyal-sinyal ini diterima, fungsi `signalHandler` akan dipanggil untuk menangani sinyal tersebut.

**Forking Process**: Program melakukan fork untuk menciptakan child process. Fork menghasilkan dua proses yang identik, kecuali pada nilai pengembalian. Child process akan memiliki nilai pengembalian 0, sementara parent process akan memiliki nilai pengembalian PID dari child process. Fork digunakan untuk membuat daemon process, yang berjalan di background.

**Exit on Fork Failure**: Jika fork gagal (nilai pid kurang dari 0), program akan keluar dengan kode kesalahan menggunakan `exit(EXIT_FAILURE)`.

**Exit on Parent Process**: Jika fork berhasil dan program berjalan di parent process (nilai pid lebih dari 0), parent process akan langsung keluar menggunakan `exit(EXIT_SUCCESS)`. Ini dilakukan agar parent process segera menyelesaikan eksekusi dan mengizinkan child process untuk menjadi daemon.

**Setting Umask**: Umask diatur ke nilai 0 untuk memastikan bahwa file yang dibuat oleh daemon memiliki izin penuh.

**Setsid**: Child process membuat sesi baru menggunakan `setsid()`. Ini memisahkan proses dari terminal dan menghindari kemungkinan terminal mengirim sinyal hangup (SIGHUP) ke proses jika terminal ditutup.

**Change Directory**: Child process berpindah direktori kerja ke root direktori (/) menggunakan `chdir("/")`. Ini dilakukan untuk memastikan bahwa daemon tidak bergantung pada direktori kerja yang spesifik.

**Close Standard File Descriptors**: Child process menutup file descriptor standar (stdin, stdout, stderr) menggunakan `close(STDIN_FILENO)`, `close(STDOUT_FILENO)`, dan `close(STDERR_FILENO)`. Ini dilakukan untuk memastikan bahwa daemon tidak memiliki koneksi dengan terminal atau output yang terkait dengannya.

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

**Check if mode is default**: Program pertama-tama memeriksa apakah tidak ada argumen baris perintah yang diberikan (`argc == 1`). Jika ini benar, maka mode program diatur ke mode default (`mode = 0`).

**Check if mode is specified**: Jika jumlah argumen baris perintah adalah 2 dan argumen pertama adalah `-m`, maka program akan memeriksa argumen kedua (`argv[2]`) untuk menentukan mode yang diinginkan.

a. Jika argumen kedua adalah `"default"`, maka mode program diatur ke mode `default (mode = 0)`.

b. Jika argumen kedua adalah `"backup"`, maka mode program diatur ke mode `backup (mode = 1)`.

c. Jika argumen kedua adalah `"restore"`, maka mode program diatur ke mode restore `(mode = 2)`.

**Invalid Mode or Arguments**: Jika argumen baris perintah tidak memenuhi syarat di atas, program akan mencetak pesan kesalahan yang menyatakan bahwa mode yang ditentukan tidak valid. Selanjutnya, program akan menampilkan pesan penggunaan menggunakan fungsi `usage()`, dan keluar dengan kode kesalahan menggunakan `exit(EXIT_FAILURE)`.

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

**While Loop**: Loop ini akan berjalan terus-menerus selama nilai ekspresi `1` benar, yang berarti selamanya.

**Check Mode**: Program memeriksa nilai variabel `mode` untuk menentukan mode kerja yang sedang berlangsung. Jika mode adalah `0` (mode default), maka program akan melakukan pengunduhan file.

**Fork Child Process**: Program melakukan fork untuk membuat child process. Child process akan bertanggung jawab untuk menjalankan perintah unduh.

**Child Process**: Bagian ini adalah bagian yang dijalankan oleh child process. Child process menggunakan `execvp()` untuk menjalankan perintah unduh dengan `wget` menggunakan argumen yang telah ditetapkan sebelumnya. Jika `execvp()` berhasil, child process akan keluar dengan `exit(EXIT_SUCCESS)`. Hal ini dilakukan agar child process tidak melanjutkan eksekusi program utama.

**Fork Failure Handling**: Jika proses fork gagal (nilai pid kurang dari 0), program akan mencatat pesan kesalahan menggunakan `syslog()` dengan level `LOG_ERR`.

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

**Parent Process**: Program memasuki blok ini sebagai parent process setelah child process untuk unduhan selesai dieksekusi.

**Wait for Child Process**: Program menggunakan fungsi `waitpid()` untuk menunggu child process selesai. Ini memungkinkan program untuk menunggu proses unduhan selesai sebelum melanjutkan ke langkah berikutnya. Fungsi `waitpid()` akan memblokir parent process hingga child process selesai dieksekusi.

**Check Child Process Status**: Setelah child process selesai dieksekusi, program memeriksa status keluaran (exit status) dari child process menggunakan makro `WIFEXITED()` dan `WEXITSTATUS()`. Jika child process keluar secara normal (exit), dan status keluarannya adalah 0, maka program mencatat pesan informasi bahwa unduhan berhasil. Jika tidak, program mencatat pesan kesalahan bahwa unduhan gagal.

**Fork untuk Proses Unzip**: Setelah proses unduhan selesai, program melakukan fork untuk membuat child process yang bertanggung jawab untuk mengekstrak (unzip) file yang telah diunduh.

**Child Process untuk Unzip**: Bagian ini adalah bagian yang dijalankan oleh child process untuk mengekstrak file. Child process menggunakan `execvp()` untuk menjalankan perintah unzip dengan argumen yang telah ditetapkan sebelumnya. Jika `execvp()` berhasil, child process akan keluar dengan `exit(EXIT_SUCCESS)`. Hal ini dilakukan agar child process tidak melanjutkan eksekusi program utama.

**Fork Failure Handling**: Jika proses fork gagal (nilai pid kurang dari 0), program akan mencatat pesan kesalahan menggunakan `syslog()` dengan level `LOG_ERR`.

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

**Parent Process**: Program memasuki blok ini sebagai parent process setelah child process untuk mengekstrak selesai dieksekusi.

**Wait for Child Process**: Program menggunakan fungsi `waitpid()` untuk menunggu child process selesai. Ini memungkinkan program untuk menunggu proses mengekstrak selesai sebelum melanjutkan ke langkah berikutnya. Fungsi `waitpid()` akan memblokir parent process hingga child process selesai dieksekusi.

**Check Child Process Status**: Setelah child process selesai dieksekusi, program memeriksa status keluaran (exit status) dari child process menggunakan makro `WIFEXITED()` dan `WEXITSTATUS()`. Jika child process keluar secara normal (exit), dan status keluarannya adalah 0, maka program mencatat pesan informasi bahwa file berhasil diekstrak. Jika tidak, program mencatat pesan kesalahan bahwa proses unzip gagal.

**Fork untuk Proses Dekripsi Nama File**: Setelah proses unzip selesai, program melakukan fork untuk membuat child process yang bertanggung jawab untuk mendekripsi nama file.

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

**Pengecekan Child Process**: Bagian ini hanya akan dieksekusi oleh child process, karena pernyataan `decrypt_child_pid == 0` mengevaluasi ke benar hanya jika child process sedang berjalan.

**Mendapatkan Daftar File di Direktori**: Program membuka direktori yang ditentukan dengan menggunakan fungsi `opendir()` dan menyimpan struktur direktori dalam variabel `dir`. Struktur dirent akan digunakan untuk membaca setiap entri (file) dalam direktori.

**Loop untuk Dekripsi Nama File**: Program melakukan loop menggunakan `readdir()` untuk membaca setiap entri dalam direktori. Hanya file-file yang diproses dalam loop ini, karena direktori "." dan ".." dilewati dengan menggunakan fungsi `strcmp()` untuk memeriksa nama file.

**Pengecekan untuk Dekripsi**: Setelah file dipilih, program memeriksa apakah nama file dimulai dengan karakter numerik atau tidak. Jika iya, file tersebut dilewati.

**Pengolahan Nama File**: Setelah file dipilih untuk diproses, program membuat string yang berisi path lengkap dari file lama (`old_name`) dan path lengkap baru (`new_name`). Nama file dienkripsi menggunakan fungsi `decryptFileName()`. Setelah itu, nama file yang sudah dienkripsi digunakan untuk mengubah nama file lama menjadi nama file baru menggunakan fungsi `rename()`.

**Menutup Direktori**: Setelah semua file dalam direktori diproses, program menutup direktori menggunakan fungsi `closedir()` untuk membersihkan sumber daya yang digunakan.

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

**Kode untuk Child Process yang Gagal Membuka Direktori**: Jika child process tidak dapat membuka direktori yang ditentukan, program mencatat pesan kesalahan menggunakan fungsi `syslog()` dengan level `LOG_ERR` dan keluar dari child process dengan status keluaran `EXIT_FAILURE`. Ini menandakan bahwa proses pendekripsi gagal dan program akan berhenti.

**Keluar dari Child Process Setelah Selesai**: Jika child process berhasil membuka dan memproses direktori, program keluar dari child process dengan status keluaran `EXIT_SUCCESS`. Ini menandakan bahwa proses pendekripsi selesai tanpa masalah.

**Pengecekan Fork Failure**: Jika gagal melakukan fork untuk child process pendekripsi, program mencatat pesan kesalahan menggunakan `syslog()` dan keluar dari child process dengan status keluaran `EXIT_FAILURE`.

**Parent Process**: Bagian ini akan dieksekusi oleh parent process setelah child process pendekripsi selesai dieksekusi.

**Menunggu Child Process Selesai**: Parent process menunggu child process pendekripsi selesai menggunakan fungsi `waitpid()`. Ini memungkinkan parent process untuk menunggu hasil eksekusi dari child process sebelum melanjutkan ke langkah berikutnya.

**Pengecekan Status Keluaran Child Process**: Setelah child process selesai dieksekusi, parent process memeriksa status keluaran child process menggunakan makro `WIFEXITED()` dan `WEXITSTATUS()`. Jika child process keluar secara normal dan status keluarannya adalah 0, program mencatat pesan informasi bahwa nama file telah berhasil dideskripsi. Jika tidak, program mencatat pesan kesalahan bahwa proses pendekripsi gagal.

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

**Membuka Direktori**: Program membuka direktori yang berisi file yang akan diproses. Jika direktori berhasil dibuka, maka program akan melanjutkan proses.

**Iterasi Melalui Setiap File dalam Direktori**: Program melakukan iterasi melalui setiap file yang ada dalam direktori yang telah dibuka.

**Pengecekan Kode "r3N4mE" dalam Nama File**: Setiap nama file yang diiterasi diperiksa untuk memastikan apakah mereka mengandung kode "r3N4mE" di dalamnya. Jika iya, maka file tersebut akan diproses.

**Penentuan Ekstensi File dan Penamaan Baru**: Program menentukan ekstensi file dengan menggunakan fungsi `strrchr()` untuk mencari titik terakhir dalam nama file. Kemudian, berdasarkan ekstensi file tersebut, program menentukan nama baru untuk file tersebut.

**Pengubahan Nama File**: Setelah menentukan nama baru untuk file, program menggunakan fungsi `rename()` untuk mengubah nama file yang lama menjadi nama baru yang telah ditentukan.

**Logging Event**: Setelah pengubahan nama file, program memanggil fungsi `logEvent()` untuk mencatat kegiatan yang telah dilakukan dalam log. Ini memberikan informasi bahwa file telah berhasil direname.

**Penutupan Direktori**: Setelah selesai mengiterasi semua file dalam direktori, program menutup direktori untuk membersihkan sumber daya yang digunakan.

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

**Fork Process**: Program melakukan fork untuk membuat child process yang akan melakukan operasi penghapusan file.

**Child Process**: Child process melakukan langkah-langkah berikut:

- Membuka direktori yang berisi file yang akan dihapus.

- Melakukan iterasi melalui setiap file dalam direktori.

- Setiap nama file yang diiterasi diperiksa untuk memastikan apakah mereka mengandung string "d3Let3". Jika iya, maka file tersebut akan dihapus.

- Setelah file dihapus, sebuah log event dicatat menggunakan fungsi `logEvent()` untuk mencatat kegiatan penghapusan file.

**Penutupan Direktori**: Setelah selesai mengiterasi semua file dalam direktori, program menutup direktori untuk membersihkan sumber daya yang digunakan.

**Penanganan Kesalahan**: Jika ada kesalahan dalam membuka atau mengakses direktori, program akan mencatat kesalahan tersebut dalam log dan keluar dari child process dengan status kegagalan.

**Exit Child Process**: Setelah menyelesaikan tugasnya, child process keluar dengan status keberhasilan.

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

**Penanganan Child Process Failure**: Jika fork gagal untuk membuat child process yang bertanggung jawab untuk menghapus file, maka akan dicatat pesan kesalahan dalam log.

**Penanganan Parent Process**: Jika fork berhasil, parent process akan menunggu child process selesai dengan menggunakan `waitpid()`. Setelah child process selesai, parent process akan memeriksa status keluaran dari child process.

- Jika child process selesai dengan status keluaran yang menandakan keberhasilan (`exit status 0`), maka akan dicatat dalam log bahwa file-file yang mengandung string "`d3Let3`" berhasil dihapus.

- Jika child process selesai dengan status keluaran yang menandakan kegagalan (`exit status bukan 0`), maka akan dicatat dalam log bahwa proses penghapusan file yang mengandung string "`d3Let3`" gagal.

.

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

**Pembuatan Direktori Cadangan**: Fungsi `createBackupDirectory()` dipanggil untuk memastikan bahwa direktori cadangan sudah dibuat sebelum memulai proses backup.

**Pencarian File**: Program membuka direktori yang berisi file-file untuk dicadangkan. Jika direktori berhasil dibuka, program akan memproses setiap file dalam direktori tersebut.

**Pemindaian File**: Setiap file dalam direktori diproses. Jika nama file mengandung string "`m0V3`", file tersebut akan dipindahkan ke dalam direktori cadangan.

**Pemindahan File**: Setiap file yang memenuhi syarat dipindahkan ke direktori cadangan dengan menggunakan fungsi `moveFile()`. Selain itu, log peristiwa dicatat untuk setiap file yang berhasil dipindahkan.

**Penanganan Kesalahan**: Jika program gagal membuka direktori, sebuah pesan kesalahan akan dicatat dalam log dan program akan keluar dengan status kegagalan.

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

**Pencarian File**: Program membuka direktori "`backup`" untuk mencari file-file yang akan dipulihkan. Jika direktori berhasil dibuka, program akan memproses setiap file dalam direktori tersebut.

**Pemindaian File**: Setiap file dalam direktori "`backup`" diproses. Jika nama file mengandung string "`m0V3`", file tersebut akan dipindahkan kembali ke dalam direktori "`library`".

**Pemindahan File**: Setiap file yang memenuhi syarat dipindahkan kembali ke direktori "`library`" dengan menggunakan fungsi `moveFile()`. Selain itu, log peristiwa dicatat untuk setiap file yang berhasil dipulihkan.

**Penanganan Kesalahan**: Jika program gagal membuka direktori "`backup`", sebuah pesan kesalahan akan dicatat dalam log dan program akan keluar dengan status kegagalan.

**Pemrosesan Berulang**: Setelah semua file yang cocok dipindahkan kembali, program akan menunggu selama 30 detik sebelum memulai pemindaian ulang. Ini memungkinkan program untuk terus berjalan secara berulang dalam mode pemulihan.

**Pesan Terminasi**: Ketika program berakhir (dalam kasus tertentu, karena sinyal atau kondisi tertentu), pesan "`Program terminated.`" akan dicetak sebelum program mengembalikan nilai 0 dan berhenti.


### Kendala Pengerjaan Soal 2

Sebelum kode dimodifikasi untuk bisa menggunakan mode dengan mengirim sinyal ke daemon seperti SIGRTMIN, SIGUSR1, dan SIGUSR2, program tersebut berhasil menggunakan mode dengan perintah `./management -m backup` dan `./management -m restore` untuk melakukan backup dan restore. 

Tetapi setelah dimodifikasi agar bisa menggunakan mode dengan mengirim sinyal ke daemon, program tersebut bisa lagi untuk menggunakan mode dengan perintah `./management -m backup` dan `./management -m restore` untuk melakukan backup dan restore walaupun program bisa menggunakan mode dengan mengirim sinyal ke daemon.

- **Screenshoot problem**

![image](https://github.com/fioreenza/Sisop-2-2024-MH-IT25/assets/144349814/26653820-5923-4499-9137-a1a83e2def34)

Terlihat dari gambar di atas bahwa program tidak berhasil menggunakan mode dengan perintah `-m backup` maupun `-m restore`. Tetapi program tersebut bisa menggunakan mode dengan hanya mengirim sinyal ke daemon yaitu dengan kill -SIGUSR <PID program>, kill -SIGUSR2 <PID program>, dan kill -SIGRTMIN <PID program>




### Screenshot Hasil Pengerjaan Soal 2

- Jalankan program management nya
  
  ![image](https://github.com/fioreenza/Sisop-2-2024-MH-IT25/assets/144349814/94b743e7-9ec7-432e-a967-2501a03d80db)

- Program telah berhasil mendownload, unzip, decrypt file, serta berhasil merename nama file tersebut sesuai dengan nama yang memuat kode r3N4mE, d3Let3, dan m0V3.

![image](https://github.com/fioreenza/Sisop-2-2024-MH-IT25/assets/144349814/bd613ec6-713d-4bb0-b9b8-8756b2207fd7)

- Program telah berhasil memindahkan file-file yang nama file nya mengandung kode m0V3 ke directory backup menggunakan mode dengan mengirim sinyal ke daemon yaitu kill -SIGUSR1 <PID program>

![image](https://github.com/fioreenza/Sisop-2-2024-MH-IT25/assets/144349814/ef6433f1-fc48-414a-aac5-bebcf5c9458f)

- Program telah berhasil memindahkan kembali file-file yang berada di directory backup, ke directory asalnya yaitu directory library menggunakan mode dengan mengirim sinyal ke daemon yaitu kill -SIGUSR2 <PID program>

![image](https://github.com/fioreenza/Sisop-2-2024-MH-IT25/assets/144349814/226b8d25-9bfc-459b-82fc-bdd8ac5508b9)

- Setelah menghapus file library.zip dan directory library, program tersebut berhasil mendownload, unzip, decrypt serta merename kembali file-file tersebut menggunakan mode dengan mengirim sinyal ke daemon yaitu -kill SIGRTMIN <PID program>

![image](https://github.com/fioreenza/Sisop-2-2024-MH-IT25/assets/144349814/1a5ff558-7168-40ab-af9a-40fd750e64cb)

- Berikut adalah isi dari file history.log

![image](https://github.com/fioreenza/Sisop-2-2024-MH-IT25/assets/144349814/513f2872-0e8e-4bb1-b623-daa3d5cc6f2e)



## Soal 3
**Wira Samudra Siregar (5027231041)**
### Deskripsi Soal 3
3. Pak Heze adalah seorang admin yang baik. Beliau ingin membuat sebuah program admin yang dapat memantau para pengguna sistemnya. Bantulah Pak Heze untuk membuat program  tersebut!
   1. Nama program tersebut dengan nama admin.c
   2. Program tersebut memiliki fitur menampilkan seluruh proses yang dilakukan oleh seorang user dengan menggunakan command:./admin <user>
   3. Program dapat memantau proses apa saja yang dilakukan oleh user. Fitur ini membuat program berjalan secara daemon dan berjalan terus menerus. Untuk menjalankan fitur ini menggunakan command: ./admin -m <user>
   Dan untuk mematikan fitur tersebut menggunakan: ./admin -s <user>
   Program akan mencatat seluruh proses yang dijalankan oleh user di file <user>.log dengan format:[dd:mm:yyyy]-[hh:mm:ss]_pid-process_nama-process_GAGAL/JALAN
   4.Program dapat menggagalkan proses yang dijalankan user setiap detik secara terus menerus dengan menjalankan: ./admin -c user
   sehingga user tidak bisa menjalankan proses yang dia inginkan dengan baik. Fitur ini dapat dimatikan dengan command: ./admin -a user
   5. Ketika proses yang dijalankan user digagalkan, program juga akan melog dan menset log tersebut sebagai GAGAL. Dan jika di log menggunakan fitur poin c, log akan ditulis dengan JALAN

### Penyelesaian Soal 3 
    
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
### Penjelasan
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
*Fungsi showProcesses(char user)

Membuat array args yang berisi argumen untuk perintah ps -u user
Melakukan fork untuk membuat proses baru.
Jika fork berhasil dan merupakan proses anak, maka execvp("ps", args) akan dijalankan untuk menampilkan daftar proses.
Jika fork gagal, akan menampilkan pesan error fork.
Jika fork berhasil dan merupakan proses induk, maka akan menunggu proses anak selesai dengan waitpid.

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
*Fungsi daemonProcess(char user)

Melakukan fork untuk membuat proses baru.

Jika merupakan proses anak, maka:

Membuat session ID baru dengan setsid.
Menutup STDIN, STDOUT, dan STDERR.
Membuat file log dengan nama user.log dan membukanya untuk ditulis.


Melakukan perulangan tak terhingga:

Membuat array args yang berisi argumen untuk perintah ps -u user.
Melakukan fork untuk membuat proses baru.
Jika merupakan proses anak, maka execvp("ps", args) akan dijalankan dengan STDOUT dan STDERR diarahkan ke /dev/null.
Jika fork gagal, akan menampilkan pesan error fork.
Jika fork berhasil dan merupakan proses induk, maka akan menunggu proses anak selesai dengan waitpid.
Mencatat hasil eksekusi ps ke dalam file log dengan format: [timestamp]-[pid]-[user]_(JALAN|GAGAL).
Menunggu 1 detik sebelum melanjutkan perulangan.
Menutup file log setelah keluar dari perulangan.

    char *allowedProcesses[] = {"init", "systemd", "bash", "pkill", NULL};
*Array allowedProcesses

Array yang berisi nama-nama proses yang diizinkan untuk berjalan, seperti init, systemd, bash, dan pkill.

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
*Fungsi killUserProcessesContinuously(char user)

Melakukan perulangan tak terhingga:

Mendapatkan daftar PID proses yang sedang berjalan untuk pengguna tertentu dengan perintah pgrep -u user.

Untuk setiap PID:

Mendapatkan nama proses dengan perintah ps -p PID -o cmd=.
Memeriksa apakah nama proses ada dalam daftar allowedProcesses.
Jika tidak ada, maka membunuh proses tersebut dengan kill(pid, SIGKILL).
Menunggu 1 detik sebelum melanjutkan perulangan.

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
*Fungsi stopKillUserProcessesContinuously(char user)

Mendapatkan daftar PID proses yang menjalankan killUserProcessesContinuously untuk pengguna tertentu dengan perintah pgrep -f 'killUserProcessesContinuously user'.
Untuk setiap PID, membunuh proses tersebut dengan kill(pid, SIGKILL).

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
*Fungsi stopKillUserProcesses(char user)

Memanggil stopKillUserProcessesContinuously(user).
Membuat array args yang berisi argumen untuk perintah pkill -U user -f pkill.
Melakukan fork untuk membuat proses baru.
Jika merupakan proses anak, maka execvp("pkill", args) akan dijalankan untuk membunuh semua proses pkill yang sedang berjalan untuk pengguna tertentu.
Jika fork gagal, akan menampilkan pesan error fork.
Jika fork berhasil dan merupakan proses induk, maka akan menunggu proses anak selesai dengan waitpid.

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
*Fungsi main(int argc, char argv[])

Memeriksa jumlah argumen yang diberikan.
Jika argumen tidak valid, akan menampilkan petunjuk penggunaan.
Menyimpan nama pengguna dari argumen ketiga.


Memeriksa opsi yang diberikan dari argumen kedua:

Jika -m, maka memanggil daemonProcess(user).

Jika -s, perintah untuk mematikan daemon process(user).

Jika -c, maka memanggil killUserProcessesContinuously(user).

Jika -a, maka memanggil stopKillUserProcesses(user).

Jika hanya ada satu argumen (nama program dan pengguna), maka memanggil showProcesses(user).

Jika opsi tidak valid, akan menampilkan pesan error.

### Kendala Pengerjaan Soal 3
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
Pada program *Fungsi killUserProcessesContinuously() terdapat masalah saat menjalankan **sudo ./admin -c <user>** program tidak berhenti loop, bahkan saat sudah direvisi tetap tidak menemukan solusinya.

### Screenshot Kendala Soal 3
![image](https://github.com/fioreenza/Sisop-2-2024-MH-IT25/assets/145766477/4c3bc012-15f1-41e2-83f1-9db014c3ce0a)

### Screenshot Hasil Pengerjaan Soal 3
* Memanggil sudo ./admin wsamudraa, maka memanggil showProcesses(user).
![image](https://github.com/fioreenza/Sisop-2-2024-MH-IT25/assets/145766477/72a5ad3f-bc21-44fc-aadd-e212bfa1e77c)

* Memanggil sudo ./admin -m wsamudraa, maka memanggil daemonProcess(user). lalu kita cek dengan px aux | grep admin
![image](https://github.com/fioreenza/Sisop-2-2024-MH-IT25/assets/145766477/9ffe034c-c95e-43e1-87fb-2d280c0a175b).

* Memanggil sudo ./admin -s wsamudraa,  perintah untuk mematikan daemon process(user). lalu kita cek <user>.log
![image](https://github.com/fioreenza/Sisop-2-2024-MH-IT25/assets/145766477/cdbdacfc-d52c-4d13-b66b-e8579948d7c0).

* Memanggil sudo ./admin -c wsamudraa, maka memanggil killUserProcessesContinuously(user). tetapi program ini mengalami kendala.
![Screenshot 2024-04-27 184605](https://github.com/fioreenza/Sisop-2-2024-MH-IT25/assets/145766477/463a1c92-8e6e-4049-a837-b5e9e034d1c8).

* Memanggil sudo ./admin -a wsamudraa,  maka memanggil stopKillUserProcesses(user). tetapi program ini tidak bekerja karena program killUserProcessesContinuously(user) mengalami kendala sehingga berdampak kepada program selanjutnya.
![image](https://github.com/fioreenza/Sisop-2-2024-MH-IT25/assets/145766477/0b2da63e-fce5-4bde-ba7b-5bfd7247f9fd).

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
        pid_t pid ;
        
        pid = fork();
        
        if (pid < 0) {
            perror("Error forking");
            exit(EXIT_FAILURE);
        }
    
        if (pid > 0) {
            exit(EXIT_SUCCESS);
        }
    
        umask(0);

        sid = setsid();
        
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

  Program tidak berjalan secara daemon, untuk mengubah program menjadi sebuah daemon, code direvisi menjadi yang tertera di atas. Terdapat
  penambahan di awal bagian int main sebagai berikut :

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
* Compile program setup.c menggunakan command gcc
  
  <img width="710" alt="Screenshot 2024-04-25 at 22 17 09" src="https://github.com/fioreenza/Sisop-2-2024-MH-IT25/assets/147926732/7eed5ab1-decd-4fc6-87ec-97d2e80f9291">
* Jalankan command setup -o untuk membuka aplikasi yang diinginkan dan sesuai jumlah windows yang diinginkan
  
  <img width="694" alt="Screenshot 2024-04-25 at 22 17 22" src="https://github.com/fioreenza/Sisop-2-2024-MH-IT25/assets/147926732/155f91d8-eeb9-44bf-af73-9c9acbac3141">
* Jalankan command setup -f file.conf untuk membuka aplikasi yang diinginkan dan jumlah windows yang telah tertulis di file.conf
  
  <img width="794" alt="Screenshot 2024-04-25 at 22 19 09" src="https://github.com/fioreenza/Sisop-2-2024-MH-IT25/assets/147926732/a4399a80-eabc-46f3-9b8e-96a627b1bbda">
* Jalankan command setup -k untuk menutup semua windows dari aplikasi yang dibuka melalui setup -o ataupun melalui setup -f file.conf
  
  <img width="788" alt="Screenshot 2024-04-25 at 22 19 22" src="https://github.com/fioreenza/Sisop-2-2024-MH-IT25/assets/147926732/d6219d28-1d17-49cf-84b0-8bbb8eedd798">
* Buka kembali aplikasi melalui setup -o dan setup -f file.conf
  
  <img width="788" alt="Screenshot 2024-04-25 at 22 19 49" src="https://github.com/fioreenza/Sisop-2-2024-MH-IT25/assets/147926732/587d3146-3a65-4a1b-bb17-fefda0fb13cd">
* Lalu jalankan command setup -k file.conf untuk menutup aplikasi yang dibuka melalui setup -f file.conf saja. Disini terlihat hanya 2 windows wireshark dan 2 windows firefox yang tertutup (sesuai dengan aplikasi dan jumlah windows yang tertulis di file.conf)
  
  <img width="792" alt="Screenshot 2024-04-25 at 22 19 59" src="https://github.com/fioreenza/Sisop-2-2024-MH-IT25/assets/147926732/56bb8c33-c21d-4fbe-a405-71e1757e1fd6">
