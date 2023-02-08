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

// int strinstr(char *cs, char *ct)
// {
//     int index = -1;

//     if (cs != NULL && ct != NULL)
//     {
//         char *ptr_pos = NULL;

//         ptr_pos = strstr(&cs, &ct);
//         if (ptr_pos != NULL)
//         {
//             index = ptr_pos - cs;
//         }
//     }
//     return index;
// }

int verif_lettre(char *lettre_donnee, char *lettres_mot)
{
    for (int i = 0; i < strlen(lettres_mot); i++)
    {
        if (lettres_mot[i] == *lettre_donnee)
        {
            return 1;
        }
    }
    return 0;
}

void remplace_lettre(char *lettre_donne, char *mot, char *mot_devine)
{
    for (int i = 0; i < strlen(mot); i++)
    {
        if (*lettre_donne == mot[i])
        {
            mot_devine[i] = *lettre_donne;
        }
    }
}

void init_game(char *mot, char *lettre_mot, char *mot_devine)
{
    for (int i = 0; i < strlen(mot); i++)
    {

        if (strchr(lettre_mot, mot[i]) == 0)
        {
            // printf("%c", mot[i]);
            strcat(lettre_mot, &mot[i]);
        }
        strcat(mot_devine, "_");
    }
}

int verif_erreurs(int *erreurs)
{
    if(*erreurs == 6)
    {
        return 1;
    }
    return 0;
}

int checkStat(char *mot, char *mot_devine, int erreurs){
    if (erreurs == 6){
        return 2;
    }
    else if ((strcmp(mot, mot_devine) == 0)){
        return 1;
    }
    return 0;
}

char message_debut(char *message_modifiable, char *mot, char *mot_devine) {
    strcpy(message_modifiable, "");
	char message_nblettres[150] = "Le mot fait "; char nblettres[128]; char message_suite_nblettres[34] = " lettres de long, devinez le !\n";
	char message_forme[64] = "Voici la forme du mot: ";
	sprintf(nblettres, "%zu", strlen(mot));
	strcat(message_modifiable, message_nblettres); strcat(message_modifiable, nblettres); strcat(message_modifiable,message_suite_nblettres);
	strcat(message_modifiable, message_forme); strcat(message_modifiable, mot_devine);

    return *message_modifiable;
}

char message_actu(char *message_modifiable, char *mot_devine, char *nberreurs) {
    strcpy(message_modifiable, "");
    char message_forme[64] = "Voici le mot actualisé: "; char message_erreurs[24] = "\nNombre d'erreurs: ";
	strcat(message_modifiable, message_forme); strcat(message_modifiable, mot_devine);
    strcat(message_modifiable, message_erreurs); strcat(message_modifiable, nberreurs);

    return *message_modifiable;
}
