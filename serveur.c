#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Fonction pour écrire dans le fichier log 
void write_log(char *message) {
    FILE *f = fopen("broker.log", "a");
    if (f == NULL) return;
    fprintf(f, "%s\n", message);
    fclose(f);
    printf("LOG: %s\n", message); // Affiche aussi sur le terminal 
}

// Fonction exécutée pour chaque nouveau client 
void *handle_client(void *socket_desc) {
    int sock = *(int*)socket_desc;
    char buffer[1024] = {0};
    
    write_log("Nouveau client connecté.");

    // Boucle de communication avec ce client
    while(read(sock, buffer, 1024) > 0) {
        write_log("Action client reçue.");
        
        // Simulation de réponse du Broker 
        char *reply = "Ordre reçu par le Broker";
        send(sock, reply, strlen(reply), 0);
        
        memset(buffer, 0, 1024);
    }

    write_log("Client déconnecté."); 
    close(sock);
    free(socket_desc);
    return NULL;
}

int main() {
    int server_fd, new_socket, *new_sock;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 10);

    write_log("Serveur Broker démarré sur le port 8080..."); 

    while((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))) {
        pthread_t client_thread;
        new_sock = malloc(1);
        *new_sock = new_socket;

        // Création d'un thread pour gérer le client en parallèle 
        if(pthread_create(&client_thread, NULL, handle_client, (void*)new_sock) < 0) {
            perror("Erreur création thread");
            return 1;
        }
    }
    return 0;
}