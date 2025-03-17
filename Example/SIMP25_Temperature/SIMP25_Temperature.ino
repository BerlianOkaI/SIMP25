/*
Sistem Instrumentasi dan Metode Pengukuran - 2025

Program untuk melakukan perhitungan temperatur yang dirasakan
sensor berdasarkan nilai ADC yang diperoleh dari keluaran signal
conditioning. Konversi nilai ADC tersebut terhadap temperatur
yang dirasakan sensor diperoleh berdasarkan data kalibrasi
temperatur vs bacaan adc. Model yang digunakan untuk memetakan
bacaan ADC ke temperatur adalah model kuadratik y(x) = a*x^2 + b*x + c.

Program ini menggunakan program sebelumnya [ADC dan Serial] di mana
pada program ini, hasil bacaan ADC (baik non-filtered maupun filtered
melalui moving average) digunakan untuk menghitung temperatur.
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

float f_Temp_C;                    // Temperatur dalam derajat Celcius.
float f_TempFilter_C;              // Filtered Temperatur dalam derajat Celcius.

/* Fungsi */
float fGetTemperature(float adc_value)
{
     // Fungsi untuk menghitung temperatur berdasarkan nilai adc
     // Koefisien model a, b, dan c diisi/diganti berdasarkan hasil regresi Anda masing-masing
     float a, b, c, result;
     a = -0.002;         // This is just an example
     b = 1.24;           // This is just an example
     c = 25.00;          // This is just an example
     // Hitung resultnya
     result = a * adc_value * adc_value;     // Suku kuadrat
     result = result + (b * adc_value);      // Suku pangkat 1
     result = result + c;                    // Suku pangkat 0
     
     return result;
}


void setup() {
     // Inisiasi variabel
     f_ADCMovAvg = 0;
     uc_arrayIndex = 0;
     ul_ADCTimeMS = 0;
     ul_SerialTimeMS = 0;
     f_Temp_C = 0;
     f_TempFilter_C = 0;

     // Inisiasi Komunikasi Serial Arduino-PC
     Serial.begin(9600);                               // Komunikasi Serial dengan Baud Rate 9600
     Serial.println("SIMP - 2025 [Bacaan Temperatur]");
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
          /* Komunikasi Serial ke PC untuk Menampilkan Hasil Perhitungan Temperatur */
          // Bagian ini hanya akan dieksekusi setiap SELANG_WAKTU_SERIAL sekali
          ul_SerialTimeMS = millis();   // Update Cek Point

          // Hitung Moving Averagenya dengan Merata-ratakan nilai ADC yang tersimpan di dalam array
          unsigned long ul_SUM = 0;
          for(int index=0; index<10; index++)
          {
               ul_SUM = ul_SUM + ui_ADC[index];
          }
          f_ADCMovAvg = (float)ul_SUM / 10.0;
          // Hitung nilai temperaturnya
          f_Temp_C = fGetTemperature((float)ui_ADC[uc_arrayIndex]);        // Temperatur non-filtered berdasarkan bacaan adc terakhir
          f_TempFilter_C = fGetTemperature(f_ADCMovAvg);                   // Temperatur filtered berdasarkan moving average adc

          // Print Hasil Bacaan ADC
          /* Contoh yang Dihasilkan di Serial Monitor:
             Elapsed Time (ms) : 2000    T (Celcius) : 73.2  T filtered (Celcius) : 75.1
          */
          Serial.print("Elapsed Time (ms) : ");
          Serial.print(ul_SerialTimeMS);
          Serial.write('\t');
          Serial.print("T (Celcius) : ");
          Serial.print(f_Temp_C);
          Serial.write('\t');
          Serial.print("T filtered (Celcius) : ");
          Serial.println(f_TempFilter_C);
     }
}
