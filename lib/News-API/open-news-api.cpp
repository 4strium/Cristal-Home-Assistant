#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <open-news-api.h>
#include <get_secret_values.h>

#define MAX_TITLES 5

String url_news = "https://newsapi.org/v2/top-headlines?country=fr&sortBy=popularity&apiKey=";

String init_http_news(void){

    String apiKey = get_secret(5);

    // Remove any trailing newline characters
    apiKey.trim();

    // Append the API key to the URL
    url_news += apiKey;

    // Utilisation de la bibliothèque HTTPClient pour effectuer une requête GET
    HTTPClient http;
    http.begin(url_news); // Début de la requête HTTP

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

    http.end();
}

String* get_five_top_news(void) {
    // Créer un tableau pour stocker les titres
    static String titles[MAX_TITLES];
    for (int i = 0; i < MAX_TITLES; i++) {
        titles[i] = ""; // Initialiser le tableau
    }

    String req_rep = init_http_news();
    
    if (req_rep == "") {
        Serial.println("Aucune réponse reçue !");
        return titles;
    }

    DynamicJsonDocument doc(2048); // Taille du document JSON, ajustez si nécessaire

    // Désérialiser le JSON
    DeserializationError error = deserializeJson(doc, req_rep);
    if (error) {
        Serial.println("Erreur lors de la désérialisation du JSON !");
        return titles;
    }

    // Accéder à la liste des articles
    JsonArray articles = doc["articles"].as<JsonArray>();

    // Extraire les titres des 5 premiers articles
    int count = 0;
    for (JsonObject article : articles) {
        if (count >= MAX_TITLES) break; // Limiter à 5 titres
        const char* title = article["title"];
        if (title) {
            titles[count] = String(title);
            count++;
        }
    }

    // Vérifier s'il y a des titres trouvés
    if (count == 0) {
        titles[0] = "Aucun titre trouvé !";
    }

    return titles;
}