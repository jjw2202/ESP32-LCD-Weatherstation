#include "globals.h"

#define WIFI_FILE_NAME "/wifi.cfg"
#define POSITION_FILE_NAME "/position.json"

void savewifisettings(String ssid, String pass);

ws_t loadwifisettings();

void loadposition();
void loadposition(bool forceoverwrite);

void saveposition();

bool SPIFFSsetup();

String readFile(const char * path);

bool writeFile(const char * path, const char * content);

bool appendFile(const char * path, const char * content);

bool renameFile(const char * path1, const char * path2);

bool deleteFile(const char * path);
