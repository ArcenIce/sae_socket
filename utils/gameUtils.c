#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>     /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h>  /* pour htons et inet_aton */
#include <string.h>

#define ALPHABET "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

int verif_lettre(char *lettreDonne, char *lettresMot) { //Lettre dans le mot ?
    for (int i = 0; i < strlen(lettresMot); i++) {
        if (lettresMot[i] == *lettreDonne) {
            return 1;
        }
    }
    return 0;
}

void remplace_lettre(char *lettreDonnee, char *mot, char *motDevine) { //Remplace les tirets
    for (int i = 0; i < strlen(mot); i++) {
        if (*lettreDonnee == mot[i]) {
            motDevine[i] = *lettreDonnee;
        }
    }
}

void init_game(char *mot, char *lettreMot, char *motDevine) { //Initialise le jeu
    strcpy(motDevine, "");
    for (int i = 0; i < strlen(mot); i++) {
        if (strchr(lettreMot, mot[i]) == 0) {
            strcat(lettreMot, &mot[i]);
        }
        strcat(motDevine, "_");
    }
}

int verif_erreurs(int *erreurs) { //Check si perdu
    if(*erreurs == 6) {
        return 1;
    }
    return 0;
}

int checkStat(char *mot, char *motDevine, int erreurs) { //Check si gagné ou perdu ou rien
    if (erreurs == 6) {
        return 2;
    }
    else if ((strcmp(mot, motDevine) == 0)) {
        return 1;
    }
    return 0;
}

char message_debut(char *messageModifiable, char *mot, char *motDevine) { //Fonction pour générer la chaine de caractères du message de début

    strcpy(messageModifiable, "");
	char messageNblettres[150] = "Le mot fait \033[1m"; char nblettres[128]; char messageSuiteNblettres[34] = "\033[0m lettres de long, devinez le !";
	char messageForme[64] = "\nVoici la forme du mot: ";
	sprintf(nblettres, "%zu", strlen(mot));
	strcat(messageModifiable, messageNblettres); strcat(messageModifiable, nblettres); strcat(messageModifiable,messageSuiteNblettres);
	strcat(messageModifiable, messageForme); strcat(messageModifiable, motDevine);

    return *messageModifiable;
}

char message_actu(char *messageModifiable, char *motDevine, char *nberreurs) { //Fonction pour générer la chaine de caractères du message de début

    strcpy(messageModifiable, "");
    char messageForme[64] = "Voici le mot actualisé: "; char messageErreurs[24] = "\nNombre d'erreurs: ";
	strcat(messageModifiable, messageForme);
    strcat(messageModifiable, motDevine);
    strcat(messageModifiable, messageErreurs);
    strcat(messageModifiable, nberreurs);

    return *messageModifiable;
}