#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


AsyncWebServer server(420); //Creates the webserver object on port 420

//Data used to setup the Display
const int lcdColumns = 16; //Num of columns on the LCD display.
const int lcdRows = 2; //Num of rows on the LCD display
const int SDAPin = 25; //GPIO pin on the ESP32 that the I2C's SDA pin is connected to 
const int SCLPin = 33; //GPIO pin on the ESP32 that the I2C's SCL pin is connected to
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); //Creates the object that represents the LCD display.

//Data used to setup the speaker
const int bzrPin = 12;
const int bzrChannel = 1;
const int alarmFreq = 0;
const int bzrResolution = 8;

//Network creds
const char* ssid = "";
const char* password = "";

const char* PARAM_INPUT_1 = "duration";

// HTML web page to handle the input of the timer duration
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Smart Timer</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/get">
    Duration: <input type="text" name="duration">
    <input type="submit" value="Submit">
  </form><br>
</body></html>)rawliteral";

//Handles any invalid requests
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

//Used to bridge between arduino tone() and esp32 ledcWriteTone() 
//Now I can easily copy paste using midi to arduino rather than typing it out line-by-line
void tone(int speakerChannel, int frequency, unsigned long delayTime) {
  ledcWriteTone(speakerChannel, frequency);
  delay(delayTime);
  ledcWriteTone(speakerChannel, 0);
}

//This handles playing the song for the alarm
//Currently it is "Saria's Song" from Legend of Zelda Ocarina of Time
void alarm(){
    tone(bzrChannel, 349, 53.95675);
    delay(53.95675);
    delay(53.95675);
    tone(bzrChannel, 440, 53.95675);
    delay(53.95675);
    delay(53.95675);
    tone(bzrChannel, 493, 215.827);
    delay(215.827);
    delay(4.49639583333);
    tone(bzrChannel, 349, 53.95675);
    delay(53.95675);
    delay(53.3947005208);
    tone(bzrChannel, 440, 53.95675);
    delay(53.95675);
    delay(52.8326510417);
    delay(214.140851563);
    tone(bzrChannel, 493, 1.6861484375);
    delay(1.6861484375);
    tone(bzrChannel, 349, 106.227351563);
    delay(106.227351563);
    delay(6.18254427083);
    tone(bzrChannel, 440, 53.95675);
    delay(53.95675);
    delay(53.3947005208);
    tone(bzrChannel, 493, 53.95675);
    delay(53.95675);
    delay(52.2706015625);
    tone(bzrChannel, 659, 53.95675);
    delay(53.95675);
    delay(53.3947005208);
    delay(214.140851563);
    tone(bzrChannel, 587, 1.6861484375);
    delay(1.6861484375);
    tone(bzrChannel, 493, 52.2706015625);
    delay(52.2706015625);
    delay(52.2706015625);
    tone(bzrChannel, 523, 53.95675);
    delay(53.95675);
    delay(53.95675);
    tone(bzrChannel, 493, 107.9135);
    delay(107.9135);
    delay(6.18254427083);
    tone(bzrChannel, 391, 53.95675);
    delay(53.95675);
    delay(53.3947005208);
    delay(533.947005208);
    tone(bzrChannel, 329, 5.62049479167);
    delay(5.62049479167);
    tone(bzrChannel, 293, 48.3362552083);
    delay(48.3362552083);
    delay(53.95675);
    tone(bzrChannel, 329, 107.9135);
    delay(107.9135);
    delay(6.18254427083);
    tone(bzrChannel, 391, 53.95675);
    delay(53.95675);
    delay(53.3947005208);
    delay(641.860505208);
    tone(bzrChannel, 329, 5.62049479167);
    delay(5.62049479167);
    tone(bzrChannel, 349, 102.293005208);
    delay(102.293005208);
    delay(6.18254427083);
    tone(bzrChannel, 440, 53.95675);
    delay(53.95675);
    delay(53.3947005208);
    delay(213.578802083);
    tone(bzrChannel, 493, 2.24819791667);
    delay(2.24819791667);
    delay(105.103252604);
    tone(bzrChannel, 349, 0.562049479167);
    delay(0.562049479167);
    tone(bzrChannel, 440, 53.3947005208);
    delay(53.3947005208);
    delay(52.8326510417);
    delay(214.140851563);
    tone(bzrChannel, 493, 1.6861484375);
    delay(1.6861484375);
    tone(bzrChannel, 349, 106.227351563);
    delay(106.227351563);
    delay(6.18254427083);
    tone(bzrChannel, 440, 53.95675);
    delay(53.95675);
    delay(53.3947005208);
    tone(bzrChannel, 493, 53.95675);
    delay(53.95675);
    delay(52.2706015625);
    tone(bzrChannel, 659, 53.95675);
    delay(53.95675);
    delay(53.3947005208);
    delay(214.140851563);
    tone(bzrChannel, 587, 1.6861484375);
    delay(1.6861484375);
    tone(bzrChannel, 493, 52.2706015625);
    delay(52.2706015625);
    delay(52.2706015625);
    tone(bzrChannel, 523, 53.95675);
    delay(53.95675);
    delay(53.95675);
    tone(bzrChannel, 659, 107.9135);
    delay(107.9135);
    delay(6.18254427083);
    tone(bzrChannel, 493, 53.95675);
    delay(53.95675);
    delay(53.3947005208);
    delay(533.947005208);
    tone(bzrChannel, 391, 5.62049479167);
    delay(5.62049479167);
    tone(bzrChannel, 493, 48.3362552083);
    delay(48.3362552083);
    delay(53.95675);
    tone(bzrChannel, 391, 107.9135);
    delay(107.9135);
    delay(6.18254427083);
    tone(bzrChannel, 293, 53.95675);
    delay(53.95675);
    delay(53.3947005208);
    delay(641.860505208);
    tone(bzrChannel, 329, 5.62049479167);
    delay(5.62049479167);
    tone(bzrChannel, 293, 102.293005208);
    delay(102.293005208);
    delay(6.18254427083);
    tone(bzrChannel, 329, 53.95675);
    delay(53.95675);
    delay(53.3947005208);
    delay(213.578802083);
    tone(bzrChannel, 349, 2.24819791667);
    delay(2.24819791667);
    delay(105.103252604);
    tone(bzrChannel, 391, 0.562049479167);
    delay(0.562049479167);
    tone(bzrChannel, 440, 53.3947005208);
    delay(53.3947005208);
    delay(52.8326510417);
    delay(214.140851563);
    tone(bzrChannel, 493, 1.6861484375);
    delay(1.6861484375);
    tone(bzrChannel, 523, 106.227351563);
    delay(106.227351563);
    delay(6.18254427083);
    tone(bzrChannel, 493, 53.95675);
    delay(53.95675);
    delay(53.3947005208);
    delay(641.860505208);
    tone(bzrChannel, 329, 5.62049479167);
    delay(5.62049479167);
    tone(bzrChannel, 349, 102.293005208);
    delay(102.293005208);
    delay(6.18254427083);
    tone(bzrChannel, 391, 53.95675);
    delay(53.95675);
    delay(53.3947005208);
    delay(213.578802083);
    tone(bzrChannel, 440, 2.24819791667);
    delay(2.24819791667);
    delay(105.103252604);
    tone(bzrChannel, 493, 0.562049479167);
    delay(0.562049479167);
    tone(bzrChannel, 523, 53.3947005208);
    delay(53.3947005208);
    delay(52.8326510417);
    delay(214.140851563);
    tone(bzrChannel, 587, 1.6861484375);
    delay(1.6861484375);
    tone(bzrChannel, 659, 106.227351563);
    delay(106.227351563);
    delay(6.18254427083);
    tone(bzrChannel, 698, 53.95675);
    delay(53.95675);
    delay(53.3947005208);
    delay(641.860505208);
    tone(bzrChannel, 783, 5.62049479167);
    delay(5.62049479167);
    tone(bzrChannel, 293, 102.293005208);
    delay(102.293005208);
    delay(6.18254427083);
    tone(bzrChannel, 329, 53.95675);
    delay(53.95675);
    delay(53.3947005208);
    delay(213.578802083);
    tone(bzrChannel, 349, 2.24819791667);
    delay(2.24819791667);
    delay(105.103252604);
    tone(bzrChannel, 391, 0.562049479167);
    delay(0.562049479167);
    tone(bzrChannel, 440, 53.3947005208);
    delay(53.3947005208);
    delay(52.8326510417);
    delay(214.140851563);
    tone(bzrChannel, 493, 1.6861484375);
    delay(1.6861484375);
    tone(bzrChannel, 523, 106.227351563);
    delay(106.227351563);
    delay(6.18254427083);
    tone(bzrChannel, 493, 53.95675);
    delay(53.95675);
    delay(47.21215625);
    tone(bzrChannel, 329, 647.481);
    delay(647.481);
    tone(bzrChannel, 329, 107.9135);
    delay(107.9135);
    tone(bzrChannel, 293, 53.95675);
    delay(53.95675);
    delay(53.95675);
    delay(6.74459375);
    tone(bzrChannel, 349, 47.21215625);
    delay(47.21215625);
    delay(53.95675);
    tone(bzrChannel, 349, 6.74459375);
    delay(6.74459375);
    tone(bzrChannel, 329, 47.21215625);
    delay(47.21215625);
    delay(58.4531458333);
    delay(107.351450521);
    tone(bzrChannel, 391, 0.562049479167);
    delay(0.562049479167);
    tone(bzrChannel, 349, 53.3947005208);
    delay(53.3947005208);
    delay(52.8326510417);
    delay(106.227351563);
    tone(bzrChannel, 440, 1.6861484375);
    delay(1.6861484375);
    tone(bzrChannel, 391, 52.2706015625);
    delay(52.2706015625);
    delay(53.95675);
    tone(bzrChannel, 493, 107.9135);
    delay(107.9135);
    delay(6.18254427083);
    tone(bzrChannel, 440, 53.95675);
    delay(53.95675);
    delay(53.3947005208);
    delay(106.227351563);
    tone(bzrChannel, 523, 1.6861484375);
    delay(1.6861484375);
    tone(bzrChannel, 493, 52.2706015625);
    delay(52.2706015625);
    delay(53.3947005208);
    delay(107.351450521);
    tone(bzrChannel, 587, 0.562049479167);
    delay(0.562049479167);
    tone(bzrChannel, 523, 53.3947005208);
    delay(53.3947005208);
    delay(52.8326510417);
    tone(bzrChannel, 493, 53.95675);
    delay(53.95675);
    delay(104.541203125);
    tone(bzrChannel, 440, 53.95675);
    delay(53.95675);
    delay(1.6861484375);
    tone(bzrChannel, 493, 1294.962);
    delay(1294.962);
}

//This handles printing out the timer and how long is left for the duration entered by the user.
void timer(String inputtedDuration) {
    int duration = inputtedDuration.toInt();
  
    for (;duration>0;duration--) {
        lcd.clear();
        lcd.print(duration,DEC); //This outputs the time left to the attached LCD display
        //Serial.println(duration);
        delayMicroseconds(998000); //Delay for 0.998 because clear() takes 2000microseconds .
        
    }
    lcd.clear();
    lcd.print("Time\'s up!"); //This changes the time left to a message for the user.
    alarm(); //This plays the alarm song after the timer is over
    
  
}

//This handles setup of the connection to the internet, initialising the server, initialising the LCD and connection of the buzzer.
void setup() {
  Serial.begin(115200);
  //This connects the buzzer, on whichever pin it is connected to, to the channel associated to it
  ledcAttachPin(bzrPin, bzrChannel);
  ledcSetup(bzrChannel, alarmFreq, bzrResolution);
  //Wire.setClock(100);
  //This connects the ESP32 to the internet
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    return;
  }

  //This turns on the LCD and sets the cursor to 0,0 with a blank screen.
  lcd.begin(SDAPin, SCLPin); //SDA, SCL
  lcd.backlight();
  lcd.home();
  lcd.print("Waiting...");

  // Send a HTML page with a single input field so the user can enter the time for the timer.
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputtedDuration;
    String inputParam;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      String inputVal = request->getParam(PARAM_INPUT_1)->value();
      inputtedDuration = inputVal;
      inputParam = PARAM_INPUT_1;
      request->send(200, "text/html", "<body>Please wait whilst the timer runs </body>");
      timer(inputtedDuration); //This then runs the timer for the duration necessary, whose function also contains a call to the alarm to play the alarm song.
      request->send(200, "text/html", "<body>Time's Up! </body>");
      ESP.restart(); //Then the ESP32 is restarted in order to clear any leftover memory.
    }
    
    
  });
  server.onNotFound(notFound);
  server.begin(); //This just initialises the ESP32 webserver
}

void loop() {
}
