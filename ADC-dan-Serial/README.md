# SIMP25_ADC-dan-Serial

Repository ini menyediakan langkah-langkah untuk membuat Program ADC dan Serial Arduino yang dibutuhkan pada Praktek Mata Kuliah Sistem Instrumentasi dan Metode Pengukuran 2025. Karena program ini merupakan program pertama yang akan dibuat oleh peserta mata kuliah tersebut, maka pada bagian awal ini akan dijelaskan bagaimana cara membuat program dengan Arduino IDE. Contoh program Arduino ADC dan Serial dapat dlihat pada directory `Example/SIMP25_ADCdanSerial`.\
<br/>

## Instalasi Arduino IDE

Sebelum memulai pembuatan Program Arduino, silakan unduh terlebih dahulu perangkat lunak Arduino IDE yang dapat diakses di tautan (https://www.arduino.cc/en/software). Di bagian Download Options, pilih sesuai dengan OS dari PC kita masing-masing.
<br/>

## Pembuatan Program dengan Arduino IDE

Setelah berhasil melakukan instalasi Arduino IDE, silakan buka Arduino IDE tersebut dari PC kita masing-masing. Silakan perhatikan langkah-langkah berikut dengan seksama.
1. **Tampilan Arduino IDE**
   <br/> Berikut ini adalah tampilan dari Arduino IDE versi 2.3.4. *Di sini, digunakan latar berwarna hitam untuk Arduino IDE*.\
   <img src="https://github.com/BerlianOkaI/SIMP25_ADC-dan-Serial/blob/main/Gambar/Tampilan%20Arduino%20IDE%20(v2.3.4).png" alt="drawing" />\
   <br/>
2. **Buat Sketch Baru**
   <br/> Sketch merupakan tempat di mana kita akan menuliskan code kita untuk memprogram Arduino. Jika kita ingin membuat sketch baru, silakan pergi ke `File`/`New Sketch` untuk membuat sketch baru.\
   <img src="https://github.com/BerlianOkaI/SIMP25_ADC-dan-Serial/blob/main/Gambar/Sketch%20Baru.png" alt="drawing" width="500"/>\
   <br/>
3. **Pilih Board Arduino**
   <br/> Sebelum kita memprogram Arduino kita, kita harus pilih jenis board pada Arduino IDE yang sesuai dengan board yang akan digunakan pada praktek kali ini, yaitu Arduino Uno. Untuk memilih board, silakan pergi ke `Tools`/`board`/`Arduino AVR Boards` dan pilih `Arduino Uno`.\
   <img src="https://github.com/BerlianOkaI/SIMP25_ADC-dan-Serial/blob/main/Gambar/Pilih%20Board%20Arduino.png" alt="drawing" width="800" />\
   <br/>
4. **Tulis Program di Sketch**
   <br/> Silakan ketik program kita di sketch tersebut untuk ADC dan Serial. Contoh program yang akan kita tulis dapat dilihat di directory `Example/SIMP25_ADCdanSerial` pada repository ini. Silakan pilih `SIMP25_ADCdanSerial.ino` untuk melihat [contoh program](https://github.com/BerlianOkaI/SIMP25/tree/main/ADC-dan-Serial/Example) tersebut.\
   <img src="https://github.com/BerlianOkaI/SIMP25_ADC-dan-Serial/blob/main/Gambar/Tulis%20Code.png" alt="drawing" />\
   <br/>
5. **Compile Program Arduino**
   <br/> Compile program yang telah kita tulis dengan memilih `verify` pada Arduino IDE seperti pada gambar berikut.\
   <img src="https://github.com/BerlianOkaI/SIMP25_ADC-dan-Serial/blob/main/Gambar/Compile%20Code.png" alt="drawing" width="500" />\
   <br/> Bila program kita tidak memiliki error dan berhasil di-compile, maka tampilannya akan seperti ini.
   <img src="https://github.com/BerlianOkaI/SIMP25_ADC-dan-Serial/blob/main/Gambar/Success%20to%20Compile.png" alt="drawing" />\
   <br/>
6. **Upload Program ke Arduino Uno**
   <br/> Untuk memprogram Arduino Uno, kita harus upload program tersebut ke Arduino Uno. Hubungkan Arduino Uno dengan PC kita
   melalui kabel USB Type-B to Type-A yang telah disediakan. Pilih serial port yang terhubung dengan Arduino Uno (Misal, COM2) di
   `Tools`/`Port` seperti pada gambar berikut.\
   <img src="https://github.com/BerlianOkaI/SIMP25_ADC-dan-Serial/blob/main/Gambar/Pilih%20Serial%20Port.png" alt="drawing" width=500/>\
   <br/> Setelah Serial Port berhasil dipilih, langkah selanjutnya adalah upload program dengan menekan tombol `Upload` di Arduino IDE
   seperti pada gambar berikut.\
   <img src="https://github.com/BerlianOkaI/SIMP25_ADC-dan-Serial/blob/main/Gambar/Upload%20Code.png" alt="drawing" width="500"/>\
   <br/>

## Melihat Hasil Bacaan ADC Melalui Serial Monitor
<br/> Untuk melihat hasil bacaan ADC yang dikirimkan oleh Arduino Uno kita ke PC kita melalui komunikasi serial, kita harus buka Serial Monitor terlebih
dahulu. Pastikan `Serial Port` telah dipilih dengan benar di `Tools`/`Port`. Untuk membuka Serial Monitor, tekan tombol berikut. Alternatif lain adalah 
dengan pergi ke `Tools`/`Serial Monitor`. Setelah itu, Serial Monitor akan muncul di bagian bawah Arduino IDE.\
<img src="https://github.com/BerlianOkaI/SIMP25_ADC-dan-Serial/blob/main/Gambar/Open%20Serial%20Monitor%20(Marked).png" alt="drawing"/>\
<br/> Selanjutnya, pilih nilai baud rate yang sesuai dengan baud rate yang kita setting untuk Arduino Uno di program kita sebelumnya,
(dalam hal ini, kita telah set baud rate-nya sebesar 9600). Untuk mengenal apa itu baud rate (laju pengiriman bit), silakan [baca di sini](https://docs.arduino.cc/language-reference/en/functions/communication/serial/begin/).\
<img src="https://github.com/BerlianOkaI/SIMP25_ADC-dan-Serial/blob/main/Gambar/Pilih%20Baud%20Rate.png" alt="drawing"/>\
<br/> Setelah Arduino Uno dan PC kita terhubung via kabel USB, Serial Port dipilih, dan Baud Rate diatur, maka Serial Monitor (di bagian bawah Arduino IDE) akan menampilkan komunikasi Serial Arduino Uno berupa hasil bacaan ADC kita.
   
   
