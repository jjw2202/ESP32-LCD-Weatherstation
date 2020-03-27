
void updateposition() {
  if (WiFi.status() != WL_CONNECTED) {
    wificonnect();
    if (WiFi.status() != WL_CONNECTED) return;
  }
  Serial.println("Updating position...");
  if (!ipaddress.valid) {
    ia_t newipaddress = getipaddress();
    if (newipaddress.valid) ipaddress = newipaddress;
  }
  if (ipaddress.valid) {
    pos_t newposition = getposition(ipaddress);
    if (newposition.valid) position = newposition;
  }
  saveposition();
}

WiFiClientSecure client;
String getexternalip() {
  //https://myexternalip.com/raw
  const char * hostname = "myexternalip.com";
  const String url = "/raw";
  client.connect(hostname, 443);
  client.print(String("GET ") + url + " HTTP/1.1\r\n"
    + "Host: " + hostname + "\r\n"
    + "Connection: close\r\n\r\n"
    + "Accept: application/json\r\n"
  );
  uint64_t sentmillis = millis();
  while ((millis() - sentmillis <= IP_RESPONSE_TIMEOUT) && !client.available()) delay(1);
  if (!client.available()) {
    Serial.println("failed to obtain external ip address");
    //fatal error in connection
    client.stop();
    return "";
  }
  String answer;
  while (client.available()) answer = client.readStringUntil('\n');
  Serial.println("External IP Address: " + String(answer));
  client.stop();
  return answer;
}

ia_t getipaddress() {
  ia_t ia;
  ia.externalip = "";
  ia.internalip = "";
  String externalip = getexternalip();
  if (externalip.equals("")) return ia;
  ia.externalip = externalip;
  ia.internalip = WiFi.localIP().toString();
  ia.valid = true;
  return ia;
}

pos_t getposition(ia_t ipaddress) {
  //https://ipapi.co/8.8.8.8/json/
  pos_t position;
  const char * hostname = "ipapi.co";
  const String url = "/" + String(ipaddress.externalip) + "/json" 
  #ifdef IPAPI_KEY
    + "?key=" + IPAPI_KEY
  #endif
  ;
  String answer;
  //Serial.println(String(hostname) + String(url));
  client.connect(hostname, 443);
  client.print(String("GET ") + url + " HTTP/1.1\r\n"
    + "Host: " + hostname + "\r\n"
    + "Connection: close\r\n\r\n"
    + "Accept: application/json\r\n"
  );
  uint64_t sentmillis = millis();
  while ((millis() - sentmillis <= IP_RESPONSE_TIMEOUT) && !client.available()) delay(1);
  if (!client.available()) {
    Serial.println("failed to obtain position");
    //fatal error in connection
    client.stop();
    return position;
  }
  
  answer = client.readStringUntil('{');
  answer = String("{") + client.readStringUntil('\r');
  client.stop();
  //Serial.println("Position data: " + String(answer));
  //answer = answer.substring(0); // cut off line seperator at beginning
  StaticJsonDocument<1000> doc;
  DeserializationError error = deserializeJson(doc, answer);
  if (error) {
    Serial.println("error during position deserialization");
    Serial.println(error.c_str());
    return position;
  }
  position.latitude = doc["latitude"];
  position.longitude = doc["longitude"];
  position.valid = !(position.latitude == 0 && position.longitude == 0);
  Serial.println("Latitude: " + String(position.latitude));
  Serial.println("Longitude: " + String(position.longitude));
  return position;
}
