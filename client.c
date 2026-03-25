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
    int choix;
    char produit[50];
    int quantite;

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
    printf("\n--- MENU ---\n");
    printf("1. Voir produits\n");
    printf("2. Acheter\n");
    printf("3. Vendre\n");
    printf("4. Quitter\n");
    printf("Choix: ");
    
    scanf("%d", &choix);
    getchar(); // pour enlever le \n

    if(choix == 4) break;
   memset(message, 0, 1024);
    switch(choix) {
        case 1:
            strcpy(message, "INFO");
            break;
        case 2: 
            

            printf("Produit: ");
            scanf("%s", produit);

            printf("Quantité: ");
            scanf("%d", &quantite);
            getchar();

            sprintf(message, "ACHAT %s %d", produit, quantite);
            break;
        

        case 3: {
             printf("Produit: ");
            scanf("%s", produit);

            printf("Quantité: ");
            scanf("%d", &quantite);
            getchar();

            sprintf(message, "VENTE %s %d", produit, quantite);
            break;

            }
        default:
            printf("Choix invalide\n");
            continue;
    }

    
   printf("Commande envoyée: %s\n", message);
   send(sock, message, strlen(message), 0);
   int valread = read(sock, buffer, 1024);
    if(valread <= 0) {
        printf("Connexion perdue avec le serveur.\n");
        break;
    }
    printf("Réponse Broker: %s\n", buffer);

    memset(buffer, 0, 1024);
}

    close(sock);
    return 0;
}