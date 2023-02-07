#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */

#define PORT IPPORT_USERRESERVED // = 5000 (ports >= 5000 réservés pour usage explicite)

#define LG_MESSAGE 256

void lire_heure(char* heure){
	FILE *fpipe;
	
	fpipe = popen("date '+%X'","r");
	if (fpipe == NULL){
    	perror("popen" );
    	exit(-1);
  	}
	fgets(heure, LG_MESSAGE, fpipe);
	pclose(fpipe);
}

void lire_date(char* date){
	FILE *fpipe;
	
	fpipe = popen("date '+%A %d %B %Y'","r");
	if (fpipe == NULL){
    	perror("popen" );
    	exit(-1);
  	}
	fgets(date, LG_MESSAGE, fpipe);
	pclose(fpipe);
}

void envoyer_mot(char* mot){
	printf(mot);
	fgets(mot, LG_MESSAGE, NULL);
}

int main(int argc, char *argv[]){
	int socketEcoute;

	struct sockaddr_in pointDeRencontreLocal;
	socklen_t longueurAdresse;

	char mot[4] = "test";
	int socketDialogue;
	struct sockaddr_in pointDeRencontreDistant;
	char messageRecu[LG_MESSAGE];
	char messageEnvoi[LG_MESSAGE];
	int ecrits, lus; /* nb d’octets ecrits et lus */
	int retour;

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
	if(listen(socketEcoute, 5) < 0){
   		perror("listen");
   		exit(-3);
	}
	printf("Socket placée en écoute passive ...\n");
	
	// boucle d’attente de connexion : en théorie, un serveur attend indéfiniment ! 
	while(1){
		memset(messageRecu, 0x00, LG_MESSAGE*sizeof(char));
		printf("Attente d’une demande de connexion (quitter avec Ctrl-C)\n\n");
		
		// c’est un appel bloquant
		socketDialogue = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
		if (socketDialogue < 0) {
   			perror("accept");
			close(socketDialogue);
   			close(socketEcoute);
   			exit(-4);
		}
		
		// On réception les données du client (cf. protocole)
		lus = read(socketDialogue, messageRecu, LG_MESSAGE*sizeof(char)); // ici appel bloquant
		switch(lus) {
			case -1 : /* une erreur ! */ 
				  perror("read"); 
				  close(socketDialogue); 
				  exit(-5);
			case 0  : /* la socket est fermée */
				  fprintf(stderr, "La socket a été fermée par le client !\n\n");
   				  close(socketDialogue);
   				  return 0;
			default:  /* réception de n octets */
				  printf("Message reçu : %s (%d octets)\n\n", messageRecu, lus);
		}

		// Déterminer le message à envoyer en fonction de la demande du client
		if(strcmp(messageRecu,"heure")==0)
			lire_heure(messageEnvoi);
		else if(strcmp(messageRecu,"date")==0)
			lire_date(messageEnvoi);
		else if(strcmp(messageRecu,"mot")==0)
			envoyer_mot(mot);
		else
			sprintf(messageEnvoi, "Commande non reconnue");

		// On envoie des données vers le client (cf. protocole)
		ecrits = write(socketDialogue, messageEnvoi, strlen(messageEnvoi)); 
		switch(ecrits){
			case -1 : /* une erreur ! */
				  perror("write");
   				  close(socketDialogue);
   				  exit(-6);
			case 0 :  /* la socket est fermée */
				  fprintf(stderr, "La socket a été fermée par le client !\n\n");
				  close(socketDialogue);
				  return 0;
			default:  /* envoi de n octets */
   				  printf("Message %s envoyé (%d octets)\n\n", messageEnvoi, ecrits);
				  // On ferme la socket de dialogue et on se replace en attente ...
   				  close(socketDialogue);
		}
	}
	// On ferme la ressource avant de quitter
   	close(socketEcoute);
	return 0; 
}