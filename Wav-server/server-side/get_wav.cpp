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

int main() {
    int serverSocket, newSocket;
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
        newSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize);
        if (newSocket < 0) {
            std::cerr << "Accepter la connexion a échoué" << std::endl;
            continue;
        }

        // Recevoir le fichier
        ssize_t bytesRead;
        std::ofstream outfile("enregistrement.wav", std::ios::binary);
        while ((bytesRead = recv(newSocket, buffer, sizeof(buffer), 0)) > 0) {
            outfile.write(buffer, bytesRead);
        }
        outfile.close();
        std::cout << "Fichier reçu avec succès" << std::endl;

        // Exécuter le script Python
        int result = system("python3 recognize-fr.py");
        if (result != 0) {
            std::cerr << "Échec de l'exécution du script Python" << std::endl;
        }

        // Lire le contenu de rapport.txt
        std::ifstream reportFile("rapport.txt");
        if (!reportFile) {
            std::cerr << "Échec de l'ouverture de rapport.txt" << std::endl;
            close(newSocket); // Fermer le socket en cas d'échec
            continue; // Passer à la prochaine connexion
        }
        std::stringstream reportBuffer;
        reportBuffer << reportFile.rdbuf();
        std::string reportContent = reportBuffer.str();
        reportFile.close();

        // Envoyer le contenu de rapport.txt au client
        send(newSocket, reportContent.c_str(), reportContent.size(), 0);
        std::cout << "Rapport envoyé avec succès" << std::endl;

        // Fermer le socket après l'envoi du rapport
        close(newSocket);
    }

    // Fermer le socket serveur (en théorie, cette ligne ne sera jamais exécutée)
    close(serverSocket);
    return 0;
}
