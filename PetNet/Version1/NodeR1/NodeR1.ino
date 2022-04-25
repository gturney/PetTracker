/*
  PetNet

  This is code to be run on an esp32 and used as the Node to be worn by a Pet.  This node will be locatable
  via its signal strength read by the "Finder" node or by nearby Relay nodes.

  Nodes:

      Description             NodeName      NodeId        MAC
          esp32 Dev Board =   Pet1          682343137     0x24, 0x6F, 0x28, 0xAB, 0xBA, 0xE0
      TTGO damaged Screen =   Finder1       682617761     0x24, 0x6F, 0x28, 0xAF, 0xEB, 0xA0
      TTGO broken Screen  =   Pet2          680951025     0x24, 0x6F, 0x28, 0x96, 0x7C, 0xF0
*/

#include "painlessMesh.h"

#define   MESH_PREFIX     "PetNet"
#define   MESH_PASSWORD   "605Artreva"
#define   MESH_PORT       9523

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

//Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

String NodeName = "Repeater1";

typedef struct NodeInfo {
  int State;
  int NodeID;
  String NodeName;
  String PetName;
} NodeInfo;

NodeInfo EchoReply;


void sendMessage() {
  String msg = "Hi from NodeF1 ";
  msg += mesh.getNodeId();
  mesh.sendBroadcast( msg );
  //taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
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

//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  //userScheduler.addTask( taskSendMessage );
  //taskSendMessage.enable();
  //Serial.print(NodeName);Serial.print(": ");Serial.println(mesh.getNodeId());
}


void loop() {
  // it will run the user scheduler as well
  mesh.update();
}
