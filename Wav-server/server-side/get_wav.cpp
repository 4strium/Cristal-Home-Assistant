#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

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
        return -1;
    }

    // Écouter les connexions entrantes
    if (listen(serverSocket, MAX_CONNECTIONS) < 0) {
        std::cerr << "Échec de l'écoute" << std::endl;
        return -1;
    }

    // Accepter la connexion entrante
    if ((newSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize)) < 0) {
        std::cerr << "Accepter la connexion a échoué" << std::endl;
        return -1;
    }

    // Recevoir le fichier
    ssize_t bytesRead;
    std::ofstream outfile("enregistrement.wav", std::ios::binary);
    while ((bytesRead = recv(newSocket, buffer, sizeof(buffer), 0)) > 0) {
        outfile.write(buffer, bytesRead);
    }
    std::cout << "Fichier reçu avec succès" << std::endl;

    // Fermer les sockets
    close(newSocket);
    close(serverSocket);
    return 0;
}