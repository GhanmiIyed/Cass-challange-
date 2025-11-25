/*
  Minimal Wind Turbine Telemetry Over painlessMesh
  Sends only timestamp + AP_MW
  Houssem / 2025
*/

#include "painlessMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler;
painlessMesh mesh;

// Forward declaration
void sendMessage();
Task taskSendMessage(TASK_SECOND * 2, TASK_FOREVER, &sendMessage);

// ===============================
//        JSON SENDER
// ===============================
void sendMessage() {

  // ---------- Dynamic Values ----------
  float AP_MW = random(500, 1500) / 1000.0;   // 0.5 → 1.5 MW

  // Static timestamp (or I can add real time via NTP)
  String timestamp = "2025-11-20T18:45:12.000Z";

  // ---------- Build JSON ----------
  String msg = "Grid {";
  msg += "\"timestamp\":\"" + timestamp + "\",";
  msg += "\"AP_MW\":" + String(AP_MW, 3);
  msg += "}";

  // ---------- Send ----------
  mesh.sendBroadcast(msg);

  // Random interval 1–3s
  taskSendMessage.setInterval(random(1000, 3000));
}

// ===============================
//          CALLBACKS
// ===============================
void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("RX from The %u: %s\n", from, msg.c_str());
}

// ===============================
//            SETUP
// ===============================
void setup() {
  Serial.begin(115200);

  mesh.setDebugMsgTypes(ERROR | STARTUP);
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);

  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();
}

// ===============================
//             LOOP
// ===============================
void loop() {
  mesh.update();
}
