# LCD dan Button

Repositori menyediakan contoh program yang dapat menunjukkan hasil bacaan temperatur oleh Arduino Uno ke **LCD** (_Liquid Crystal_). Selain itu, program ini juga memanfaatkan **tombol/button yang dipasang dengan resistor pulldown** untuk mengontrol tampilan LCD seperti mem-_pause_ tampilan dari LCD dan lain-lain. Program yang dibuat pada repositori ini akan menggunakan library `<LiquidCrystal.h>` sehingga kita perlu melakukan instalasi library tersebut terlebih dahulu.

<br/> Pada repository ini, ada dua contoh program:
1. `SIMP25_LCD_Counter`
   <br/> Contoh program untuk demonstrasi penggunaan LCD dan Button.
3. `SIMP25_Thermometer_UI`
   <br/> Contoh program *User Interface* (UI) yang menampilkan hasil bacaan temperatur ke dalam LCD di mana bacaan dari LCD   tersebut dapat di-*hold* dengan menggunakan tombol/button.

## Panduan Instalasi Library
1. Buka Arduino IDE
   <br/> <img src="https://github.com/BerlianOkaI/SIMP25/blob/main/Gambar/Tampilan%20Arduino%20IDE%20(v2.3.4).png" alt="drawing" width="800" />
2. Di toolbar, pergi ke `Tools`/`Manage Libraries...` *(atau bisa menggunakan `Ctrl`+`Shift`+`I`)* untuk melakukan manajemen perpustakaan (*library*) Arduino IDE.
   <br/> <img src="https://github.com/BerlianOkaI/SIMP25/blob/main/Gambar/Open%20Manage%20Libraries.png" alt="drawing" width="800" />
3. Setelah itu, untuk Arduino IDE versi 2.3.4 dan equivalennya, akan muncul tampilan dari Library Manager di bagian samping kiri.
   <br/> <img src="https://github.com/BerlianOkaI/SIMP25/blob/main/Gambar/Manage%20Libraries.png" />
4. Di Library Manager tersebut, silakan cari library `LiquidCrystal` dengan mengetikan nama library tersebut di kolom pencarian. Pilih Library `LiquidCrystal` *by Arduino, Adafruit* dan silakan install library tersebut.
   <br/> <img src="https://github.com/BerlianOkaI/SIMP25/blob/main/Gambar/Unduh%20LiquidCrystal.png" alt="drawing" width="800" />
5. Jika library tersebut berhasil di-*install*, maka di sketch yang akan kita buat kita dapat melakukan include file library tersebut dengan menuliskan `#include <LiquidCrystal.h>`. Pastikan Anda menaruh `#include <LiquidCrystal.h>` di bagian atas sebelum Anda menuliskan code di Sketch Arduino.

## Program Arduino Uno
Silakan merujuk ke bagian [Example]()
