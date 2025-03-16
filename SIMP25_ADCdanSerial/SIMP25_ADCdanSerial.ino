/*
Sistem Instrumentasi dan Metode Pengukuran - 2025

Program untuk melakukan konversi A/D (Analog to Digital Conversion
atau ADC) dari output signal conditioning dan menampilkannya di PC
melalui komunikasi Serial. Bacaan-Bacaan ADC juga akan dibuatkan
nilai Moving Average-nya untuk meminimalisir noise pada sinyal kita.

Di program ini, kita dapat set berapa periode/selang waktu dari konversi
A/D (ADC) dan berapa periode/selang waktu pengiriman data adc terata-rata
ke PC melalui komunikasi serial melalui setting definisi "SELANG_WAKTU_ADC"
dan "SELANG_WAKTU_SERIAL". Di contoh ini, selang waktu adc bernilai 100 ms 
dan selang waktu serial bernilai 1000 ms sehingga Arduino Uno akan melakukan
konversi adc setiap 100 ms sekali dan Arduino Uno akan menghitung Moving Avg
dan mengomunikasikannya dengan PC kita setiap 1000 ms sekali. Di sini, 
digunakan fungsi millis() untuk mendapatkan elapsed time dari Arduino Uno 
dalam millisecond.
*/

// Definisi
#define SELANG_WAKTU_ADC 100       // Selang waktu konversi A/D dalam ms.
#define SELANG_WAKTU_SERIAL 1000   // Selang waktu logging hasil bacaan ADC rata-rata melalui komunikasi serial.

/* Deklarasi Variabel */
unsigned int   ui_ADC[10];         // Array untuk menyimpan Bacaan ADC oleh Arduino dengan banyaknya elemen sebesar 10 buah.
float f_ADCMovAvg;                 // Hasil Moving Average dari bacaan ADC. Hasil dihitung dengan meratakan array adc.
unsigned char  uc_arrayIndex;      // Indeks dari array adc.

unsigned long  ul_ADCTimeMS;       // Cek point pewaktu untuk ADC dalam millisecond.
unsigned long  ul_SerialTimeMS;    // Cek point pewaktu untuk komunikasi serial.

void setup() {
     // Inisiasi variabel
     f_ADCMovAvg = 0;
     uc_arrayIndex = 0;
     ul_ADCTimeMS = 0;
     ul_SerialTimeMS = 0;

     // Inisiasi Komunikasi Serial Arduino-PC
     Serial.begin(9600);                               // Komunikasi Serial dengan Baud Rate 9600
     Serial.println("SIMP - 2025 [ADC dan Serial]");
     // Setting Mode Pin Arduino Uno
     pinMode(A0, INPUT);      // Gunakan Pin A0 sebagai Input untuk keluaran Signal Conditioning
}

void loop() {
     // Bagian utama program yang akan dieksekusi secara terus menerus
     if(millis() - ul_ADCTimeMS >= SELANG_WAKTU_ADC)
     {    
          /* KONVERSI A/D */ 
          // Bagian ini hanya akan dieksekusi setiap SELANG_WAKTU_ADC
          ul_ADCTimeMS = millis();      // Update Cek Point

          // Baca keluaran signal conditioning melalui pin A0
          uc_arrayIndex ++;             // Update indeks 
          if (uc_arrayIndex >= 10) {
               uc_arrayIndex = 0;       // Pastikan indeks < 10 
          }
          ui_ADC[uc_arrayIndex] = analogRead(A0);      
     }
     if(millis() - ul_SerialTimeMS >= SELANG_WAKTU_SERIAL)
     {
          /* Komunikasi Serial ke PC untuk Menampilkan Hasil Bacaan Moving Average */
          // Bagian ini hanya akan dieksekusi setiap SELANG_WAKTU_SERIAL sekali
          ul_SerialTimeMS = millis();   // Update Cek Point

          // Hitung Moving Averagenya dengan Merata-ratakan nilai ADC yang tersimpan di dalam array
          unsigned long ul_SUM = 0;
          for(int index=0; index<10; index++)
          {
               ul_SUM = ul_SUM + ui_ADC[index];
          }
          f_ADCMovAvg = (float)ul_SUM / 10.0;
          // Print Hasil Bacaan ADC
          /* Contoh yang Dihasilkan di Serial Monitor:
             Elapsed Time (ms) : 2000    Last ADC Reading : 857  Moving Average : 679.5
          */
          Serial.print("Elapsed Time (ms) : ");
          Serial.print(ul_SerialTimeMS);
          Serial.write('\t');
          Serial.print("Last ADC Reading : ");
          Serial.print(ui_ADC[uc_arrayIndex]);
          Serial.write('\t');
          Serial.print("Moving Average : ");
          Serial.println(f_ADCMovAvg);
     }
}
