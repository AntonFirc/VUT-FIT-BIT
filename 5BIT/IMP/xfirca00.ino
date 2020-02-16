 /*
    IMP - ESP8266: snímání teploty (IoT, WiFi AP pro mobilní telefon)
    Anton Firc (xfirca00)
    original
 */


#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h> 
#include <OneWire.h>
#include <DallasTemperature.h>

const char *ssid = "ThermoESP";
const char *password = "a1b2c3d4";

ESP8266WebServer server(80);

//------------------------------------------
//DS18B20
#define ONE_WIRE_BUS D1 //Pin to which is attached a temperature sensor

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
DeviceAddress devAddr;  //An array device temperature sensors
float tempDev; //Saving the last measurement of temperature
long lastTemp; //The last measurement
long lastHistory; //last temperature save
const int durationTemp = 1000; //The frequency of temperature measurement
const long durationHistory = 60000; //The frequency of temperature history saving

String tempHist[5][2] = {{"-", "-"}, {"-", "-"}, {"-", "-"}, {"-", "-"}, {"-", "-"}};

unsigned long timeNow = 0;
unsigned long timeLast = 0;
int startingHour = 13;
int seconds = 00;
int minutes = 23;
int hours = startingHour;
int days = 0;
int dailyErrorFast = 0; // set the average number of milliseconds your microcontroller's time is fast on a daily basis
int dailyErrorBehind = 0; // set the average number of milliseconds your microcontroller's time is behind on a daily basis
int correctedToday = 1;


/* Send message to server (http://192.168.4.1)
 */
void handleRoot() {
  char temperatureString[6];
  dtostrf(tempDev, 2, 2, temperatureString);
  String message;
  message += String(hours);
  message += ":";
  message += String(minutes);
  message += ":";
  message += String(seconds);
  message += " ";
  message += temperatureString;
  message += "<br>"; 
  for (int i = 0; i < 5; i++) { //include temperature history data
    message += tempHist[i][1];
    message += " ";
    message += tempHist[i][0];
    message += "<br>"; 
  }
  server.send(200, "text/html", message );
}

//Setting up the temperature sensor
void SetupDS18B20(){
  DS18B20.begin();

  lastTemp = millis();
  DS18B20.requestTemperatures();

  // Search the wire for address
  if( DS18B20.getAddress(devAddr, 0) ){
    Serial.print("Found device.");
  }else{
    Serial.print("Found ghost device, but could not detect address. Check power and cabling.");
  }

  //Read temperature from DS18B20
  float tempC = DS18B20.getTempC( devAddr );
  Serial.print("Temp C: ");
  Serial.println(tempC);
}

//Loop measuring the temperature
void TempLoop(long now){
  if( now - lastTemp > durationTemp ){ //Take a measurement at a fixed time (durationTemp = 1000ms, 1s)
    float tempC = DS18B20.getTempC( devAddr ); //Measuring temperature in Celsius
    DS18B20.setWaitForConversion(false); //No waiting for measurement
    DS18B20.requestTemperatures(); //Initiate the temperature measurement
    lastTemp = millis();  //Remember the last time measurement
    if ( now - lastHistory > durationHistory ) {  //Save temperature history at a fixed time (durationHistory = 60000ms, 1min)
      for (int i = 4; i > 0; i--) { //shift history data in buffer
        tempHist[i][0] = tempHist[i-1][0];
        tempHist[i][1] = tempHist[i-1][1];
      }
      //save latest temperature value and time of measurement
      tempHist[0][0] = String(tempDev);
      tempHist[0][1] = String(hours);
      tempHist[0][1] += ":";
      tempHist[0][1] += String(minutes);
      tempHist[0][1] += ":";
      tempHist[0][1] += String(seconds);
      lastHistory = lastTemp;
    }
    tempDev = tempC; //Save the measured value
  }
}

void setup() {
	delay(1000);
	Serial.begin(115200);
	Serial.print("Configuring access point...");
	
	WiFi.softAP(ssid, password);

	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);
	server.on("/", handleRoot);
	server.begin();
	Serial.println("HTTP server started");

  SetupDS18B20();
}

void loop() {
  long t = millis();
	server.handleClient();

  /* time measurement, source: https://www.instructables.com/id/TESTED-Timekeeping-on-ESP8266-Arduino-Uno-WITHOUT-/ */
  timeNow = millis()/1000; // the number of milliseconds that have passed since boot
  seconds = timeNow - timeLast;//the number of seconds that have passed since the last time 60 seconds was reached.
  
  if (seconds == 60) {
    timeLast = timeNow;
    minutes = minutes + 1;
  }
  
  //if one minute has passed, start counting milliseconds from zero again and add one minute to the clock.
  
  if (minutes == 60){ 
    minutes = 0;
    hours = hours + 1;
  }
  
  // if one hour has passed, start counting minutes from zero and add one hour to the clock
  
  if (hours == 24){
    hours = 0;
    days = days + 1;
    }
  
   //if 24 hours have passed , add one day
  
  if (hours ==(24 - startingHour) && correctedToday == 0){
    delay(dailyErrorFast*1000);
    seconds = seconds + dailyErrorBehind;
    correctedToday = 1;
  }
  
  //every time 24 hours have passed since the initial starting time and it has not been reset this day before, add milliseconds or delay the progran with some milliseconds. 
  //Change these varialbes according to the error of your board. 
  // The only way to find out how far off your boards internal clock is, is by uploading this sketch at exactly the same time as the real time, letting it run for a few days 
  // and then determine how many seconds slow/fast your boards internal clock is on a daily average. (24 hours).
  
  if (hours == 24 - startingHour + 2) { 
    correctedToday = 0;
  }
  
  //let the sketch know that a new day has started for what concerns correction, if this line was not here the arduiono
  // would continue to correct for an entire hour that is 24 - startingHour. 
  
  Serial.print("The time is:           ");
  Serial.print(hours);
  Serial.print(":");
  Serial.print(minutes);
  Serial.print(":"); 
  Serial.println(seconds); 
  
  TempLoop(t);  //measure temperature
}
