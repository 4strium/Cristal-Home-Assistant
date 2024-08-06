#include <SPI.h>
#include <SD.h>

const int maxLignes = 6; // Nombre maximum de lignes à lire
const int tailleMaxLigne = 130; // Taille maximale de chaque ligne

char* get_secret(int indice) {

  if (!SD.begin()) {
    Serial.println("Erreur d'initialisation de la carte SD!");
    return nullptr;
  }
  Serial.println("Carte SD initialisée.");

  File myFile = SD.open("/secret_file.txt");
  char* lignes[maxLignes]; // Tableau de pointeurs pour stocker les lignes

  if (myFile) {
    Serial.println("Lecture du fichier secret_file.txt...");

    int ligneIndex = 0;
    while (ligneIndex < maxLignes && myFile.available()) {
      char buffer[tailleMaxLigne]; // Tampon pour lire une ligne
      int i = 0;

      // Lire la ligne jusqu'à un retour à la ligne ou jusqu'à la fin du tampon
      while (myFile.available() && i < tailleMaxLigne - 1) {
        char c = myFile.read();
        if (c == '\n') {
          break;
        }
        buffer[i++] = c;
      }
      buffer[i] = '\0'; // Terminer la chaîne

      // Nettoyer les espaces blancs et retours chariot en début et en fin de chaîne
      char* debut = buffer;
      while (isspace((unsigned char)*debut)) debut++; // Supprimer les espaces et retours chariot en début

      char* fin = debut + strlen(debut) - 1;
      while (fin > debut && (isspace((unsigned char)*fin) || *fin == '\r')) fin--; // Supprimer les espaces et retours chariot en fin
      *(fin + 1) = '\0'; // Terminer la chaîne

      // Allouer la mémoire pour la ligne et copier le contenu du tampon
      lignes[ligneIndex] = (char*)malloc(strlen(buffer) + 1);
      if (lignes[ligneIndex]) {
        strcpy(lignes[ligneIndex], buffer);
        ligneIndex++;
      }
    }

    myFile.close();
  } else {
    Serial.println("Impossible d'ouvrir le fichier secret_file.txt");
  }

  return lignes[indice];
}

