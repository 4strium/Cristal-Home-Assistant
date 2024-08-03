#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>

#define PORT 8080
#define MAX_CONNECTIONS 5

bool is_file_empty(const std::string& filename){
    struct stat file_stat;
    if (stat(filename.c_str(), &file_stat) != 0){
        return true;
    }
    return file_stat.st_size == 0;
}

int main() {
    int serverSocket, recSocket, sendSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize = sizeof(clientAddr);
    char buffer[1024] = {0};

    // Création du socket serveur
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Erreur de création de socket" << std::endl;
        return -1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Lier le socket au port
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Échec de la liaison" << std::endl;
        close(serverSocket);
        return -1;
    }

    // Écouter les connexions entrantes
    if (listen(serverSocket, MAX_CONNECTIONS) < 0) {
        std::cerr << "Échec de l'écoute" << std::endl;
        close(serverSocket);
        return -1;
    }

    std::cout << "Serveur en écoute sur le port " << PORT << std::endl;

    while (true) {
        std::cout << "En attente de connexion..." << std::endl;
        recSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize);
        if (recSocket < 0) {
            std::cerr << "La connexion utile à la réception a échouée" << std::endl;
            continue;
        }

        std::cout << "Connexion acceptée" << std::endl;

        // Recevoir le fichier
        ssize_t bytesRead;
        std::ofstream outfile("enregistrement.wav", std::ios::binary);
        if (!outfile.is_open()) {
            std::cerr << "Erreur d'ouverture du fichier enregistrement.wav" << std::endl;
            close(recSocket);
            continue;
        }

        bool receivedData = false;
        while ((bytesRead = recv(recSocket, buffer, sizeof(buffer), 0)) > 0) {
            outfile.write(buffer, bytesRead);
            receivedData = true;
        }
        outfile.close();
        // Fermer le socket après la réception du fichier audio
        close(recSocket);

        if (!receivedData || is_file_empty("enregistrement.wav")) {
            std::cerr << "Fichier reçu est vide, aucun traitement effectué" << std::endl;
            continue;   // Passer à la prochaine connexion
        }

        std::cout << "Fichier reçu avec succès" << std::endl;

        // Exécuter le script Python
        std::cout << "Exécution du script Python..." << std::endl;
        int result = system("python3 recognize-fr.py");
        if (result != 0) {
            std::cerr << "Échec de l'exécution du script Python" << std::endl;
        } else {
            std::cout << "Script Python exécuté avec succès" << std::endl;
        }

        sendSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize);
        if (sendSocket < 0) {
            std::cerr << "La connexion utile à l'envoi a échouée" << std::endl;
            continue;   // Passer à la prochaine connexion
        }

        // Lire le contenu de rapport.txt
        std::ifstream reportFile("rapport.txt");
        if (!reportFile.is_open()) {
            std::cerr << "Échec de l'ouverture de rapport.txt" << std::endl;
            close(sendSocket); // Fermer le socket en cas d'échec
            continue; // Passer à la prochaine connexion
        }
        std::stringstream reportBuffer;
        reportBuffer << reportFile.rdbuf();
        std::string reportContent = reportBuffer.str();
        reportFile.close();

        // Envoyer le contenu de rapport.txt au client
        ssize_t sentBytes = send(sendSocket, reportContent.c_str(), reportContent.size(), 0);
        if (sentBytes < 0) {
            std::cerr << "Échec de l'envoi du rapport" << std::endl;
        } else {
            std::cout << "Rapport envoyé avec succès (" << sentBytes << " bytes)" << std::endl;
        }

        close(sendSocket);
    }

    // Fermer le socket serveur (en théorie, cette ligne ne sera jamais exécutée)
    close(serverSocket);
    return 0;
}
