/*
  PetNet

  This is code to be run on an esp32 and used as the Node to be worn by a Pet.  This node will be locatable
  via its signal strength read by the "Finder" node or by nearby Relay nodes.

  Nodes:

      Description             NodeName      NodeId        MAC
          esp32 Dev Board =   Pet1          682343137     0x24, 0x6F, 0x28, 0xAB, 0xBA, 0xE0
      TTGO damaged Screen =   Finder1       682617761     0x24, 0x6F, 0x28, 0xAF, 0xEB, 0xA0
      TTGO broken Screen  =   Pet2          680951025     0x24, 0x6F, 0x28, 0x96, 0x7C, 0xF0
      TTGO New-1          =   Logger        xxxxxxxxx     
*/

#include "painlessMesh.h"
#include <SPI.h>
#include <TFT_eSPI.h>
#include <Wire.h>
#include <Bounce2.h>

#define   MESH_PREFIX     "PetNet"
#define   MESH_PASSWORD   "605Artreva"
#define   MESH_PORT       9523
#define   FF17 &FreeSans9pt7b
#define   FF21 &FreeSansBold9pt7b
#define   ROW1 0,16
#define   ROW2 0,38
#define   ROW3 0,60
#define   ROW4 0,82
#define   ROW5 0,104
#define   ROW6 0,126
#define BUTTON1 35
#define BUTTON2 0

boolean button1Pressed = false;
boolean button2Pressed = false;

TFT_eSPI      tft = TFT_eSPI();
Bounce   debounce1 = Bounce();
Bounce   debounce2 = Bounce();
Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

//Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

String NodeName = "Logger";

typedef struct NodeInfo {
  int State;
  int NodeID;
  String NodeName;
  String PetName;
} NodeInfo;

NodeInfo EchoReply;




void sendMessage() {
  String msg = "Hi from Logger ";
  msg += mesh.getNodeId();
  mesh.sendBroadcast( msg );
  //taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  tft.print(msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);

  debounce1.attach(BUTTON1);
  debounce2.attach(BUTTON2);
  debounce1.interval(5);
  debounce2.interval(5);

  mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
//mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  //userScheduler.addTask( taskSendMessage );
  //taskSendMessage.enable();
  //Serial.print(NodeName);Serial.print(": ");Serial.println(mesh.getNodeId());

  tft.init();
  tft.setRotation(1); // Change 1 or 3 for display orientation
  BaseDisplay();
}

void BaseDisplay() {
  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(FF21);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(ROW1);
  tft.print("Message:");
  tft.setCursor(ROW3);
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
  debounce1.update();
  debounce2.update();

  if (debounce1.read() == LOW && button1Pressed == false) {
    Serial.println("Button 1 just pressed...");
    button1Pressed = true;
    //put code here for a button 1 press.
    BaseDisplay();
  }
  else if (debounce1.read() == HIGH && button1Pressed == true) {
    button1Pressed = false;
  }
  if (debounce2.read() == LOW && button2Pressed == false) {
    Serial.println("Button 2 just pressed...");
    button2Pressed = true;
  }
  else if (debounce2.read() == HIGH && button2Pressed == true) {
    button2Pressed = false;
  }
}
