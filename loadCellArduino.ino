#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define button_pin 8
#define sensor_pin A0

const int SAMPLESIZE = 100;
float running_total, average_reading;
int sensor_values[SAMPLESIZE];
int array_index;

boolean hasButtonActivated;

const int REFRESH_INTERVAL = 500;
long last_updated_time;

int weight;
const float PRICE_CONVERSION_FACTOR = 0.5;
float zero_error;

void setup() {
        // set up the LCD's number of columns and rows: 
        lcd.begin(16, 2);
        // Print a message to the LCD.
        lcd.print("hello, world!");

        pinMode(button_pin, INPUT_PULLUP);
        pinMode(sensor_pin, INPUT);

        Serial.begin(9600);
}

void loop() {

        average_reading = get_averages();

        weight = do_conversion(average_reading);

        Serial.println(average_reading);

        update_display();
        
        check_for_button();
}

float get_averages() {

        sensor_values[array_index] = analogRead(sensor_pin) - zero_error;

        array_index++;
        if (array_index >= SAMPLESIZE) {
                array_index = 0;
        }

        running_total = 0; 
        average_reading = 0;     

        for (int i=0; i<SAMPLESIZE; i++) {
                running_total += sensor_values[i];
        }

        return running_total/SAMPLESIZE;
}

int do_conversion(float reading) {
        return int(map(reading, 0, 512, 0, 2500));  //cannot get more than 2.5V which is 512 analog value, mapped to 2.5kg or 2500g      
}

void update_display() {
        
        if (millis() - last_updated_time > REFRESH_INTERVAL) {             
                if (weight >=0 ) {
                        lcd.clear();
                        lcd.setCursor(0, 0);
                        lcd.print(weight);
                        lcd.print("g");
                        lcd.setCursor(0, 1);
                        lcd.print("$");
                        lcd.print(weight * PRICE_CONVERSION_FACTOR);
                }
                last_updated_time = millis();
        }
}

void check_for_button() {     

        if ( (digitalRead(button_pin) == LOW) && !hasButtonActivated) {
                //           Serial.println("pressed");       
                hasButtonActivated = true;
                zero_reading();
        } 
        else if ( (digitalRead(button_pin) == HIGH) && hasButtonActivated) {
                //           Serial.println("released");
                hasButtonActivated = false;       
        }

}

void zero_reading() {
        if (average_reading > 0) {
                 zero_error = average_reading;    
        }
}


