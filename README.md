# ESP32-LCD-Weatherstation

A small weather station using only an ESP32 and a 16x2 LCD.

## Hardware Setup

Our weather station supports LCDs connected directly to the ESP32 as well as those controlled by I2C. The special thing about it: you don't have to change anything in the code - plug and play.

### I²C

| ESP32 | LCD     |
|-------|---------|
| GND   | GND     |
| VIN   | VCC     |
| SDA   | GPIO 21 |
| SCL   | GPIO 22 |

![connection I²C](documentation/connection_I2C.png)

### Direct

| ESP32                            | LCD     |
|----------------------------------|---------|
| GND                              | VSS     |
| VIN                              | VDD     |
| via potentiometer to VIN and GND | V0      |
| GPIO 13                          | RS      |
| GND                              | RW      |
| GPIO 12                          | E       |
| -                                | D0      |
| -                                | D1      |
| -                                | D2      |
| -                                | D3      |
| GPIO 14                          | D4      |
| GPIO 27                          | D5      |
| GPIO 33                          | D6      |
| GPIO 32                          | D7      |
| via 220Ω resistor at VIN         | A       |
| GND                              | K       |

![connection direct](documentation/connection_direct.png)

## Software Setup

You only need to give it 3 things:

* Your WiFi Credentials
* An IPAPI Key
* An OpenWeatherAPI Key

### WiFi Credentials

Create a folder named `data` inside the project folder.

Inside it, create a file named `wifi.cfg`.

Place these contents inside:

```json
{
	"ssid": "SSID",
	"pass": "PASSWORD"
}
```

Now replace `SSID` and `PASSWORD` with your WiFi Name and Password accordingly.

You then need to upload it using the 
[Arduino ESP32 filesystem uploader by me-no-dev](https://github.com/me-no-dev/arduino-esp32fs-plugin).

### IPAPI Key

To get an IPAPI key for free, contact them at [ipapi.co/contact](https://ipapi.co/contact) and ask for a free trial key.

Then create and/or open the file `keys.h` inside the project folder.
Paste in `#define IPAPI_KEY "key"` and replace `key` with your key. 
The quotes must remain.

### OpenWeatherAPI Key

You can get a free OpenWeatherAPI key by registering at [their homepage](https://home.openweathermap.org/users/sign_up).

Then create and/or open the file `keys.h` inside the project folder.
Paste in `#define OPENWEATHERAPI_KEY "key"` and replace `key` with your key. 
The quotes must remain.