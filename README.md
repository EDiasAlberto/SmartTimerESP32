# SmartTimerESP32
A timer for the ESP32 (needs an I2C LCD and buzzer) that can be controlled from the internet.

#Requirements:
+ESP32 Microprocessor
+I2C LCD display
+Buzzer
+Resistors (optional)

To use the file, ensure that the "ssid" and "password" variables are filled with the appropriate values for your network connection.
The device does not require a connection to the internet, only to the local area network so other devices can communicate with it.
To change the port for the webserver, change the integer value in line 9 for the initialisation function.
