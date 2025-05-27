/*
Sistem Instrumentasi dan Metode Pengukuran - 2025

Program untuk melakukan kontrol loop tertutup dengan
kontrol berupa PID. Solid state relay (SSR) digunakan
untuk menghidup-matikan heater dan SSR tersebut akan 
Arduino Uno kontrol melalui program ini. 

Misalkan error menyatakan selisih antara temperatur
setpoint dan temperatur aktual pada sistem.
||
||   e(t) = SP - x(t)
|| 
||   e(t) : error 
||   SP   : Set point (dalam hal ini temperatur)
||   x(t) : Variabel yang ingin dikontrol (dalam hal ini temperatur)
||
Maka, output PID u(t) diberikan sebagai
||
||   u(t) = Kp * e(t) + Ki \int{e(t)} + Kd de(t)/dt
||
Jika kita uraikan formula PID di atas menjadi komponen-komponen
proporsional P(t), integral I(t), dan derivative D(t), maka
||
||   u(t) = P(t) + I(t) + D(t)
||   
Untuk perhitungan secara numerik, kita definisikan instance waktu 
t = nT dengan T menyatakan periode sampling yang digunakan untuk
komputasi PID. Kita bisa definisikan
||
||   P(nT) = Kp * e(nT)                 
||   I(nT) = Ki * \sum(T * e(nT))
||         = Ki * T * e(nT) + I((n-1)T)                     // Persamaan Rekursif
||   D(nT) = Kd * ( e(nT) - e((n-1)T) ) / T                 // Aproksimasi Turunan
||
Sehingga kita bisa hitung nilai u(nT) berdasarkan komponen-komponen
P(nT), I(nT), dan D(nT) di atas.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <LiquidCrystal.h>

// Definisi (silakan atur sesuai keperluan)
#define SELANG_WAKTU_SAMPLING_ADC  100       // Selang waktu konversi A/D dalam ms.
#define SELANG_WAKTU_SAMPLING_PID  2000ULL   // Selang waktu sampling kontrol PID dalam ms. (Maksimum 4 detik atau 4000 ms)
#define SELANG_WAKTU_LCD           1000      // Selang waktu penampilan hasil bacaan temperatur ke LCD.
#define SELANG_WAKTU_DEBOUNCE      50        // Selang waktu minimum agar tombol bisa kembali dipakai (dalma ms)

#define RESOLUSI_SETPOINT_C        1.0F

#define BUTTON_INC                 6         // Tombol untuk menaikkan nilai setpoint
#define BUTTON_DEC                 7         // Tombol untuk menurunkan nilai setpoint
#define HEATER_PIN                 9         // Pin untuk mengontrol SSR

#define CPU_FREQ              16000ULL            // in kHz
#define TIMER1_PRESCALLER     1024ULL
#define TIMER1_TOP_VALUE      ((SELANG_WAKTU_SAMPLING_PID * CPU_FREQ / TIMER1_PRESCALLER) - 1)

/* Deklarasi Objek dan Variables */
LiquidCrystal lcd(12, 10, 5, 4, 3, 2); 	// LCD: RS, E, D4, D5, D6, D7

bool lastButtonState_inc;          // Keadaan terakhir tombol (untuk menaikkan nilai setpoint)
bool lastButtonState_dec;          // Keadaan terakhir tombol (untuk menurunkan nilai setpoint)
bool currentState;

unsigned int   ui_ADC[10] = {0};   // Array untuk menyimpan Bacaan ADC oleh Arduino dengan banyaknya elemen sebesar 10 buah.
float          f_ADCMovAvg;        // Hasil Moving Average dari bacaan ADC. Hasil dihitung dengan meratakan array adc.
unsigned char  uc_arrayIndex;      // Indeks dari array adc.

volatile uint8_t  b_PIDSampTimeFlag;    // Flag untuk menandakan sudah saatnya melakukan perhitungan dan kontrol PID 
unsigned long  ul_SampTimeMS;      // Checkpoint pewaktu untuk sampling dalam millisecond.
unsigned long  ul_LCDTimeMS;       // Checkpoint pewaktu untuk Pengubahan Bacaan di LCD.
unsigned long  ul_DebounceMS_inc;  // Checkpoint pewaktu debounce
unsigned long  ul_DebounceMS_dec;  // Checkpoint pewaktu debounce 

float f_TempFilter_C;              // Filtered Temperatur dalam derajat Celcius.
float f_SetPoint_C;                // Setpoint temperatur dalam derajat Celcius.

const float f_PID_Kp     = 0.5F;   // Konstanta Proportional PID
const float f_PID_Ki     = 0.0F;   // Konstanta Integral PID
const float f_PID_Kd     = 0.0F;   // Konstanta Derivative PID

float f_PrevError_C;               // Nilai error antara setpoint dan f_TempFilter_C sebelumnya
float f_Error_C;                   // Nilai error antara setpoint dan f_TempFilter_C (aktual) dalam derajat Celcius
float f_PID_Prop;                  // Nilai komponen proportional dari PID
float f_PID_Int;                   // Nilai komponen integral dari PID
float f_PID_Deriv;                 // Nilai komponen derivative dari PID
float f_PID_Output;                // Output PID

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
float fGetPIDOutput(float setpoint, float temp)
{
     f_PrevError_C  = f_Error_C;
     f_Error_C      = setpoint - temp;
     // Hitung komponen PID
     f_PID_Prop     = f_PID_Kp * f_Error_C;
     f_PID_Int      = f_PID_Int + (f_PID_Ki * (float)(SELANG_WAKTU_SAMPLING_PID / 1000.0F) * f_Error_C);    // Sampling time dalam s
     f_PID_Deriv    = f_PID_Kd * (f_Error_C - f_PrevError_C) / ((float)SELANG_WAKTU_SAMPLING_PID / 1000.0F);
     // Hitung Output PID
     return f_PID_Prop + f_PID_Int + f_PID_Deriv;
}

void HeaterControl(float PIDOutput)
{
     uint16_t PWM_DutyCycle_Value = 0;
     if (PIDOutput > 0.0F)
     {
          // Di sini, PIDOutput merupakan % Duty Cycle dari PWM untuk kontrol SSR
          PWM_DutyCycle_Value = (uint16_t)((float)TIMER1_TOP_VALUE * (PIDOutput / 100.0F));
     }
     // Set nilai OCR1A untuk mengontrol Duty Cycle PWM
     cli();
     OCR1A = PWM_DutyCycle_Value;
     sei();
}

void HardwareTimerSetup(void)
{
     /* Setup untuk timer interrupt dengan menggunakan Atmega328P Timer1 */
     // Jika Anda tertarik untuk belajar lebih lanjut mengenai interrupt,
     // silakan Anda ambil matakuliah mikrokontroler dan instrumentasi digital
     // semester depan
     
     // *Notes: Pastikan anda tidak menggunakan analogWrite pada pin digital
     // 9 dan 10

     // Initialize Timer1 Counter Value
     ICR1  = TIMER1_TOP_VALUE;     // Top value
     OCR1A = TIMER1_TOP_VALUE / 2; // Output compare match value
     TCNT1 = 0;                    // Timer counter
     // COM : Disable, WGM : Fast PWM with ICR1 as Top Value
     TCCR1A = (1 << WGM11);                  // Ini memberikan 0b0000 0010
     TCCR1B = (1 << WGM13) | (1 << WGM12);   // Ini memberikan 0b0001 1000
     // Enable interrupt (Overflow Interrupt dan Output Compare Match A Interrupt)
     TIMSK1 = (1 << TOIE1) | (1 << OCIE1A);
}

static inline void HardwareTimerRoutineStart(void)
{
     // Mulai timer dengan menset bit ketiga clock select != 0b000
     TCCR1B = (TCCR1B & (~0x07)) | ((1 << CS12) | (1 << CS10)); 
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

/* Interrupt Service Routine */
ISR(TIMER1_OVF_vect)
{
     // Notifikasi loop utama untuk melakukan perhitungan PID
     b_PIDSampTimeFlag = true;
     if (OCR1A > 0) NyalakanHeater();
}
bool asd = false;
ISR(TIMER1_COMPA_vect)
{
     // Untuk mengenerate PWM noninverting pada pin manapun
     // *Digunakan untuk mengontrol SSR
     MatikanHeater();
}

/* Algoritma utama */
void setup() 
{
     // [Inisiasi variabel]
     f_ADCMovAvg         = 0;
     uc_arrayIndex       = 0;

     b_PIDSampTimeFlag   = true;
     ul_SampTimeMS       = 0;
     ul_LCDTimeMS        = 0;
     ul_DebounceMS_inc   = 0;
     ul_DebounceMS_dec   = 0;

     f_TempFilter_C = 0;
     f_SetPoint_C   = 70.0;

     f_Error_C      = f_SetPoint_C - f_TempFilter_C;
     f_PrevError_C  = f_Error_C;

     f_PID_Prop     = 0;
     f_PID_Int      = 0;
     f_PID_Deriv    = 0;
     f_PID_Output   = 0;

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

     // [Start Hardware Timer dan Interrupt Service-nya]
     HardwareTimerSetup();
     HardwareTimerRoutineStart();
     sei();
}

void loop() {
     // [Sampling ADC]
     if (millis() - ul_SampTimeMS >= SELANG_WAKTU_SAMPLING_ADC)
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
     }
     // [Kontrol PID]
     if (b_PIDSampTimeFlag)
     {    // Event untuk menghitung PID dan menerapkan output kontrolnya
          // Reset flag
          cli();
          b_PIDSampTimeFlag = false;
          sei();
          // Hitung PID
          f_PID_Output = fGetPIDOutput(f_SetPoint_C, f_TempFilter_C);
          Serial.println(f_PID_Output);
          // Kontrol PWM Relay berdasarkan output PID
          HeaterControl(f_PID_Output);
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
