# Close Loop PID Controller
Repository ini menyediakan program untuk mengontrol temperatur (menggunakan heater dengan relay tipe SSR) dengan menggunakan kontrol
PID (Proportional, Integral, Derivative). Program ini dibuat berdasarkan [program sebelumnya](https://github.com/BerlianOkaI/SIMP25/tree/main/Control-On-Off/SIMP25_OnOffControl)
di mana kontrol On/Off pada program sebelumnya diganti dengan kontrol PID.

<br> Pada program ini, satu pin output digital dari Arduino Uno akan digunakan untuk mengontrol nyala matinya SSR yang terhubung langsung
dengan Heater. Heater tersebut (melalui SSR) akan dinyala matikan secara periodik dengan periodisitas siklus sebesar (default) 2000ms; 
periode satu siklus dapat diubah hingga nilai maksimum sampai 4000ms.
Output dari perhitungan numerik PID akan digunakan untuk menentukan besarnya duty cycle (lama On time dan Off time) dari Heater.
Untuk menghasilkan Time-Proportional Ouput (TPO) tersebut, program ini menggunakan Timer1 dari ATmega328P sebagai timer utamanya
yang dijalankan dengan Waveform Generation Mode berupa Fast PWM. Untuk menghindari hal yang tidak diinginkan, jangan gunakan
fungsi `analogWrite()` dan sejenisnya pada Pin digital `9` dan `10`.

## Perhitungan PID
Kontrol PID menggunakan formula sebagai berikut.

$$
u(t) = K_{p} e(t) + K_{i} \int_{0}^{t} e(s) ds + K_{d} \frac{de(t)}{dt}
$$

Dengan memisalkan komponen proportional, integral, dan derivative sebagai:

$$
\begin{equation}
\begin{split}
  \mathcal{P}(t) & = & K_{p} e(t) \\
  \mathcal{I}(t) & = & K_{i} \int_{0}^{t} e(s) ds \\
  \mathcal{D}(t) & = & K_{d} \frac{de(t)}{dt}
\end{split}
\end{equation}
$$

Maka, output PID $u(t)$ dapat dinyatakan sebagai berikut.

$$
\begin{equation}
  u(t) = \mathcal{P}(t) + \mathcal{I}(t) + \mathcal{D}(t)
\end{equation}
$$
