#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 10, 5, 4, 3, 2); 	// LCD: RS, E, D4, D5, D6, D7

// Tombol dan counter
const int buttonPin = 7;
bool lastButtonState = LOW;
int counter = 0;

void setup() {
  pinMode(buttonPin, INPUT); 		// tombol dengan pull-down
  lcd.begin(16, 2);
  lcd.clear();
}

void loop() {
  int adcValue = analogRead(A0);
  float voltage = adcValue * (5.0 / 1023.0); 	// Asumsi Vref = 5V

  lcd.setCursor(0, 0);
  lcd.print("ADC: ");
  lcd.print(voltage, 3); 		// tampilkan 3 digit desimal
  lcd.print(" V   ");  		// padding biar bersih saat update

  // ======= Tombol & Counter =======
  bool currentState = digitalRead(buttonPin);

  if (currentState == HIGH && lastButtonState == LOW) {
    counter++;
    delay(50); 		// debounce
  }

  lcd.setCursor(0, 1);
  lcd.print("Count: ");
  lcd.print(counter);
  lcd.print("     "); 		// bersihkan sisa karakter
  lastButtonState = currentState;
  delay(500); // update tiap 500ms
}
