#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */
#include <ctype.h>
#include <string.h>

#include "utils/socketUtils.c"
#include "utils/gameUtils.c"

#define LG_MESSAGE 256

int main(int argc, char *argv[]){
	
	int descripteurSocket;
	struct sockaddr_in sockaddrDistant;
	socklen_t longueurAdresse;

	char messageEnvoi[LG_MESSAGE]; // buffer stockant le message à envoyer
	char messageRecu[LG_MESSAGE]; // buffer stockant le message reçur
	int nb; // nb d’octets écrits et lus

	char ip_dest[16];
	int  port_dest;

	// Pour pouvoir contacter le serveur, le client doit connaître son adresse IP et le port de comunication
	// Ces 2 informations sont passées sur la ligne de commande
	// Si le serveur et le client tournent sur la même machine alors l'IP locale fonctionne : 127.0.0.1
	// Le port d'écoute du serveur est 5000 dans cet exemple, donc en local utiliser la commande :
	// ./client_base_tcp 127.0.0.1 5000 heure
	if (argc>1) { // si il y a au moins 3 arguments passés en ligne de commande, récupération ip, port et message
		strncpy(ip_dest,argv[1],16);
		sscanf(argv[2],"%d",&port_dest);
	}else{
		printf("USAGE : %s ip port message\n",argv[0]);
		exit(-1);
	}


	// Crée un socket de communication
	descripteurSocket = socket(AF_INET, SOCK_STREAM, 0);
	// Teste la valeur renvoyée par l’appel système socket()
	if(descripteurSocket < 0){
		perror("Erreur en création de la socket..."); // Affiche le message d’erreur
		exit(-1); // On sort en indiquant un code erreur
	}
	printf("Socket créée! (%d)\n", descripteurSocket);


	// Remplissage de sockaddrDistant (structure sockaddr_in identifiant la machine distante)
	// Obtient la longueur en octets de la structure sockaddr_in
	longueurAdresse = sizeof(sockaddrDistant);
	// Initialise à 0 la structure sockaddr_in
	// memset sert à faire une copie d'un octet n fois à partir d'une adresse mémoire donnée
	// ici l'octet 0 est recopié longueurAdresse fois à partir de l'adresse &sockaddrDistant
	memset(&sockaddrDistant, 0x00, longueurAdresse);
	// Renseigne la structure sockaddr_in avec les informations du serveur distant
	sockaddrDistant.sin_family = AF_INET;
	// On choisit le numéro de port d’écoute du serveur
	sockaddrDistant.sin_port = htons(port_dest);
	// On choisit l’adresse IPv4 du serveur
	inet_aton(ip_dest, &sockaddrDistant.sin_addr);

	// Débute la connexion vers le processus serveur distant
	if((connect(descripteurSocket, (struct sockaddr *)&sockaddrDistant,longueurAdresse)) == -1){
		perror("Erreur de connection avec le serveur distant...");
		close(descripteurSocket);
		exit(-2); // On sort en indiquant un code erreur
	}
	printf("Connexion au serveur %s:%d réussie!\n\n",ip_dest,port_dest);
	


 	// Envoi du message
	// clientSendMessage(&descripteurSocket, messageEnvoi);

	char player[LG_MESSAGE];
	// réception du numéro du joueur
	clientGetMessage(&descripteurSocket, player);
	printf("Joueur : %s\n", player);

	// jeu client pour Joueur 1
	if (strcmp(player, "J1") == 0){
		const char prompt[] = "En attente du joueur 2";

		// Return and clear with spaces, then return and print prompt.
		printf("%s\n", prompt);

		char mot[150];
		// 1er message reçu server
		clientGetMessage(&descripteurSocket, messageRecu);
		// demande du mot
		printf("\nEntrez un mot à faire deviner :\n");
		scanf("%s", mot);
		// passage du mot en majuscule
		for (int i = 0; i<sizeof(mot); i++){
			mot[i] = toupper(mot[i]);
		}
		printf("Le mot a deviner est : %s\n\n", mot);

		// START PARTIE
		char lettresMot[27];
		char motDevine[sizeof(mot)];
		int erreurs = 0;
		char nberreurs[2] = "0";
		// initialisation du jeu avec une liste des lettres dans le mot et le mot changé en _
		init_game(mot, lettresMot, motDevine);
		// envoi du mot au server
		clientSendMessage(&descripteurSocket, motDevine);
		int fin = 0;
		char lettre[LG_MESSAGE];

		// PARTIE
		while (fin == 0)
		{
			// réception message server
			clientGetMessage(&descripteurSocket, lettre);
			printf("\nLe Joueur 2 a envoyé la lettre %s\n", lettre);
			// check si la lettre fait parti du mot
			if (verif_lettre(lettre, lettresMot) == 1){
				// afficahage de la lettre dans le mot
				remplace_lettre(lettre, mot, motDevine);
			}
			else{
				erreurs++;
				// affichage des erreurs
				sprintf(nberreurs, "%d", erreurs);
			}
			// vérification de l'état de la partie
			int stat = checkStat(mot, motDevine, erreurs);
			// si partie non finie
			if (stat == 0){
				message_actu(messageEnvoi, motDevine, nberreurs);
				printf("Actualisation du message : %s\n", messageEnvoi);
				clientSendMessage(&descripteurSocket, messageEnvoi);
			}
			// partie gagnée
			else if (stat == 1){
				printf("Le Joueur 2 a trouvé le mot !\n");
				sprintf(messageEnvoi, "Fin de la partie !\nVous avez trouvé le mot %s !", mot);
				clientSendMessage(&descripteurSocket, messageEnvoi);
				fin = 1;
			}
			// partie perdue
			else if (stat == 2){
				printf("Le Joueur 2 a perdu !\n");
				sprintf(messageEnvoi, "Fin de la partie !\nVous n'avez pas trouvé le mot %s !", mot);
				clientSendMessage(&descripteurSocket, messageEnvoi);
				fin = 1;
			}
		}
	}

	// jeu client pour Joueur 2
	else if (strcmp(player, "J2") == 0){
		printf("En attente du mot du Joueur 1\n");
		// attente réception du mot
		clientGetMessage(&descripteurSocket, messageRecu);
		printf("Le mot à deviner est : %s\n", messageRecu);
		int fin = 0;
		char lettre[LG_MESSAGE];
		int start = 1;
		// Partie du joueur 2
		while (fin == 0){
			// on évite d'afficher le message pour le 1er coup
			if (start != 1){
				printf("Mot actualisé : %s\n\n", messageRecu);
			}
			else{
				start = 0;
				printf("\n");
			}
			// demande de lettre
			printf("Entrez un lettre :\n");
			scanf("%s", lettre);
			// récupération de la 1ère lettre seulement et en majuscule
			*lettre = toupper(lettre[0]);
			// envoi de la lettre
			clientSendMessage(&descripteurSocket, lettre);
			// réception du lot actualisé
			clientGetMessage(&descripteurSocket, messageRecu);
			// Fin de partie
			if (strstr(messageRecu, "Fin de la partie") != NULL)
			{
				// affichage du message de fin de partie
				printf("%s\n", messageRecu);
				fin = 1;
			}
		}
	}
	return 0;
}