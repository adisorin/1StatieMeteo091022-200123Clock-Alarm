## Statie Meteo Clock-Alarm
Arduino mega 2560 with TFT, alarm clock, humidity control, heat, automatic brightness adjustment of the hardware TFT (with photoresistor and a transistor that powers the display LEDs). The alarm has a direct output on the Arduino MEGA pin. An Arduino module with two relays can be added to the output pins for temperature and humidity control.
<img width="1756" height="932" alt="image" src="https://github.com/user-attachments/assets/e9c1fe5e-2ccb-431d-b9ef-ccfe688aa4bf" />


Prezentare Generală a Proiectului:
# Stație Meteo
Programul integrează diverși senzori și periferice pentru a măsura, afișa și reacționa la condițiile de mediu, cum ar fi temperatura, umiditatea și luminozitatea. Interfața principală este un ecran tactil color.
# 1. Librării și Dependențe
Codul utilizează o suită robustă de librării standard Arduino și dedicate:
Interfațare Hardware: Wire.h, SPI.h.
Afișaj TFT și Tactil: MCUFRIEND_kbv.h (o librărie populară pentru ecrane TFT Arduino shield).
Senzori: DHT.h (pentru senzori DHT11/DHT22), OneWire.h, DallasTemperature.h (pentru senzorul DS18B20).
Calcul și Timp Real: math.h, TimeLib.h, DS1307RTC.h (pentru modulul de ceas real-time RTC).
Manipulare Date: stdio.h, string.h.
# 2. Componente Hardware Cheie
Sunt definite numeroase constante pentru alocarea pinilor și parametrii senzorilor:
Senzori de Mediu:
DHTPIN (A7): Senzor de Umiditate și Temperatură (DHT11).
ONE_WIRE_BUS (A8): Senzor de Temperatură Impermeabil (DS18B20).
LDRpin (A9): Senzor de lumină (LDR - Light Dependent Resistor).
Actuatori și Ieșiri:
buzzer (Pin 22): Pentru alarme sonore și notificări.
RELAY_RH (Pin 24): Releu pentru controlul umidității (ex: dezumidificator/umidificator).
RELAY_TH (Pin 25): Releu pentru controlul temperaturii (ex: încălzitor/răcitor).
# Intrări Utilizator:
buttonPin (Pin 23): Buton pentru oprirea Releu pin 24 daca RH < 81 (oprire forțată dezumidificator).

buttonPinAL (Pin 31): Buton dedicat pentru alarmă.
Afișaj: Ecran tactil color TFT (MCUFRIEND_kbv), cu ID-uri posibile 0x9486/87 sau 0x7523.
# 3. Funcționalități Principale
Codul demonstrează o serie de funcții avansate:
# A. Ecran de Încărcare (Splash Screen)
Funcția loadingScreen() este apelată la pornire (setup). Aceasta afișează un mesaj "Loading...", o bară de progres animată și generează o secvență complexă de bipuri sonore (tone()) pentru a indica inițializarea sistemului.
# B. Interfață Grafică Avansată
După încărcare, ecranul este configurat cu o interfață vizuală detaliată:
Sunt trasate linii (tft.fillRect) pentru a crea chenare, divizoare și zone de afișaj.
Zonele de text sunt etichetate: "Temperature", "Humidity", "WEATHER STATION AdiSorin".
Sunt definite culori personalizate (#define) pentru o estetică specifică (TURQUOISE, CHARCOAL, CYAN etc.).
# C. Măsurarea Datelor și Calculul Punctului de Rouă
Sunt stocate variabile (temp, rh, ah, td) pentru a reține valorile curente.
O funcție matematică dewPoint() este inclusă pentru a calcula punctul de rouă (temperatura la care aerul devine saturat cu vapori de apă) pe baza temperaturii curente și a umidității relative.
# D. Control Logic (Funcția loop - neprezentată integral)
Deși funcția principală loop() nu este completă în fragmentul furnizat, structura variabilelor sugerează o logică de control:
Variabilele RELAY_RH și RELAY_TH sunt setate ca OUTPUT în setup, indicând faptul că sistemul poate porni sau opri dispozitive externe (cum ar fi ventilatoare sau încălzitoare) pe baza citirilor senzorilor.
Variabila contor și butonul de alarmă indică o logică de alertă, posibil pentru umiditate ridicată (comentariul ////Contor buzzer rh==95 sugerează o alarmă la atingerea pragului de 95% umiditate relativă).

Created by: Sorinescu Adrian
