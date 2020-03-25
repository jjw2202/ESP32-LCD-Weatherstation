# ESP32-LCD-Weatherstation

A small Weatherstation using only an ESP32 and a LCD display.

## Setup

You only need to give it 3 things:

* Your WiFi Credentials
* An IPAPI Key

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

