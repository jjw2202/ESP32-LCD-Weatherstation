
// upload to spiffs tutorial: https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/
// upload to spiffs plugin: https://github.com/me-no-dev/arduino-esp32fs-plugin

//kindly copied from https://github.com/programminghoch10/LEDClock

//SPIFFS data management functions

#define WIFI_FILE_NAME "/wifi.cfg"
const size_t WIFI_JSON_CAPACITY = JSON_OBJECT_SIZE(2) + 120;

//TODO: rewrite to support direct serialization to file

void savewifisettings(String ssid, String pass) {
  DynamicJsonDocument doc(WIFI_JSON_CAPACITY);
  ssid = ssid.length() > 32 ? ssid.substring(0,32) : ssid; //max 32 chars
  pass = pass.length() > 63 ? pass.substring(0,63) : pass; //max 63 chars
  doc["ssid"] = ssid;
  doc["pass"] = pass;
  char output[measureJson(doc)];
  serializeJson(doc, output, sizeof(output));
  writeFile(WIFI_FILE_NAME, output);
}

ws_t loadwifisettings() {
  String json = readFile(WIFI_FILE_NAME);
  DynamicJsonDocument doc(WIFI_JSON_CAPACITY);
  deserializeJson(doc, json);
  ws_t ws;
  ws.ssid = doc["ssid"].as<String>();
  ws.pass = doc["pass"].as<String>();
  return ws;
}


//SPIFFS Core Functions

#define FORMAT_SPIFFS_IF_FAILED true

bool spiffsavailable = false;

bool SPIFFSsetup() {
  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
    Serial.println("SPIFFS Mount Failed, retrying...");
    delay(100);
    if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
      Serial.println("SPIFFS Mount Failed twice!");
      return false;
    }
  }
  spiffsavailable = true;
  return true;
}



String readFile(const char * path){
  Serial.printf("Reading file: %s\r\n", path);
  File file = SPIFFS.open(path);
  if(!file || file.isDirectory()){
    Serial.println("Failed to open file for reading!");
    return "";
  }
  Serial.println("File size is: " + String(file.size()));
  String returnstring;
  if(file.available()) {
    returnstring = file.readString();
  }
  //Serial.println("Read file contents: " + String(returnstring));
  return returnstring;
}

bool writeFile(const char * path, const char * content){
  Serial.printf("Writing file: %s\r\n", path);
  File file = SPIFFS.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing!");
    return false;
  }
  if(file.print(content)){
    return true;
  } else {
    Serial.println("File write failed");
    return false;
  }
}

bool appendFile(const char * path, const char * content){
  Serial.printf("Appending to file: %s\r\n", path);
  File file = SPIFFS.open(path, FILE_APPEND);
  if(!file){
    Serial.println("- failed to open file for appending");
    return false;
  }
  if(file.print(content)){
    return true;
  } else {
    Serial.println("File append failed");
    return false;
  }
}

bool renameFile(const char * path1, const char * path2){
  Serial.printf("Renaming file %s to %s\r\n", path1, path2);
  if (SPIFFS.rename(path1, path2)) {
    return true;
  } else {
    Serial.println("File rename failed!");
    return false;
  }
}

bool deleteFile(const char * path){
  Serial.printf("Deleting file: %s\r\n", path);
  if(SPIFFS.remove(path)){
    return true;
  } else {
    Serial.println("File delete failed!");
    return false;
  }
}
