#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <open-news-api.h>
#include <SPIFFS.h>
#include <U8g2lib.h>

#define MAX_TITLES 5
#define DISPLAY_TIME 5000  // Temps d'affichage de chaque titre en millisecondes

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R2, /* reset=*/ U8X8_PIN_NONE);

String url = "https://newsapi.org/v2/top-headlines?country=fr&sortBy=popularity&apiKey=";

String init_http_news(void){

    // Read the API key from the file
    File file = SPIFFS.open("/api_key.txt", "r");
    if (!file) {
        Serial.println("Failed to open file");
        return;
    }

    String apiKey = file.readString();
    file.close();

    // Remove any trailing newline characters
    apiKey.trim();

    // Append the API key to the URL
    url += apiKey;

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

void display_titles(void) {
    // Obtenir le tableau de titres
    String* titles = get_five_top_news();

    // Initialiser l'écran OLED
    u8g2.begin();
    u8g2.setFont(u8g2_font_ncenB08_tr);

    for (int i = 0; i < MAX_TITLES; i++) {
        u8g2.clearBuffer(); // Effacer le tampon de l'écran
        u8g2.setCursor(0, 10); // Position du curseur pour le texte
        u8g2.print("Titre ");
        u8g2.print(i + 1);
        u8g2.print(": ");
        u8g2.print(titles[i]);
        u8g2.sendBuffer(); // Envoyer le tampon à l'écran
        
        delay(DISPLAY_TIME); // Attendre avant d'afficher le titre suivant
    }
}