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

	int socketDialogue1, socketDialogue2;
	struct sockaddr_in pointDeRencontreDistant;
	char messageRecu[LG_MESSAGE];
	char messageEnvoi[LG_MESSAGE];
	int retour;

	char mot[LG_MESSAGE];
	strcpy(mot, "TABULATION");
	char lettresMot[27];

	// Crée un socket de communication
	socketEcoute = socket(PF_INET, SOCK_STREAM, 0);
	setsockopt(socketEcoute, SOL_SOCKET, SO_REUSEADDR, 1, sizeof(1));
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

	// On fixe la taille de la file d’attente à 5 (pour les demandes de connexion non encore traitées)
	if(listen(socketEcoute, 5) < 0){
   		perror("listen");
   		exit(-3);
	}
	printf("Socket placée en écoute passive ...\n");

	// boucle d’attente de connexion : en théorie, un serveur attend indéfiniment !
	while(1){

		char motDevine1[sizeof(mot)];
		int erreurs1 = 0;
		char nberreurs1[2] = "0";
		init_game(&mot, lettresMot, motDevine1);

		char motDevine2[sizeof(mot)];
		int erreurs2 = 0;
		char nberreurs2[2] = "0";
		init_game(&mot, lettresMot, motDevine2);

		memset(messageRecu, 0x00, LG_MESSAGE*sizeof(char));
		printf("Attente d’une demande de connexion (quitter avec Ctrl-C)\n\n");

		// c’est un appel bloquant
		socketDialogue1 = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
		if (socketDialogue1 < 0) {
			perror("accept");
			close(socketDialogue1);
			close(socketEcoute);
			exit(-4);
		}

		serverSendMessage(&socketDialogue1, "Bienvenue, vous êtes le joueur 1, la partie commencera quand le deuxième joueur arrivera.\n");

		socketDialogue2 = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
		if (socketDialogue2 < 0) {
			perror("accept");
			close(socketDialogue2);
			close(socketEcoute);
			exit(-4);
		}

		serverSendMessage(&socketDialogue2, "Bienvenue, vous êtes le joueur 2, la partie commence maintenant.\n");

		printf("Les deux joeurs sont connectés, on peut commencer la partie.\n");

		int joueur = 2;
		char message[512];
		int fin = 0;

		*message = message_debut(&message, &mot, &motDevine1);
		serverSendMessage(&socketDialogue1, &message);

		*message = message_debut(&message, &mot, &motDevine2);
		serverSendMessage(&socketDialogue2, &message);

		sleep(1);

		while (fin == 0) {

			if (joueur == 2) {

				joueur = 1;

				serverSendMessage(&socketDialogue2, "C'est votre tour.\n");

				*messageRecu = serverGetMessage(&socketDialogue2);

				if (verif_lettre(messageRecu, lettresMot) == 1) {
					remplace_lettre(&messageRecu, &mot, motDevine2);
					printf("Mot actualisé :%s\n", motDevine2);
				} else {
					erreurs2++;
					sprintf(nberreurs2, "%d", erreurs2);
				}

				*message = message_actu(&message, &motDevine2, &nberreurs2);

				int verif = checkStat(mot, motDevine2, erreurs2);
				if (verif == 1) {
					char messageFin[512] = "Bravo vous avez trouvé ! Le mot était : ";
					strcat(messageFin,mot);
					serverSendMessage(&socketDialogue2, messageFin);
					serverSendMessage(&socketDialogue1, "Le joueur 2 a gagné, vous avez perdu\n");
					fin = 1;
				} 
				else if (verif == 2) {
					char messageFin[512] = "Dommage vous avez perdu... Le mot était : ";
					strcat(messageFin,mot);
					serverSendMessage(&socketDialogue2, messageFin);
					serverSendMessage(&socketDialogue1, "Le joueur 2 a perdu, vous avez gagné\n");
					fin = 1;
				} else {
					serverSendMessage(&socketDialogue2, message);
				}

			} else {

				joueur = 2;

				serverSendMessage(&socketDialogue1, "C'est votre tour.\n");

				*messageRecu = serverGetMessage(&socketDialogue1);

				if (verif_lettre(messageRecu, lettresMot) == 1) {
					remplace_lettre(&messageRecu, &mot, motDevine1);
					printf("Mot actualisé :%s\n", motDevine1);
				} else {
					erreurs1++;
					sprintf(nberreurs1, "%d", erreurs1);
				}

				*message = message_actu(&message, &motDevine1, &nberreurs1);
				serverSendMessage(&socketDialogue1, message);

				int verif = checkStat(mot, motDevine1, erreurs1);
				if (verif == 1) {
					char messageFin[512] = "Bravo vous avez trouvé ! Le mot était : ";
					strcat(messageFin,mot);
					serverSendMessage(&socketDialogue1, messageFin);
					serverSendMessage(&socketDialogue2, "Le joueur 1 a gagné, vous avez perdu\n");
					fin = 1;
				} 
				else if (verif == 2) {
					char messageFin[512] = "Dommage vous avez perdu... Le mot était : ";
					strcat(messageFin,mot);
					serverSendMessage(&socketDialogue1, messageFin);
					serverSendMessage(&socketDialogue2, "Le joueur 1 a perdu, vous avez gagné\n");
					fin = 1;
				} else {
					serverSendMessage(&socketDialogue1, "\n");
				}
			}

		}

		close(socketDialogue1);
		close(socketDialogue2);

	}
	// On ferme la ressource avant de quitter
   	close(socketEcoute);
	return 0;
}
