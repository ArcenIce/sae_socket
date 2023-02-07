#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */

#define LG_MESSAGE 256

char getMessage(int *dialog) {

    int lus;
    char messageRecu[LG_MESSAGE];
    memset(messageRecu, 0x00, LG_MESSAGE*sizeof(char));

    lus = read(*dialog, messageRecu, LG_MESSAGE*sizeof(char)); // ici appel bloquant
    switch(lus) {
        case -1 : /* une erreur ! */ 
            perror("read"); 
            close(*dialog); 
            exit(-5);
        case 0  : /* la socket est fermée */
            fprintf(stderr, "La socket a été fermée par le client !\n\n");
            close(*dialog);
            return 0;
        default:  /* réception de n octets */
            printf("Message reçu : %s (%d octets)\n\n", messageRecu, lus);
    }

    return *messageRecu;

}