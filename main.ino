#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_SSD1306.h>

RTC_DS3231 rtc;
Adafruit_SSD1306 display(128, 32, &Wire, -1);

const int RELAY_PIN_1 = 2;
const int RELAY_PIN_2 = 3;

void setup() {
  Wire.begin();
  rtc.begin();
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
}

void loop() {
  DateTime now = rtc.now();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Current time:");
  display.setTextSize(2);
  display.setCursor(0, 10);
  display.print(now.hour());
  display.print(':');
  if (now.minute() < 10) {
    display.print('0');
  }
  display.print(now.minute());
  display.display();
}

void startRelays() {
  digitalWrite(RELAY_PIN_1, HIGH);
  digitalWrite(RELAY_PIN_2, HIGH);
}

void stopRelays() {
  digitalWrite(RELAY_PIN_1, LOW);
  digitalWrite(RELAY_PIN_2, LOW);
}

void activateRelays() {
  startRelays();
  delay(10000);
  stopRelays();
  delay(10000);
}

void setRelayActivationTime() {
  int hour = 0;
  int minute = 0;
  int duration = 0;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Set relay activation time:");
  display.println();
  display.println("Enter hour (0-23):");
  display.display();
  while (true) {
    if (Serial.available() > 0) {
      hour = Serial.parseInt();
      if (hour >= 0 && hour <= 23) {
        break;
      }
    }
  }
  display.print(hour);
  display.print(':');
  display.display();
  
  display.println("Enter minute (0-59):");
  display.display();
  while (true) {
    if (Serial.available() > 0) {
      minute = Serial.parseInt();
      if (minute >= 0 && minute <= 59) {
        break;
      }
    }
  }
  display.print(minute);
  display.display();
  
  display.println("Enter duration (seconds):");
  display.display();
  while (true) {
    if (Serial.available() > 0) {
      duration = Serial.parseInt();
      if (duration > 0) {
        break;
      }
    }
  }
  display.clearDisplay();
  display.display();
  
  // TODO: save activation time and duration to EEPROM
}

void loop() {
  DateTime now = rtc.now();
  int current_hour = now.hour();
  int current_minute = now.minute();

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Time: ");
  if (current_hour < 10) {
    display.print('0');
  }
  display.print(current_hour);
  display.print(':');
  if (current_minute < 10) {
    display.print('0');
  }
  display.println(current_minute);
  display.println();
  
  display.print("Relay 1: ");
  if (relay1_state == HIGH) {
    display.println("ON");
  } else {
    display.println("OFF");
  }
  
  display.print("Relay 2: ");
  if (relay2_state == HIGH) {
    display.println("ON");
  } else {
    display.println("OFF");
  }
  
  display.display();
  
  if (current_hour == relay_activation_hour && current_minute == relay_activation_minute) {
    digitalWrite(relay1_pin, HIGH);
    relay1_state = HIGH;
    delay(relay1_duration * 1000);
    digitalWrite(relay1_pin, LOW);
    relay1_state = LOW;
    delay(relay2_delay * 1000);
    digitalWrite(relay2_pin, HIGH);
    relay2_state = HIGH;
    delay(relay2_duration * 1000);
    digitalWrite(relay2_pin, LOW);
    relay2_state = LOW;
  }
}

void menu() {
  bool menu_running = true;
  int menu_selection = 0;
  int menu_selection_min = 0;
  int menu_selection_max = 2;
  int hour_selection = relay_activation_hour;
  int minute_selection = relay_activation_minute;
  int duration_selection = relay1_duration;

  while (menu_running) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("MENU");
    display.println();
    if (menu_selection == 0) {
      display.print("> Set activation time: ");
      if (hour_selection < 10) {
        display.print('0');
      }
      display.print(hour_selection);
      display.print(':');
      if (minute_selection < 10) {
        display.print('0');
      }
      display.println(minute_selection);
    } else {
      display.println("Set activation time");
    }
    if (menu_selection == 1) {
      display.print("> Set duration (s): ");
      display.println(duration_selection);
    } else {
      display.println("Set duration (s)");
    }
    if (menu_selection == 2) {
      display.println("> Save and exit");
    } else {
      display.println("Save and exit");
    }
    display.display();

    if (digitalRead(menu_button_pin) == LOW) {
      delay(250);
      while (digitalRead(menu_button_pin) == LOW) {
        // wait for button release
      }
      if (menu_selection == 0) {
        relay_activation_hour = hour_selection;
        relay_activation_minute = minute_selection;
      } else if (menu_selection == 1) {
        relay1_duration = duration_selection;
      } else if (menu_selection == 2) {
        menu_running = false;
      }
    } else if (digitalRead(up_button_pin) == LOW) {
      delay(250);
      while (digitalRead(up_button_pin) == LOW) {
        // wait for button release
      }
      if (menu_selection == 0) {
        hour_selection = (hour_selection + 1) % 24;
      } else if (menu_selection == 1) {
        duration_selection += 1;
      }
    } else if (digitalRead(down_button_pin) == LOW) {
      delay(250);
      while (digitalRead(down_button_pin) == LOW) {
        // wait for button release
      }
      if (menu_selection == 0) {
        minute_selection = (minute_selection + 1) % 60;
      } else if (menu_selection == 1) {
        duration_selection -= 1;
        if (duration_selection < 1) {
          duration_selection = 1;
        }
      }
    } else if (digitalRead(select_button_pin) == LOW) {
      delay(250);
      while (digitalRead(select_button_pin) == LOW) {
        // wait for button release
      }
      menu_selection = (menu_selection + 1) % (menu_selection_max + 1);
      if (menu_selection < menu_selection_min) {
        menu_selection = menu_selection_min;
      }
    }
  }
}

void setup() {
  // Initializálás
  Serial.begin(9600); // Soros kommunikáció beállítása
  pinMode(relay1Pin, OUTPUT); // Relé 1-es lábának beállítása kimenetnek
  pinMode(relay2Pin, OUTPUT); // Relé 2-es lábának beállítása kimenetnek
  rtc.begin(); // RTC modul inicializálása
  lcd.begin(128, 64); // Kijelző inicializálása
  lcd.clearDisplay(); // Kijelző törlése
  lcd.setCursor(0, 0); // Kijelző kurzorának beállítása az első sorra, első oszlopba
  lcd.print("Relay 1: "); // Szöveg kiírása a kijelzőre
  lcd.setCursor(0, 1); // Kijelző kurzorának beállítása a második sorra, első oszlopba
  lcd.print("Relay 2: "); // Szöveg kiírása a kijelzőre
}

void loop() {
  // Nézzük meg, hogy volt-e gombnyomás
  if (millis() - lastButtonCheck >= BUTTON_CHECK_INTERVAL) {
    lastButtonCheck = millis();
    checkButtons();
  }

  // Ellenőrizzük, hogy elérkezett-e a bekapcsolás ideje
  if (now.hour() == START_HOUR && now.minute() == START_MINUTE && now.second() == 0 && !relayState) {
    startRelays();
  }

  // Ellenőrizzük, hogy elérkezett-e az első relé kikapcsolásának ideje
  if (millis() - relay1StartTime >= RELAY1_DURATION && relayState == 1) {
    stopRelay(1);
  }

  // Ellenőrizzük, hogy elérkezett-e a második relé kikapcsolásának ideje
  if (millis() - relay2StartTime >= RELAY2_DURATION && relayState == 2) {
    stopRelay(2);
  }

  // Frissítsük a kijelzőt
  if (millis() - lastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL) {
    lastDisplayUpdate = millis();
    updateDisplay();
  }
}
