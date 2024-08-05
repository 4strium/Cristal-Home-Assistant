#include <WiFi.h>
#include <HTTPClient.h>

const char* serverUrl = "http://178.128.174.115:3000/execute";
const char* apiKey;

void exec_com_assistant(String apiKey, String deviceId, String modelId, String phrase) {

  HTTPClient http;

  // Commence la requête POST
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("x-api-key", apiKey.c_str());

  // Corps de la requête JSON avec paramètres
  String jsonPayload = "{\"deviceId\": \"" + deviceId + "\", \"deviceModelId\": \"" + modelId + "\", \"phrase\": \"" + phrase + "\"}";

  // Envoie la requête
  int httpResponseCode = http.POST(jsonPayload);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Réponse du serveur : " + response);
  } else {
    Serial.println("Erreur de la requête HTTP : " + String(httpResponseCode));
  }

  http.end();
}