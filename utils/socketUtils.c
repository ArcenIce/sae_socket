#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */

#define LG_MESSAGE 256

char serverGetMessage(int *dialog) {

    int lus;
    char messageRecu[LG_MESSAGE];
    memset(messageRecu, 0x00, LG_MESSAGE*sizeof(char));

    lus = read(*dialog, messageRecu, LG_MESSAGE*sizeof(char)); // ici appel bloquant
    switch(lus) {
        case -1 : /* une erreur ! */ 
            perror("read");
            exit(-5);
        case 0  : /* la socket est fermée */
            fprintf(stderr, "La socket a été fermée par le client !\n\n");
            return 0;
        default:  /* réception de n octets */
            printf("Message reçu : %s (%d octets)\n\n", messageRecu, lus);
    }
    return *messageRecu;

}

int serverSendMessage(int *dialog, char *message) {
    int ecrits;
	ecrits = write(*dialog, message, strlen(message)); 
	switch(ecrits){
		case -1 : /* une erreur ! */
			perror("write");
   			exit(-6);
		case 0 :  /* la socket est fermée */
			fprintf(stderr, "La socket a été fermée par le client !\n\n");
			return 0;
		default:  /* envoi de n octets */
   			printf("Message %s envoyé (%d octets)\n\n", message, ecrits);
			// On ferme la socket de dialogue et on se replace en attente ...
	}

}

char clientGetMessage(int *dialog) {

    int nb;
    char messageRecu[LG_MESSAGE];

	/* Reception des données du serveur */
	switch(nb = read(*dialog, messageRecu, LG_MESSAGE)) {
		case -1 : /* une erreur ! */
			perror("Erreur de lecture...");
			exit(-4);
		case 0 : /* la socket est fermée */
   		fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
			return 0;
		default: /* réception de n octets */
		  	messageRecu[nb]='\0';
			if (strcmp(messageRecu,"\n") != 0) {
				printf("Message reçu du serveur : %s (%d octets)\n\n", messageRecu, nb);
			}
	}	// On ferme la ressource avant de quitter

    return *messageRecu;

}

int clientSendMessage(int *dialog, char message[]) {

    int nb;

	switch(nb = write(*dialog, message, strlen(message))){
		case -1 : /* une erreur ! */
     		perror("Erreur en écriture...");
		    exit(-3);
		case 0 : /* la socket est fermée */
			fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
			return 0;
		default: /* envoi de n octets */
			printf("Message %s envoyé! (%d octets)\n\n", message, nb);
	}

}