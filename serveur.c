#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct {
    float cash;
    int stock;
} Portefeuille;

// Le Broker commence avec beaucoup d'actions et un budget fixe 
Portefeuille broker = {10000.0, 100}; 
float prix_unitaire = 50.0;
pthread_mutex_t verrou = PTHREAD_MUTEX_INITIALIZER; // Pour éviter que deux clients achètent la même action en même temps

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
    int quantite;
    
    write_log("Nouveau client connecté.");

    // Boucle de communication avec ce client
    while(read(sock, buffer, 1024) > 0) {
        if (strncmp(buffer, "ACHAT", 5) == 0) {
            sscanf(buffer, "ACHAT %d", &quantite);
            float total = quantite * prix_unitaire;

            pthread_mutex_lock(&verrou);
            if (broker.stock >= quantite) {
                broker.stock -= quantite;
                broker.cash += total;
                send(sock, "ACHAT_OK", 8, 0);
                write_log("Transaction : Vente au client réussie.");
            } else {
                send(sock, "ERREUR_STOCK_BROKER", 19, 0);
                write_log("Erreur : Stock broker insuffisant.");
            }
            pthread_mutex_unlock(&verrou);
        } 
        else if (strncmp(buffer, "VENTE", 5) == 0) {
            sscanf(buffer, "VENTE %d", &quantite);
            float total = quantite * prix_unitaire;

            pthread_mutex_lock(&verrou);
            if (broker.cash >= total) {
                broker.stock += quantite;
                broker.cash -= total;
                send(sock, "VENTE_OK", 8, 0);
                write_log("Transaction : Achat au client réussi.");
            } else {
                send(sock, "ERREUR_CASH_BROKER", 18, 0);
                write_log("Erreur : Fonds broker insuffisants.");
            }
            pthread_mutex_unlock(&verrou);
        }
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