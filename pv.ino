*
  Solar Plant Telemetry Over painlessMesh
  Fields sent:
  - DATE_TIME
  - PLANT_ID
  - SOURCE_KEY
  - DC_POWER
  - AC_POWER
  - DAILY_YIELD
  - TOTAL_YIELD
  Houssem / 2025
*/

#include "painlessMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler;
painlessMesh mesh;

void sendMessage();
Task taskSendMessage(TASK_SECOND * 2, TASK_FOREVER, &sendMessage);

// ===============================
//           JSON SENDER
// ===============================
void sendMessage() {

  // ---- Random Test Values ----
  float dcPower = random(1000, 5000) / 1.0;     // 1–5 kW
  float acPower = dcPower * 0.95;               // inverter efficiency
  float dailyYield = random(1, 20) / 1.0;       // 1–20 kWh
  float totalYield = random(100, 5000) / 1.0;   // lifetime energy

  // Static timestamp (NTP can be added)
  String dateTime = "2025-11-20 18:45:12";

  // Static IDs (change if you want)
  String plantID = "PLANT_001";
  String sourceKey = "SRC_ABC_123";

  // ---- Build JSON ----
  String msg = "PV {";
  msg += "\"DATE_TIME\":\"" + dateTime + "\",";
  msg += "\"PLANT_ID\":\"" + plantID + "\",";
  msg += "\"SOURCE_KEY\":\"" + sourceKey + "\",";
  msg += "\"DC_POWER\":" + String(dcPower, 1) + ",";
  msg += "\"AC_POWER\":" + String(acPower, 1) + ",";
  msg += "\"DAILY_YIELD\":" + String(dailyYield, 2) + ",";
  msg += "\"TOTAL_YIELD\":" + String(totalYield, 2);
  msg += "}";

  // ---- Broadcast ----
  mesh.sendBroadcast(msg);

  // Random send interval
  taskSendMessage.setInterval(random(1000, 3000));
}

// ===============================
//           CALLBACKS
// ===============================
void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("RX from %u: %s\n", from, msg.c_str());
}

// ===============================
//             SETUP
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
//              LOOP
// ===============================
void loop() {
  mesh.update();
}
