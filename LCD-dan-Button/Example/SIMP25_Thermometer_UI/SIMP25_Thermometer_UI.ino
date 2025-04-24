/*
Sistem Instrumentasi dan Metode Pengukuran - 2025

Program untuk melakukan perhitungan temperatur yang dirasakan
sensor berdasarkan nilai ADC yang diperoleh dari keluaran signal
conditioning. Konversi nilai ADC tersebut terhadap temperatur
yang dirasakan sensor diperoleh berdasarkan data kalibrasi
temperatur vs bacaan adc. Model yang digunakan untuk memetakan
bacaan ADC ke temperatur adalah model kuadratik y(x) = a*x^2 + b*x + c.

Hasil perhitungan temperatur tersebut kemudian akan ditampilkan ke LCD
dengan menggunakan library LiquidCrystal.h. Di program ini juga, button
dengan konfigurasi resistor pulldown akan digunakan untuk mengontrol
tampilan LCD berupa hold and continue.
*/

#include <LiquidCrystal.h>

#define SELANG_WAKTU_ADC 100       // Selang waktu konversi A/D dalam ms.
#define SELANG_WAKTU_LCD 1000      // Selang waktu penampilan hasil bacaan temperatur ke LCD.
#define SELANG_WAKTU_DEBOUNCE 50   // Selang waktu minimum agar tombol bisa kembali dipakai (dalma ms)

/* Deklarasi Objek dan Variables */
LiquidCrystal lcd(12, 10, 5, 4, 3, 2); 	// LCD: RS, E, D4, D5, D6, D7
const uint8_t buttonPin = 7;       // Pin untuk button
bool b_hold_lcd;                   // State/Keadaan pause tampilan bacaan temperatur di LCD
bool lastButtonState;              // Keadaan terakhir tombol 

unsigned int   ui_ADC[10] = {0};   // Array untuk menyimpan Bacaan ADC oleh Arduino dengan banyaknya elemen sebesar 10 buah.
float f_ADCMovAvg;                 // Hasil Moving Average dari bacaan ADC. Hasil dihitung dengan meratakan array adc.
unsigned char  uc_arrayIndex;      // Indeks dari array adc.

unsigned long  ul_ADCTimeMS;       // Checkpoint pewaktu untuk ADC dalam millisecond.
unsigned long  ul_LCDTimeMS;       // Checkpoint pewaktu untuk Pengubahan Bacaan di LCD.
unsigned long  ul_DebounceMS;      // Checkpoint pewaktu debounce 

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

void setup() 
{
     // [Inisiasi variabel]
     f_ADCMovAvg = 0;
     uc_arrayIndex = 0;

     ul_ADCTimeMS = 0;
     ul_LCDTimeMS = 0;
     ul_DebounceMS = 0;

     f_Temp_C = 0;
     f_TempFilter_C = 0;

     b_hold_lcd = false;
     lastButtonState = LOW;

     // [Inisiasi Komunikasi Serial Arduino-PC (Logging purpose)]
     // *Pastikan Baud Rate di Serial Monitor 19200 !
     Serial.begin(19200);                               
     Serial.println("SIMP - 2025 [Bacaan Temperatur]");

     // [Inisiasi LCD]
     lcd.begin(16,2);
     lcd.clear();
     lcd.print("Suhu : ");
     lcd.setCursor(0, 1);
     lcd.print("Time : 0");

     // [Setting Mode Pin Arduino Uno]
     pinMode(A0, INPUT);      // Gunakan Pin A0 sebagai Input untuk keluaran Signal Conditioning
}

void loop() {
     // [Sampling ADC]
     if (millis() - ul_ADCTimeMS >= SELANG_WAKTU_ADC)
     {
          // Simpan checkpoint waktu terbaru
          ul_ADCTimeMS = millis();
          // Update Indeks Buffer
          uc_arrayIndex++;
          if (uc_arrayIndex >= 10) uc_arrayIndex = 0;
          // Lakukan Konversi ADC
          ui_ADC[uc_arrayIndex] = analogRead(A0);
     }
     // [Bacaan Temperatur ke LCD]
     if (millis() - ul_LCDTimeMS >= SELANG_WAKTU_LCD)
     {
          // Simpan Checkpoint waktu terbaru
          ul_LCDTimeMS = millis();

          // Hitung Moving Averagenya dengan Merata-ratakan nilai ADC yang tersimpan di dalam array
          unsigned long ul_SUM = 0;
          for(int index=0; index<10; index++)
          {
               ul_SUM = ul_SUM + ui_ADC[index];
          }
          f_ADCMovAvg = (float)ul_SUM / 10.0;

          // Hitung nilai temperaturnya
          f_Temp_C = fGetTemperature((float)ui_ADC[uc_arrayIndex]);        // Temperatur unfiltered
          f_TempFilter_C = fGetTemperature(f_ADCMovAvg);                   // Temperatur filtered berdasarkan moving average adc

          // [Logging Purpose]
          Serial.print("Time (s) : ");
          Serial.print(ul_LCDTimeMS / 1000UL);
          Serial.write('\t');
          Serial.print("Suhu (C): ");
          Serial.println(f_TempFilter_C);

          // Update Bacaan Temperatur di LCD Jika b_hold_lcd bernilai false
          if(!b_hold_lcd){
               lcd.setCursor(7, 0);
               lcd.print(f_TempFilter_C, 5);
               lcd.print("   ");
               lcd.print(" C");
               lcd.setCursor(7, 1);
               lcd.print(ul_LCDTimeMS / 1000UL);
               lcd.print("   ");
          }
     }
     bool currentState = digitalRead(buttonPin);
     if ((currentState == HIGH) && (lastButtonState == LOW) && (millis() - ul_DebounceMS >= SELANG_WAKTU_DEBOUNCE))
     {
          // Terjadi Rising Edge (transisi tegangan LOW ke High)
          // yang mengindikasikan adanya penekanan tombol
          // > Hold/Unhold LCD (invert keadaan sebelumnya)
          b_hold_lcd = !b_hold_lcd;
          // > Update Checkpoint
          ul_DebounceMS = millis();
          // > Update LCD berdasarkan hold state
          if (b_hold_lcd){
               // [Keadaan Pause]
               // Tampilkan Huruf "H" pada LCD
               lcd.setCursor(15, 1);
               lcd.write('H');
          } else {
               // [Keadaan Unpause]
               lcd.clear();
               lcd.print("Suhu : ");
               lcd.print(f_TempFilter_C, 5);
               lcd.setCursor(0, 1);
               lcd.print("Time : ");
          }
     }
     lastButtonState = currentState;
}
