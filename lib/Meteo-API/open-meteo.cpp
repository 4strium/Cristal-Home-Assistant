#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <open-meteo.h>

String url = "https://api.open-meteo.com/v1/forecast?latitude=48.427&longitude=-4.5598&current=temperature_2m,is_day,weather_code&forecast_days=1";

String init_http(void){
    // Utilisation de la bibliothèque HTTPClient pour effectuer une requête GET
    HTTPClient http;
    http.begin(url); // Début de la requête HTTP

    int code = http.GET(); // Effectuer la requête GET
    
    if (code > 0) { // Vérifier si la requête a réussi
    // Obtenir le contenu de la réponse
        String payload = http.getString();
        http.end();
        return payload;
    } else {
        Serial.println("Erreur lors de la requête HTTP !");
        http.end();
        return "error";
    }

}

float get_temp(void){    
    String req_rep = init_http();

    DynamicJsonDocument doc(1024); // Taille du document JSON

    // Désérialiser le JSON
    DeserializationError error = deserializeJson(doc, req_rep);

    // Vérifier s'il y a une erreur lors de la désérialisation
    if (!error) {
      // Extraire la valeur de "temperature_2m"
      float temperature = doc["current"]["temperature_2m"];  
      return temperature;
    } else {
      Serial.println("Erreur lors de la désérialisation du JSON !");
    }
}

int get_WMO(void){    
    String req_rep = init_http();

    DynamicJsonDocument doc(1024); // Taille du document JSON

    // Désérialiser le JSON
    DeserializationError error = deserializeJson(doc, req_rep);

    // Vérifier s'il y a une erreur lors de la désérialisation
    if (!error) {
      // Extraire la valeur de "weather_code"
      int wmo = doc["current"]["weather_code"];  
      return wmo;
    } else {
      Serial.println("Erreur lors de la désérialisation du JSON !");
    }
}

int get_day_night(void){    
    String req_rep = init_http();

    DynamicJsonDocument doc(1024); // Taille du document JSON

    // Désérialiser le JSON
    DeserializationError error = deserializeJson(doc, req_rep);

    // Vérifier s'il y a une erreur lors de la désérialisation
    if (!error) {
      // Extraire la valeur de "is_day"
      int dn = doc["current"]["is_day"];  
      return dn;
    } else {
      Serial.println("Erreur lors de la désérialisation du JSON !");
    }
}

