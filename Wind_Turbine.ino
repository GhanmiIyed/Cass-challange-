/*
  Wind Turbine Minimal Telemetry Over painlessMesh
  Fields:
  - Date/Time
  - LV ActivePower (kW)
  - Wind Speed (m/s)
  - Theoretical_Power_Curve (kWh)
  - Wind Direction (°)
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

  // ---- Random Dynamic Values ----
  float LV_ActivePower = random(500, 2000) / 1.0;         // 500–2000 kW
  float windSpeed = random(30, 180) / 10.0;               // 3.0–18.0 m/s
  float theoreticalPower = windSpeed * windSpeed * windSpeed * 0.5; // simple model
  int windDirection = random(0, 360);

  // Static timestamp (add NTP if needed)
  String timestamp = "2025-11-20T18:45:12.000Z";

  // ---- Build JSON ----
  String msg = "{";
  msg += "\"DateTime\":\"" + timestamp + "\",";
  msg += "\"LV_ActivePower_kW\":" + String(LV_ActivePower, 1) + ",";
  msg += "\"WindSpeed_m_s\":" + String(windSpeed, 2) + ",";
  msg += "\"Theoretical_Power_Curve_kWh\":" + String(theoreticalPower, 2) + ",";
  msg += "\"WindDirection_deg\":" + String(windDirection);
  msg += "}";

  // ---- Broadcast ----
  mesh.sendBroadcast(msg);

  // Send every 1–3 seconds
  taskSendMessage.setInterval(random(1000, 3000));
}

// ===============================
//          CALLBACKS
// ===============================
void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("RX from %u: %s\n", from, msg.c_str());
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
