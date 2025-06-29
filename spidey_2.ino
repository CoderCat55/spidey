#include "Wire.h"
#include "Adafruit_PWMServoDriver.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80); // server port 80

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Page Not found");
}
// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define USMIN  550 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150 //normally 600
#define USMAX  2390 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600 //normally 2400
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates
#define A 0
#define B 1
#define C 2
#define D 3

#define a 4
#define b 5
#define c 6
#define d 7
/*usmın 550  usmax2390  oluyor diğer türlü servo zorlanıyor*/
struct POSDATA {
  int posA = 0, posB = 0, posC = 0, posD = 0; 
  int posa = 0, posb = 0, posc = 0, posd=0;  

};

POSDATA posdata;
char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Spidey</title>
    <style>
        .value-display {
            display: inline-block;
            width: 30px;
            text-align: right;
            margin-left: 10px;
        }
    </style>
</head>
<script>
document.addEventListener('DOMContentLoaded', function() {
    const sliders = document.querySelectorAll('.slider');
    
    sliders.forEach(slider => {
        // Create and display initial value
        const valueDisplay = document.createElement('span');
        valueDisplay.className = 'value-display';
        valueDisplay.textContent = slider.value;
        slider.parentNode.appendChild(valueDisplay);
        
        slider.addEventListener('input', function() {
            const servoId = this.id;
            const value = this.value;
            
            // Update the displayed value
            this.nextElementSibling.textContent = value;
            
            fetch(`/setServo?servo=${servoId}&pos=${value}`)
                .then(response => response.text())
                .then(data => console.log(data))
                .catch(error => console.error('Error:', error));
        });
    });
});
</script>
<body>
    <h1>Spidey</h1>
    <h2>made by codercat55</h2>
    <div class="slidecontainer">
      <label for="A">Servo A</label>
      <input type="range" min="0" max="180" value="90" class="slider" id="A">
    </div>
    <div class="slidecontainer">
      <label for="B">Servo B</label>
      <input type="range" min="0" max="180" value="90" class="slider" id="B">
    </div>
    <div class="slidecontainer">
      <label for="C">Servo C</label>
      <input type="range" min="0" max="180" value="90" class="slider" id="C">
    </div>
    <div class="slidecontainer">
      <label for="D">Servo D</label>
      <input type="range" min="0" max="180" value="90" class="slider" id="D">
    </div>
    <div class="slidecontainer">
      <label for="a">Servo a</label>
      <input type="range" min="0" max="180" value="90" class="slider" id="a">
    </div>
    <div class="slidecontainer">
      <label for="b">Servo b</label>
      <input type="range" min="0" max="180" value="90" class="slider" id="b">
    </div>
    <div class="slidecontainer">
      <label for="c">Servo c</label>
      <input type="range" min="0" max="180" value="90" class="slider" id="c">
    </div>
    <div class="slidecontainer">
      <label for="d">Servo d</label>
      <input type="range" min="0" max="180" value="90" class="slider" id="d">
    </div>
</body>
</html>
)=====";

void setup() {
  Serial.begin(115200);
  Serial.println("PCA9685ServoV1");

  if(!WiFi.softAP("spidey", "")) {
    Serial.println("Failed to start AP");
    while(1);
  }
  Serial.println("softap: ");
  Serial.println("");
  Serial.println(WiFi.softAPIP());

  if (!MDNS.begin("spidey")) { //spidey.local/
    Serial.println("Error setting up MDNS responder!");
  } else {
    Serial.println("MDNS responder started");
  }

  server.on("/", [](AsyncWebServerRequest * request) { 
    request->send_P(200, "text/html", webpage);
  });

  server.on("/setServo", HTTP_GET, [](AsyncWebServerRequest *request){
    if(request->hasParam("servo") && request->hasParam("pos")) {
        String servo = request->getParam("servo")->value();
        int pos = request->getParam("pos")->value().toInt();
        
        if(servo == "A") posdata.posA = pos;
        else if(servo == "B") posdata.posB = pos;
        else if(servo == "C") posdata.posC = pos;
        else if(servo == "D") posdata.posD = pos;
        else if(servo == "a") posdata.posa = pos;
        else if(servo == "b") posdata.posb = pos;
        else if(servo == "c") posdata.posc = pos;
        else if(servo == "d") posdata.posd = pos;
        
        request->send(200, "text/plain", "OK");
    } else {
        request->send(400, "text/plain", "Bad Request");
    }
  });

  server.onNotFound(notFound);
  server.begin();

  if (!pwm.begin()) {
    Serial.println("Failed to initialize PWM driver");
    while(1);
  }
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);
  delay(10);
}

void setServoPos(int servo, int pos){
  //This first bit of code makes sure we are not trying to set the servo outside of limits
  int sendPos;
  if(pos > 180){
    pos = 180;
  }
  if(pos < 0){
    pos = 0;
  }
  sendPos = USMIN + ((USMAX - USMIN)/180 * pos);
  pwm.writeMicroseconds(servo, sendPos);

}

void loop(){
  setServoPos(A,posdata.posA);
  setServoPos(B,posdata.posB);
  setServoPos(C,posdata.posC);
  setServoPos(D,posdata.posD);
  setServoPos(a,posdata.posa);
  setServoPos(b,posdata.posb);
  setServoPos(c,posdata.posc);
  setServoPos(d,posdata.posd);
  delay(200);
}