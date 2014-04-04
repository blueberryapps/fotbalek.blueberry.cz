#include <SPI.h>
#include <MAX7219.h>
#include <Ethernet.h>

// ethernet
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char server[] = "fotbalek.blueberry.cz";
EthernetClient client;
String url;

// LCD
static const int red_in = 30;
static const int blue_in = 32;
int count = 0;
int state_red = 0, state_blue = 0;
char red = '0';
char blue = '0';
char led[9];

const MAX7219_Topology topology[2] = {{MAX7219_MODE_7SEGMENT, 0, 0, 0, 7},{MAX7219_MODE_NC, 0, 8, 0, 7}};
const char alphabet[17] PROGMEM = "0123456789-EHLP ";
MAX7219 maxled;

void updateLED(char d0, char d1, char d2, char d3, char d4, char d5, char d6, char d7) {
  char temp[9] = {d0,d1,d2,d3,d4,d5,d6,d7};
  for (byte i = 0; i < 8; i++) led[i] = temp[i];
}

void ethernet_setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to configure using IP address instead of DHCP:
    Ethernet.begin(mac);
  }
}


void connect(String url)
{
  // give the Ethernet shield a second to initialize:
  //delay(1000);
  Serial.println("connecting");

  if (client.connect(server, 80)) {
    Serial.println("connected");
    client.println("GET /" + url + " HTTP/1.1");
    client.println("Host: fotbalek.blueberry.cz");
    client.println("Connection: close");
    client.println();
    
    if (client.available()) {
      char c = client.read();
      Serial.print(c);
    }
    if (client.connected()) {
      client.stop();
      Serial.println("disconnected");
    }
  } 
  else {
    Serial.println("connection failed");
  }
}

// SETUP
void setup() {
  maxled.begin(topology, 2);
  maxled.zeroDisplay();
  pinMode(red_in, INPUT);
  pinMode(blue_in, INPUT);
  ethernet_setup();
}

// LOOP
void loop() { 
  state_red = digitalRead(red_in);
  if (state_red == 0) {
    (int)red++;
    delay(100);
  }
  state_blue = digitalRead(blue_in);
  if (state_blue == 0) {
    (int)blue++;
    delay(100);
  }
   
  if (red > '9' || blue > '9') {
    url = "red=" + (String)red + "&blue=" + (String)blue;
    Serial.println("Game end: " + url);
    connect(url);
    url.c_str()
        
    if (red > '9') {
      for (byte i = 1; i < 15; i++) {
        updateLED('0','1',' ',' ',' ',' ',' ',blue);
        maxled.set7Segment(led);
        delay(300);
        updateLED(' ',' ',' ',' ',' ',' ',' ',blue);
        maxled.set7Segment(led);
        delay(300);
      }
    }
    if (blue > '9') {
      for (byte i = 1; i < 15; i++) {
        updateLED(red,' ',' ',' ',' ',' ','0','1');
        maxled.set7Segment(led);
        delay(300);
        updateLED(red,' ',' ',' ',' ',' ',' ',' ');
        maxled.set7Segment(led);
        delay(300);
      }
    }
    
    red = '0';
    blue = '0';
    Serial.println();
  }
  
  updateLED(red,' ',' ',' ',' ',' ',' ',blue);
  maxled.set7Segment(led);  
  delay(100);
}

