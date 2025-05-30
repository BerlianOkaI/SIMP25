# Perhatian
Program ini dibuat berdasarkan [program sebelumnya](https://github.com/BerlianOkaI/SIMP25/blob/main/ADC-dan-Serial/Example/SIMP25_Temperature/SIMP25_Temperature.ino) di [ADC-dan-Serial](https://github.com/BerlianOkaI/SIMP25/tree/main/ADC-dan-Serial). Silakan buka file `SIMP25_Thermometer_UI.ino`


# Issue and Fixes
1. Serial Monitor menampilkan logging yang terlalu cepat
   <br/> **Causes**
   <br/> >> _Code bacaan temperatur ke LCD di bagian `void loop()` dieksekusi setiap `SELANG_WAKTU_ADC` sekali, bukan tiap `SELANG_WAKTU_LCD` sekali._
   <br/> **Fixes**
   <br/> >> _Ganti `if (millis() - ul_LCDTimeMS >= SELANG_WAKTU_ADC)` ke `if (millis() - ul_LCDTimeMS >= SELANG_WAKTU_LCD)` (bagian program: void loop())_ \
   <br/>  
2. LCD tidak bisa keluar dari keadaan Hold (tidak bisa melanjutkan penampilan temperatur kembali setelah tombol ditekan dua kali).
   <br/> **Causes**
   <br/> >> _Baris program `b_hold_lcd = ~b_hold_lcd` memuat operasi not yang salah_
   <br/> **Fixes**
   <br/> >> _Ganti `~` menjadi `!` sebagai operator not untuk men-toggle state dari b_hold_lcd, `b_hold_lcd = !b_hold_lcd` (bagian program: void loop())_\
   <br/>
3. Hurf 'H' sebagai indikator "Hold" di LCD Overlapped dengan "Temp" di LCD
   <br/> **Causes**
   <br/> >> _Perintah set cursor untuk LCD yang kurang tepat_
   <br/> **Fixes**
   <br/> >> _Ubah nilai argumen col pada `lcd.setCursor(col, row)` dari `lcd.setCursor(0,1)` menjadi `lcd.setCursor(15,1)` sebelum memberikan perintah `lcd.write('H')` untuk bagian code yang dieksekusi ketika b_hold_lcd bernilai true saat tombol ditekan (bagian program: void loop())_\
   <br/>
4. Bacaan Temperatur sangat tidak tepat
   <br/> **Causes**
   <br/> >> _Bacaan temperatur dihitung dengan menggunakan fungsi `float fGetTemperature(float adc_value)` di mana pada definisi fungsi tersebut, terdapat tiga variabel `a`, `b`, dan `c` untuk menghitung temperatur dengan model kuadratik (untuk model linear, silakan ubah `a = 0`)_
   <br/> **Fixes**
   <br/> >> _Silakan ubah ketiga parameter tersebut berdasarkan hasil regresi masing-masing. Untuk regresi linear, silakan isi `a = 0` atau silakan modifikasi sendiri perhitungan temperatur yang tertera di `float fGetTemperature(float adc_value)`_ 
