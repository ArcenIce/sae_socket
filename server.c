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

	int socketJoueur1;
	int socketJoueur2;
	struct sockaddr_in pointDeRencontreDistant;
	char messageRecu[LG_MESSAGE];
	char messageJ1[LG_MESSAGE];
	char messageJ2[LG_MESSAGE];
	int retour;

	char mot[LG_MESSAGE];
	strcpy(mot, "TABULATION");
	char lettresMot[27];
	char motDevine[sizeof(mot)];
	int erreurs = 0;
	char nberreurs[2] = "0";
	
	init_game(&mot, lettresMot, motDevine);

	// Crée un socket de communication
	socketEcoute = socket(PF_INET, SOCK_STREAM, 0);
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
	if(listen(socketEcoute, 2) < 0){
   		perror("listen");
   		exit(-3);
	}
	printf("Socket placée en écoute passive ...\n");

	// boucle d’attente de connexion : en théorie, un serveur attend indéfiniment !
	while(1){
		erreurs = 0;
		nberreurs[2] = "0";
		init_game(&mot, lettresMot, motDevine);
		memset(messageRecu, 0x00, LG_MESSAGE*sizeof(char));
		printf("Attente des demandes de connexion (quitter avec Ctrl-C)\n\n");

		// c’est un appel bloquant
		socketJoueur1 = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
		if (socketJoueur1 < 0) {
   			perror("accept");
			close(socketJoueur1);
   			close(socketEcoute);
   			exit(-4);
		}
		serverSendMessage(&socketJoueur1, "1");
		socketJoueur2 = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
		if (socketJoueur2 < 0)
		{
			perror("accept");
			close(socketJoueur2);
			close(socketEcoute);
			exit(-4);
		}
		serverSendMessage(&socketJoueur2, "2");
		serverSendMessage(&socketJoueur1, "2");

		int fin = 0;

		serverGetMessage(&socketJoueur1, messageJ2);
		printf("s : %s\n", &messageJ2);

		serverSendMessage(&socketJoueur2, &messageJ2);

		while (fin == 0) {
    		serverGetMessage(&socketJoueur1, messageRecu);

			if (verif_lettre(messageRecu, lettresMot) == 1) {
				remplace_lettre(&messageRecu, &mot, motDevine);
				printf("Mot actualisé :%s\n", motDevine);
			} else {
				erreurs++;
				// printf("%d", erreurs);
				sprintf(nberreurs, "%d", erreurs);
			}

			int verif = checkStat(mot, motDevine, erreurs);
			if (verif == 1) {
				char messageFin[512] = "Bravo vous avez trouvé ! Le mot était : ";
				strcat(messageFin,mot);
				serverSendMessage(&socketJoueur1, messageFin);
				fin = 1;
			} 
			else if (verif == 2) {
				char messageFin[512] = "Dommage vous avez perdu... Le mot était : ";
				strcat(messageFin,mot);
				serverSendMessage(&socketJoueur1, messageFin);
				fin = 1;
			}
			else{
				serverSendMessage(&socketJoueur1, messageJ1);
			}

			// On envoie des données vers le client (cf. protocole)
			// serverSendMessage(&socketJoueur1, messageRecu);
		}
		close(socketJoueur1);
	}
	// On ferme la ressource avant de quitter
   	close(socketEcoute);
	return 0;
}
