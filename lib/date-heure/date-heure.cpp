#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <date-heure.h>

String url_dt = "http://api.timezonedb.com/v2.1/get-time-zone?key=WS28EZFET19B&format=json&by=position&lat=48.427&lng=-4.5598";

String init_http_dt(void){
    // Utilisation de la bibliothèque HTTPClient pour effectuer une requête GET
    HTTPClient http;
    http.begin(url_dt); // Début de la requête HTTP

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

String get_date(void) {    
    String req_rep = init_http_dt();

    DynamicJsonDocument doc(1024); // Taille du document JSON

    // Désérialiser le JSON
    DeserializationError error = deserializeJson(doc, req_rep);

    // Vérifier s'il y a une erreur lors de la désérialisation
    if (!error) {
        // Extraire la valeur de "formatted"
        String dateTime = doc["formatted"];
        
        // Extraire la date au format YYYY-MM-DD
        String year = dateTime.substring(0, 4);
        String month = dateTime.substring(5, 7);
        String day = dateTime.substring(8, 10);

        // Reformatage en DD-MM-YYYY
        String formattedDate = day + "-" + month + "-" + year;
        return formattedDate;
    } else {
        Serial.println("Erreur lors de la désérialisation du JSON !");
        return "error";
    }
}

String get_time(void) {    
    String req_rep = init_http_dt();

    DynamicJsonDocument doc(1024); // Taille du document JSON

    // Désérialiser le JSON
    DeserializationError error = deserializeJson(doc, req_rep);

    // Vérifier s'il y a une erreur lors de la désérialisation
    if (!error) {
        // Extraire la valeur de "formatted"
        String dateTime = doc["formatted"];
        return dateTime.substring(11,16); // Extraire l'heure (format HH:MM)
    } else {
        Serial.println("Erreur lors de la désérialisation du JSON !");
        return "error";
    }
}