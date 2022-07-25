# AppendCar
## A Wi-Fi connected and controlled car based on ESP32cam
### The car is implemented by hardware and software (client and server-side) design
The Aim of this project is to develop a wireless surveillance bot with awesome features with the lowest hardware cost and footprint.  
The emphasis on software is higher to achieve maximum performance, fault-tolerance, failure recovery, low component count, integration with custom applications and so-on.

## Features
* ### Hardware 
  * Use of readily available components
  * Re-programmable with the option of programmer on board (plug and program)
  * rechargable battery with lipo charger on board
  * efficient motor driver with sleep mode
  * headlight and taillight with brightness control
  * Low component count
  * efficient voltage regulator with a low quiescent current
* ### Software
  * User-friendly web interface
  * Camera stream and controls on same page
  * Desktop and mobile views
  * Multi-touch controls
  * Car speed control and headlight/taillight brightness control
  * State save
  * capture feature with direct image download
  * check car status
  * On the fly SSID and password change for WiFi (STA) and Hotspot (AP) modes
  * Password validation.
  * Choose between STA and AP modes
  * Factory reset option
  * Check RSSI to check signal strength (from router)
  * change quality of Camera feed
## Dependencies
 * [LITTLEFS](https://github.com/lorol/LITTLEFS)
