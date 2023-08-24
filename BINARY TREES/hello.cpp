#include <WiFi101.h>
#include<WiFiSSLClient.h>
#include <RTCZero.h>

const char* ssid = SECRET_SSID;    //  your network SSID (name)
const char* password = SECRET_PSWD;  // your network password
String httpsRequest = SECRET_REQUEST; // your Blynk API token

const char* host = "hooks.zapier.com";
WiFiSSLClient client;

RTCZero rtc; // create RTC object

/* Change these values to set the current initial time */
const byte seconds = 0;
const byte minutes = 0;
const byte hours = 16;

/* Change these values to set the current initial date */
const byte day = 4;
const byte month = 12;
const byte year = 17;


int lightPin = A0; //the analog pin the light sensor is connected to
int tempPin = A1; //the analog pin the TMP36's Vout (sense) pin is connected to
int moisturePin= A2; 

// Set this threeshold accordingly to the resistance you used
// The easiest way to calibrate this value is to test the sensor in both dry and wet earth 
int threeshold= 800; 

bool alert_already_sent=false;
bool email_already_sent=true;

void setup() {
    Serial.begin(9600);
    while(!Serial);
    delay(2000);
    Serial.print("Connecting Wifi: ");
    Serial.println(ssid);
    while (WiFi.begin(ssid, password) != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
    }
  Serial.println("");
  Serial.println("WiFi connected");
  
  
  rtc.begin(); // initialize RTC 24H format
  rtc.setTime(hours, minutes, seconds);
  rtc.setDate(day, month, year);
  rtc.setAlarmTime(16, 1, 0);  // Set the time for the Arduino to send the email
  rtc.enableAlarm(rtc.MATCH_HHMMSS);
  rtc.attachInterrupt(alarmMatch);
}

void loop() {
  
/*  Serial.print(get_temperature());
  Serial.println(" degrees C ");
  Serial.print(get_light());
  Serial.println(" light");
  Serial.print(get_average_moisture());
  Serial.println(" moisture level");
  Serial.println("");

  delay(1000);*/
  
  String warning="";
  // Send an extra email only if the plant needs to be waterd
  if(get_average_moisture() < threeshold && !alert_already_sent){ 
    warning ="Warning your plant needs water !"; // Insert here your emergency message
    warning.replace(" ", "%20");
    send_email(get_temperature(), get_average_moisture(),get_light(), warning);
    alert_already_sent=true; // Send the alert only once
  } 
  
  // Send the daily email
  if(!email_already_sent){
    send_email(get_temperature(), get_average_moisture(),get_light(), warning);
    email_already_sent=true;
  }

}

float get_temperature(){
  int reading = analogRead(tempPin);  
  float voltage = reading * 3.3;
  voltage /= 1024.0; 
  
 // Print tempeature in Celsius
 float temperatureC = (voltage - 0.5) * 100 ; //converting from 10 mv per degree wit 500 mV offset

 // Convert to Fahrenheit
 float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
 
 return temperatureC;
}
int get_average_moisture(){ // make an average of 10 values to be more acurate
  
  int tempValue=0; // variable to temporarly store moisture value
  for(int a=0; a<10; a++){
    tempValue+=analogRead(moisturePin);
    delay(10);
  }
  return tempValue/10;
}
int get_light(){
  int light_value=analogRead(A0);
  return light_value;
}

void alarmMatch(){ // triggered when the alarm goes on
  Serial.println("Alarm Match!");
  email_already_sent = false;
  alert_already_sent = false;
}

void send_email(float temperature, int moisture, int light, String warning){
  
  // convert values to String
  String _temperature = String(temperature);
  String _moisture = String(moisture);
  String _light = String(light);
  String _warning = warning;

  if (client.connect(host, 443)) {
    client.println("POST "+httpsRequest+"?temperature="+_temperature+"&moisture="+_moisture+"&light="+_light+"&warning="+_warning+" HTTP/1.1");
    client.println("Host: "+ String(host));
    client.println("Connection: close");
    client.println();
    
    delay(1000);
    while (client.available()) { // Print on the console the answer of the server
      char c = client.read();
      Serial.write(c);
    }
    client.stop();  // Disconnect from the server
  }
  else {
    Serial.println("Failed to connect to client");
  }
}




