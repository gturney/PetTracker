/*
  PetNet

  This code is meant to run on an esp32 and used as the Node to be worn by a Pet.  This node will be locatable
  via its signal strength read by the "Finder" node or by nearby Relay nodes.

  Nodes:

      Description             NodeName      NodeId
          esp32 Dev Board =   Pet1          682343137     0x24, 0x6F, 0x28, 0xAB, 0xBA, 0xE0
      TTGO damaged Screen =   Finder1       682617761     0x24, 0x6F, 0x28, 0xAF, 0xEB, 0xA0
      TTGO broken Screen  =   Pet2          680951025     0x24, 0x6F, 0x28, 0x96, 0x7C, 0xF0
      TTGO New-1          =   Logger        2475132237  
*/

#include "painlessMesh.h"

#define   MESH_PREFIX     "PetNet"
#define   MESH_PASSWORD   "605Artreva"
#define   MESH_PORT       9523

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

void sendMessage() ; // Prototype so PlatformIO doesn't complain
void reinit(); // Re-Initialize mesh to force Pet tracker to connect to closest Node

//Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );
Task taskReinit(20000, TASK_FOREVER, &reinit);

String NodeName = "Pet1";

typedef struct NodeInfo {
  int State;
  int NodeID;
  String NodeName;
  String PetName;
} NodeInfo;

NodeInfo EchoReply;



void sendMessage() {
  String msg = "Hi from NodeP1 ";
  msg += mesh.getNodeId();
  mesh.sendBroadcast( msg );
  //taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}

void reinit() {
  Serial.println("=================================");
  Serial.println("Entering the reinit() function: ");
  Serial.println(mesh.subConnectionJson());
  mesh.sendSingle(2475132237, mesh.subConnectionJson());
//  if ( mesh.isConnected(682617761)) {
//    Serial.println("Seems this node is connected to the Mesh.  Stopping this node.");
//    mesh.stop();
//    mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_STA );
//  }
  Serial.println("=================================");
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
//    if ( nodeId = 682617761 ) {
//      Serial.println("Connected to Finder1");
//      //mesh.sendBroadcast("Pet1 Connected to Finder1");
//      mesh.sendSingle(682617761, "Pet1 Connected to Finder1");
//    } else {
      String other = "Pet1 Connected to ";
      other += nodeId;
      Serial.println(other);
      mesh.sendSingle(68217761, other);
      mesh.sendSingle(2475132237, other);
//    }
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
//mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
  mesh.setDebugMsgTypes( ERROR | STARTUP | COMMUNICATION );

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_STA );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  //userScheduler.addTask( taskSendMessage );
  //taskSendMessage.enable();
  userScheduler.addTask( taskReinit );
  taskReinit.enable();
  //Serial.print(NodeName);Serial.print(": ");Serial.println(mesh.getNodeId());
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
}
