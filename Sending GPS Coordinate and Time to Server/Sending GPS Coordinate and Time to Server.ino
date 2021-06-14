#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "GIONEE GN5005L";
const char* password = "00000000";

//Your Domain name with URL path or IP address with path
String serverName = "https://omni-chi.vercel.app/api/ping";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

static const int RXPin = 18, TXPin = 19;
static const uint32_t GPSBaud = 9600;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

TinyGPSPlus gps;

String gpslatitude(){
//  Serial.println("GPS Location");
  if(gps.location.isValid()){
    float latitude = gps.location.lat();
    float latitude1 = Serial.print(latitude, 6);
    return String(latitude);
  }
  else{
    return "000";
  }
}


String gpslongitude(){
 // Serial.println("GPS Location");
  if(gps.location.isValid()){
    float longitude = gps.location.lng();
    float longitude1 = Serial.print(longitude, 6);
    return  String(longitude);
  }
  else{
    return "000";
  }
  }

String gpsday(){
  if (gps.date.isValid()){
    int day1 = gps.date.day();
    return String(day1);
  }
  else{
    return "00";
  }
}

String gpsmonth(){
  if (gps.date.isValid()){
    int month1 = gps.date.month();
    return String(month1);
  }
  else{
    return "00";
  }
}

String gpsyear(){
  if (gps.date.isValid()){
    int year1 = gps.date.year();
    return String(year1);
  }
  else{
    return "00";
  }
}

String gpsdate(){
  String year1 = gpsyear();
  String month1 = gpsmonth();
  String day1 = gpsday();

  String date1 = month1 + "/" + day1 + "/" + year1;
  return date1;
}

String gpshour(){
  if(gps.time.isValid()){
    int hour1 = gps.time.hour() + 1;
    if(hour1 < 10){
      int hour2 = 0 + hour1;
      return String(hour2);
    }
    else{
      return String(hour1);
    }
  }
  else{
    return "00";
  }
}

String gpsminute(){
  if(gps.time.isValid()){
    int minute1 = gps.time.minute();
    if(minute1 < 10){
      int minute2 = 0 + minute1;
      return String(minute2);
    }
    else{
      return String(minute1);
    }
  }
  else{
    return "00";
  }
}

String gpssecond(){
  if(gps.time.isValid()){
    int second1 = gps.time.second();
    if(second1 < 10){
      int second2 = 0 + second1;
      return String(second2);
    }
    else{
      return String(second1);
    }
  }
  else{
    return "00";
  }
}

String gpstime(){
  String hour1, minute1, second1;
  hour1 = gpshour();
  minute1 = gpsminute();
  second1 = gpssecond();

  String time1 = hour1 + ":" + minute1 + ":" + second1;
  return time1;
}

int voltage(){

  int volt = analogRead(2);
  Serial.println(volt);
  return volt;

}

String senddataonrequest(){
 
  String serverdata = "?GPSLATITUDE="+gpslatitude() + "&GPSLONGITUDE="+gpslongitude() + "&GPSTIME="+gpstime() + "&GPSDATE="+gpsdate() + "&GPS_STATUS=ON";
  return serverdata;
}

String senddataoffrequest(){
 
  String serverdata = "?GPSLATITUDE="+gpslatitude() + "&GPSLONGITUDE="+gpslongitude() + "&GPSTIME="+gpstime() + "&GPSDATE="+gpsdate() + "&GPS_STATUS=OFF";
  return serverdata;
}

String sendtoserver(String Send_data){
  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      
        
      Serial.println(Send_data);

      String serverPath = serverName + Send_data;

      Serial.println(serverPath);
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
       // String payload = http.getString();
       // Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    
    
    lastTime = millis();
  }
}

void setup() {
  Serial.begin(115200); 
  ss.begin(GPSBaud);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
  while (ss.available() > 0){
    if (gps.encode(ss.read())){
      Serial.println(gpstime());
      }
    }
}

void loop() {
  while (ss.available() > 0)
      if (gps.encode(ss.read()))
      if (voltage() > 2000){
        sendtoserver(senddataonrequest());
      }
      else{
        sendtoserver(senddataoffrequest());
      }
      
  
}
