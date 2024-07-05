#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main() {
    // Adresse IP et port du serveur distant
    std::string serverIP = "178.128.174.115";
    int serverPort = 8080;

    // Chemin du fichier .wav à envoyer
    std::string filePath = "input.wav";

    // Ouverture du fichier .wav en mode binaire
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "Erreur lors de l'ouverture du fichier." << std::endl;
        return 1;
    }

    // Création du socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Erreur lors de la création du socket." << std::endl;
        return 1;
    }

    // Configuration de l'adresse du serveur
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverIP.c_str(), &(serverAddress.sin_addr)) <= 0) {
        std::cerr << "Adresse IP invalide." << std::endl;
        return 1;
    }

    // Connexion au serveur
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Erreur lors de la connexion au serveur." << std::endl;
        return 1;
    }

    // Lecture et envoi du fichier .wav
    char buffer[BUFFER_SIZE];
    while (!file.eof()) {
        file.read(buffer, BUFFER_SIZE);
        int bytesRead = file.gcount();
        if (send(clientSocket, buffer, bytesRead, 0) != bytesRead) {
            std::cerr << "Erreur lors de l'envoi du fichier." << std::endl;
            return 1;
        }
    }

    // Fermeture du socket et du fichier
    close(clientSocket);
    file.close();

    std::cout << "Le fichier a été envoyé avec succès." << std::endl;

    return 0;
}