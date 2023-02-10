#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */
#include <string.h>

#include "utils/socketUtils.c"
#include "utils/gameUtils.c"

#define PORT IPPORT_USERRESERVED // = 5000 (ports >= 5000 réservés pour usage explicite)

#define LG_MESSAGE 256

//157.90.140.167

int main(int argc, char *argv[]){
	int socketEcoute;

	struct sockaddr_in pointDeRencontreLocal;
	socklen_t longueurAdresse;

	// Sockets de connexion aux joueurs
	int socketJoueur1;
	int socketJoueur2;
	struct sockaddr_in pointDeRencontreDistant;

	// Messages utilisés pour transférer les
	char messageRecu[LG_MESSAGE];
	char messageJ1[LG_MESSAGE];
	char messageJ2[LG_MESSAGE];

	// Variable pour le problème "address already used"
	int option = 1;
	// Crée un socket de communication
	socketEcoute = socket(PF_INET, SOCK_STREAM, 0);
	// Permet de régler le problème "address already used" qui fait perdre du temps pour relancer le server
	setsockopt(socketEcoute, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	
	// Teste la valeur renvoyée par l’appel système socket()
	if(socketEcoute < 0){
		perror("socket"); // Affiche le message d’erreur
		exit(-1); // On sort en indiquant un code erreur
	}
	printf("Socket créée avec succès ! (%d)\n", socketEcoute); // On prépare l’adresse d’attachement locale


	// Remplissage de sockaddrDistant (structure sockaddr_in identifiant le point d'écoute local)
	longueurAdresse = sizeof(pointDeRencontreLocal);

	memset(&pointDeRencontreLocal, 0x00, longueurAdresse); pointDeRencontreLocal.sin_family = PF_INET;
	pointDeRencontreLocal.sin_addr.s_addr = htonl(INADDR_ANY); // attaché à toutes les interfaces locales disponibles
	pointDeRencontreLocal.sin_port = htons(5050); // = 5000 ou plus



	// On demande l’attachement local de la socket
	if((bind(socketEcoute, (struct sockaddr *)&pointDeRencontreLocal, longueurAdresse)) < 0) {
		perror("bind");
		exit(-2);
	}
	printf("Socket attachée avec succès !\n");

	// On fixe la taille de la file d’attente à 2 (pour les demandes de connexion non encore traitées)
	if(listen(socketEcoute, 2) < 0){
   		perror("listen");
   		exit(-3);
	}
	printf("Socket placée en écoute passive ...\n");

	int tourne = 1;

	// boucle d’attente de connexion : en théorie, un serveur attend indéfiniment !
	while(tourne == 1){
		memset(messageRecu, 0x00, LG_MESSAGE*sizeof(char));
		printf("Attente des demandes de connexion (quitter avec Ctrl-C)\n\n");

		// c’est un appel bloquant
		// Attente du joueur 1
		socketJoueur1 = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
		if (socketJoueur1 < 0) {
   			perror("accept");
			close(socketJoueur1);
   			close(socketEcoute);
   			exit(-4);
		}
		// Une fois connecté on envoie J1 au joueur 1 pour qu'il affiche le message d'attente
		serverSendMessage(&socketJoueur1, "J1");
		// Attente du joueur 2
		socketJoueur2 = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
		if (socketJoueur2 < 0)
		{
			perror("accept");
			close(socketJoueur2);
			close(socketEcoute);
			exit(-4);
		}
		// Une fois connecté on envoie J2 aux deux joueurs pour pouvoir lancer la partie
		serverSendMessage(&socketJoueur2, "J2");
		serverSendMessage(&socketJoueur1, "J2");
		// Une fois qu'on a trouvé les 2 joueurs on fait un fork pour diviser le processus
		// et relancer une autre partie qui sera en attente de connexion.
		if(fork()){


			// Variable qui sert pour finir la partie
			int fin = 0;

			// Initialisation de la partie

			// On attend le mot à deviner du joueur 1 (ex: ______ pour COUCOU)
			serverGetMessage(&socketJoueur1, messageJ2);
			// On envoie ce mot au joueur 2
			serverSendMessage(&socketJoueur2, messageJ2);

			// Boucle de jeu
			while (fin == 0) {
				// On attend la lettre du joueur 2
				serverGetMessage(&socketJoueur2, messageJ1);
				// On l'envoie au joueur 1
				serverSendMessage(&socketJoueur1, messageJ1);

				// On attend l'actualisation de la partie par le joueur 1
				serverGetMessage(&socketJoueur1, messageJ2);
				// On l'envoie au joueur 2
				serverSendMessage(&socketJoueur2, messageJ2);
				// On vérifie si la partie est finie ou pas
				// En regardant si le message du joueur 1 contient "Fin de la partie"
				if (strstr(messageJ2,"Fin de la partie !") != NULL){
					fin = 1;
				}
			}
			// On ferme les sockets de liaison avec les clients.
			close(socketJoueur1);
			close(socketJoueur2);
			// Dans cette version on ferme le socket d'écoute à la fin de la partie car
			// le fork lance d'autres parties en attente pas besoin de relancer la partie actuelle.
			close(socketEcoute);
			tourne = 0;
			return 0;
		}
	}
}