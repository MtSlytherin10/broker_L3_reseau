#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

typedef struct {
    float cash;
    int stock;
} Portefeuille;

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char message[1024], buffer[1024];
    int choix, quantite;
    char produit[50];
    
    // Initialisation du portefeuille client
    Portefeuille mon_p = {1000.0, 0}; 
    float prix_actuel = 50.0; // Valeur par défaut avant INFO

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
        printf("\n[MON PORTFEUILLE] Cash: %.2f | Actions: %d\n", mon_p.cash, mon_p.stock);
        printf("1. Voir prix et stock Broker (INFO)\n");
        printf("2. Acheter des actions\n");
        printf("3. Vendre des actions\n");
        printf("4. Quitter\n");
        printf("Choix: ");
        
        if (scanf("%d", &choix) != 1) break;
        getchar(); 

        if(choix == 4) break;
        memset(message, 0, 1024);

        switch(choix) {
            case 1:
                strcpy(message, "INFO");
                break;
            case 2: 
                printf("Nom du produit: ");
                scanf("%s", produit);
                printf("Quantité à acheter: ");
                scanf("%d", &quantite);
                
                if (mon_p.cash < (quantite * prix_actuel)) {
                    printf("Erreur : Fonds personnels insuffisants !\n");
                    continue;
                }
                sprintf(message, "ACHAT %s %d", produit, quantite);
                break;
            case 3:
                printf("Nom du produit: ");
                scanf("%s", produit);
                printf("Quantité à vendre: ");
                scanf("%d", &quantite);

                if (mon_p.stock < quantite) {
                    printf("Erreur : Vous n'avez pas assez d'actions !\n");
                    continue;
                }
                sprintf(message, "VENTE %s %d", produit, quantite);
                break;
            default:
                continue;
        }

        send(sock, message, strlen(message), 0);
        memset(buffer, 0, 1024);
        int valread = read(sock, buffer, 1024);
        
        if(valread <= 0) {
            printf("Connexion perdue.\n");
            break;
        }

        printf("Réponse Broker: %s\n", buffer);

        // Mise à jour locale du portefeuille si succès
        if (strcmp(buffer, "ACHAT_OK") == 0) {
            mon_p.cash -= (quantite * prix_actuel);
            mon_p.stock += quantite;
        } else if (strcmp(buffer, "VENTE_OK") == 0) {
            mon_p.cash += (quantite * prix_actuel);
            mon_p.stock -= quantite;
        } else if (strncmp(buffer, "PRIX", 4) == 0) {
            sscanf(buffer, "PRIX %f", &prix_actuel);
        }
    }

    close(sock);
    return 0;
}