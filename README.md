# ESP32-LCD-Weatherstation

A small Weatherstation using only an ESP32 and a LC-Display.

## Features

Our weather station has quite some special features:

* Displays weather data of current location
* Automatically switches language depending on location
* Automatic  detection of I²C or directly connected LCD

A little bit more hidden, but also relevant:

* Stutter free experience due to parallel processing
* Nice Icons
* No irrelevant information due to a neat and tidy layouted display

## Setup

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

### Optional parameters

The weather station can optionally be configured by changing some constants.

Parameter | Unit | Description | Default
--------- | ---- | ----------- | -------
WEATHER_CHANGE_SCREEN | Seconds | How often the weather station should switch the displayed values. | 5
WEATHER_UPDATE_INTERVAL | Minutes | How often the displayed weather data should be updated. | 5
POSITION_UPDATE_INTERVAL | Hours | How often the position data should be updated. | 2
