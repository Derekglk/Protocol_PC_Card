/************************** UDP SERVER CODE *********************************
Le serveur reçoit les échantillons envoyés par le client. Lorsqu'il a fini
la réception il les écrit sous format ASCII dans le fichier sortie.txt qu'il
crée.

Etapes:
On crée d'abord la socket UDP
On configure tous les attributs de la structure contenant l'adresse du serveur
On reçoit les paquets d'échantillons envoyés par le client
On les écrit dans un fichier
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h> // Bibliothèque pour déterminer le temps d'exécution (Variable clock_t et constante CLOCKS_PER_SEC)

int main(){
    int udpSocket, nBytes;
    float buffer[50][960]; // Buffer contenant tous les paquets (Chaque  ligne est un paquet)
    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    int i,j;

	/*Ouverture du fichier où l'on écrira tous les échantillons reçus*/
    FILE *sortie;
    sortie=fopen("sortie.txt","w+");

    long clk_tck = CLOCKS_PER_SEC; // Fréquence de l'horloge 
    clock_t t1, t2; // Tics d'horloge écoulés depuis le début de l'exécution


	/*Creation de la socket UDP */
    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

	/*Configuration des attributs de l'adresse du serveur*/
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7891);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

	/*Association de l'adresse du serveur à la socket créée*/
    bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

    while(j<50){
		/* Instant de réception du premier paquet*/
        if(j==0){
            t1 = clock();
        }

		/* Réception des échantillons envoyés par le client*/
        nBytes = recvfrom(udpSocket,buffer[j],960*sizeof(float),0,NULL, NULL);
    
		/* Instant de réception du dernier paquet*/    
        if(j==49){
            t2 = clock();
        }
       
        j++;

    }
  
	/*Affichage du temps écoulé pour la réception*/
    printf("Nb ticks/seconde = %ld,  Nb ticks écoulés %ld\n",clk_tck, (long)(t2-t1));
    printf("Temps consomme (ms) : %lf \n",(double)(t2-t1)*1000/(double)clk_tck);
   
    /* Ecriture des échantillons reçus dans un fichier*/
    for (j=0;j<50;j++){
        for (i=0; i<960; i++) {
            fprintf(sortie," %f \n",buffer[j][i] );
        }
    }

	/* Fermeture du fichier*/    
    fclose(sortie);

    return 0;
    
}
