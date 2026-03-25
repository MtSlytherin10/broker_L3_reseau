#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char message[1024];
    char buffer[1024] = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Erreur de connexion\n");
        return -1;
    }

    printf("--- Bienvenue chez le Broker ---\n"); 

    while(1) {
        printf("Entrez votre demande (ex: ACHAT 10 APPLE) ou 'exit': ");
        fgets(message, 1024, stdin);
        
        if(strncmp(message, "exit", 4) == 0) break;

        send(sock, message, strlen(message), 0); 
        read(sock, buffer, 1024); 
        printf("Réponse Broker: %s\n", buffer);
        
        memset(buffer, 0, 1024);
    }

    close(sock);
    return 0;
}