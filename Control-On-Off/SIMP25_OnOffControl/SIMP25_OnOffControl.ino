/*
Sistem Instrumentasi dan Metode Pengukuran - 2025

Program untuk melakukan kontrol loop tertutup dengan
kontrol berupa On/Off. Solid state relay (SSR) digunakan
untuk menghidup-matikan heater dan SSR tersebut akan 
Arduino Uno kontrol melalui program ini. 

Misalkan error menyatakan selisih antara temperatur
setpoint dan temperatur aktual pada sistem.
||
||   error = setpoint - aktual
|| 
Maka, secara umum kontrol On/Off yang akan dibuat
memiliki ketentuan berikut.
> Heater akan menyala apabila temperatur aktual
  lebih rendah dari setpoint dan heater akan mati
  apabila temperatur aktualnya lebih tinggi.
> Dengan definisi error di atas, maka:
  + error bernilai positif apabila setpoint > aktual
  + error bernilai negatif apabila setpoint < aktual
> Untuk menghindari transisi on/off pada SSR (dan Heater
  secara tidak langsung) yang terlalu cepat, maka 
  kita definisikan deadband / hysteresis. Akibatnya,
  algoritma kontrol On/Off Heater berubah menjadi:
  
  ->  IF [error > deadband / 2]    THEN [heater dinyalakan]
  ->  IF [error < -deadband / 2]   THEN [heater dimatikan]  

  *Notes 01: untuk -deadband/2 < error < deadband/2, keadaan heater
  tidak akan diubah oleh Arduino Uno. Artinya, jika keadaan heater
  menyala tepat sebelum nilai error masuk ke daerah deadband 
  [-deadband/2, deadband/2], maka ketika heater akan tetap menyala
  ketika error berada di daerah deadband hingga error tersebut
  keluar dari daerah deadband. Hal sebaliknya juga berlaku jika
  keadaan heater mati tepat sebelum error memasuki daerah deadband. 

  *Notes 02: pada dasarnya, kontrol loop tertutup bertujuan untuk
  mencapai nilai |error| sekecil mungkin dengan target error = 0.
  Ini ekuivalen dengan mengatakan bahwa kontrol loop tertutup bertujuan
  untuk membuat temperatur aktual bernilai sama dengan setpoint.
*/

#include <LiquidCrystal.h>

// Definisi (silakan atur sesuai keperluan)
#define SELANG_WAKTU_SAMPLING      100       // Selang waktu konversi A/D dalam ms dan kontrol Heater.
#define SELANG_WAKTU_LCD           1000      // Selang waktu penampilan hasil bacaan temperatur ke LCD.
#define SELANG_WAKTU_DEBOUNCE      50        // Selang waktu minimum agar tombol bisa kembali dipakai (dalma ms)

#define DEADBAND_IN_CELCIUS        5.0F      // Rentang Deadband dalam celcius (suffix F menyatakan float)
#define RESOLUSI_SETPOINT_C        1.0F      // Resolusi nilai perubahan setpoint dalam celcius (suffix F menyatakan float)

#define BUTTON_INC                 6         // Tombol untuk menaikkan nilai setpoint
#define BUTTON_DEC                 7         // Tombol untuk menurunkan nilai setpoint
#define HEATER_PIN                 9         // Pin untuk mengontrol SSR

/* Deklarasi Objek dan Variables */
LiquidCrystal lcd(12, 10, 5, 4, 3, 2); 	// LCD: RS, E, D4, D5, D6, D7

bool lastButtonState_inc;          // Keadaan terakhir tombol (untuk menaikkan nilai setpoint)
bool lastButtonState_dec;          // Keadaan terakhir tombol (untuk menurunkan nilai setpoint)
bool currentState;

unsigned int   ui_ADC[10] = {0};   // Array untuk menyimpan Bacaan ADC oleh Arduino dengan banyaknya elemen sebesar 10 buah.
float          f_ADCMovAvg;        // Hasil Moving Average dari bacaan ADC. Hasil dihitung dengan meratakan array adc.
unsigned char  uc_arrayIndex;      // Indeks dari array adc.

unsigned long  ul_SampTimeMS;      // Checkpoint pewaktu untuk sampling dalam millisecond.
unsigned long  ul_LCDTimeMS;       // Checkpoint pewaktu untuk Pengubahan Bacaan di LCD.
unsigned long  ul_DebounceMS_inc;  // Checkpoint pewaktu debounce
unsigned long  ul_DebounceMS_dec;  // Checkpoint pewaktu debounce 

float f_TempFilter_C;              // Filtered Temperatur dalam derajat Celcius.
float f_SetPoint_C;                // Setpoint temperatur dalam derajat Celcius.
float f_Error_C;                   // Nilai error antara setpoint dan f_TempFilter_C (aktual) dalam derajat Celcius
const float f_HalfDeadband_C = DEADBAND_IN_CELCIUS / 2;     // Setengah dari nilai deadband

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

inline void NyalakanHeater(void){
     // level tegangan rendah pada heater pin akan membuat SSR menjadi saklar tertutup
     // bagi heater sehingga heater menyala
     digitalWrite(HEATER_PIN, LOW);
}
inline void MatikanHeater(void){
     // level tegangan tinggi pada heater pin akan membuat SSR menjadi saklar terbuka
     // bagi heater sehingga heater mati
     digitalWrite(HEATER_PIN, HIGH);
}

void setup() 
{
     // [Inisiasi variabel]
     f_ADCMovAvg         = 0;
     uc_arrayIndex       = 0;

     ul_SampTimeMS       = 0;
     ul_LCDTimeMS        = 0;
     ul_DebounceMS_inc   = 0;
     ul_DebounceMS_dec   = 0;

     f_TempFilter_C = 0;
     f_SetPoint_C   = 70.0;
     f_Error_C      = f_SetPoint_C - f_TempFilter_C;

     // [Inisiasi Komunikasi Serial Arduino-PC (Logging purpose)]
     // *Pastikan Baud Rate di Serial Monitor 19200 !
     Serial.begin(19200);                               
     Serial.println("SIMP - 2025 [Kontrol Temperatur]");

     // [Inisiasi LCD]
     lcd.begin(16,2);
     lcd.clear();
     lcd.print("Suhu : ");
     lcd.setCursor(0, 1);
     lcd.print("SP   : ");

     // [Setting Mode Pin Arduino Uno]
     pinMode(A0, INPUT);      // Gunakan Pin A0 sebagai Input untuk keluaran Signal Conditioning
     pinMode(BUTTON_INC, INPUT);
     pinMode(BUTTON_DEC, INPUT);
     pinMode(HEATER_PIN, OUTPUT);
}

void loop() {
     // [Sampling ADC]
     if (millis() - ul_SampTimeMS >= SELANG_WAKTU_SAMPLING)
     {
          // * Konversi ADC Tunggal *
          // Simpan checkpoint waktu terbaru
          ul_SampTimeMS = millis();
          // Update Indeks Buffer
          uc_arrayIndex++;
          if (uc_arrayIndex >= 10) uc_arrayIndex = 0;
          // Lakukan Konversi ADC
          ui_ADC[uc_arrayIndex] = analogRead(A0);

          // * Averaging dan penghitungan temperatur *
          // Hitung Moving Averagenya dengan Merata-ratakan nilai ADC yang tersimpan di dalam array
          unsigned long ul_SUM = 0;
          for(int index=0; index<10; index++)
          {
               ul_SUM = ul_SUM + ui_ADC[index];
          }
          f_ADCMovAvg = (float)ul_SUM / 10.0;
          // Hitung nilai temperaturnya
          f_TempFilter_C = fGetTemperature(f_ADCMovAvg);                   // Temperatur filtered berdasarkan moving average adc

          // * Kontrol Heater berdasarkan nilai temperatur aktual dan setpoint *
          f_Error_C = f_SetPoint_C - f_TempFilter_C;
          if (f_Error_C - f_HalfDeadband_C > 0) 
          {    // Equivalent with (error > deadband/2) or (actual < setpoint - deadband/2)
               NyalakanHeater();
          } 
          else if (f_Error_C + f_HalfDeadband_C < 0)
          {    // Equivalent with (f_Error_C < -f_HalfDeadband) or (actual > setpoint + deadband/2)
               MatikanHeater();
          }
     }
     // [Bacaan Temperatur ke LCD]
     if (millis() - ul_LCDTimeMS >= SELANG_WAKTU_LCD)
     {
          // Simpan Checkpoint waktu terbaru
          ul_LCDTimeMS = millis();
          // Display bacaan temperatur dan setpoint
          {
               lcd.setCursor(7, 0);
               lcd.print("     ");
               lcd.setCursor(7, 0);
               lcd.print(f_TempFilter_C, 5);
               lcd.print(" C");
               lcd.setCursor(7, 1);
               lcd.print("     ");
               lcd.setCursor(7, 1);
               lcd.print(f_SetPoint_C, 5);
          }
     }
     // [Handler Tombol Setpoint Increase]
     currentState = digitalRead(BUTTON_INC);
     if (
          (currentState == HIGH) && (lastButtonState_inc == LOW)           // Rising edge transition?
          && (millis() - ul_DebounceMS_inc >= SELANG_WAKTU_DEBOUNCE)       // and the debounce?
     )
     {
          // *Terjadi Rising Edge (transisi tegangan LOW ke High)
          // yang mengindikasikan adanya penekanan tombol*
          // > Update Checkpoint
          ul_DebounceMS_inc = millis();
          // > Naikkan nilai setpoint
          f_SetPoint_C = f_SetPoint_C + RESOLUSI_SETPOINT_C;
          // > Update nilai setpoint yang ditampilkan di LCD
          lcd.setCursor(7, 1);
          lcd.print("     ");
          lcd.setCursor(7, 1);
          lcd.print(f_SetPoint_C, 5);
     }
     lastButtonState_inc = currentState;

     // [Handler Tombol Setpoint Decrease]
     currentState = digitalRead(BUTTON_DEC);
     if (
          (currentState == HIGH) && (lastButtonState_dec == LOW)           // Rising edge transition?
          && (millis() - ul_DebounceMS_dec >= SELANG_WAKTU_DEBOUNCE)       // and the debounce?
     )
     {
          // *Terjadi Rising Edge (transisi tegangan LOW ke High)
          // yang mengindikasikan adanya penekanan tombol*
          // > Update Checkpoint
          ul_DebounceMS_dec = millis();
          // > Turunkan nilai setpoint
          f_SetPoint_C = f_SetPoint_C - RESOLUSI_SETPOINT_C;
          // > Update nilai setpoint yang ditampilkan di LCD
          lcd.setCursor(7, 1);
          lcd.print("     ");
          lcd.setCursor(7, 1);
          lcd.print(f_SetPoint_C, 5);
     }
     lastButtonState_dec = currentState;
}
