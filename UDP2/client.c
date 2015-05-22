/************************** UDP CLIENT CODE *********************************
Le client lit un fichier binaire où est stockée 1sec d'une sinuoide 
échantillonée à 48KHz. Il envoie des paquets de 960 échantillons au serveur.
(Pas de modification majeure depuis la version précédente mis à part le rajout
de la fonction usleep après l'envoi de chaque paquet)

Etapes:
On crée d'abord la socket UDP
On configure tous les attributs de la structure contenant l'adresse du serveur
On parcourt le fichier et on lit 960 échantillons à chaque fois
On envoie chaque 960 échantillons récupérés du fichier en un seul paquet
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

int main(){
    int clientSocket, nBytes;
    float buffer[960];
    
    long clk_tck = CLOCKS_PER_SEC;
    clock_t t1, t2;

    int j=0;

    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    /*Ouverture du fichier*/
    FILE *fs;
    fs = fopen("signal1.txt", "rb");

	/*Creation de la socket UDP */
    clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

	/*Configuration des attributs de l'adresse du serveur*/
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7891);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

	/*Stockage de la taille de l'adresse*/
    addr_size = sizeof serverAddr;

    /* Le fichier contient 50 paquets = 48 000 / 960  */    
    for(j=0; j<50; j++){
        
		/* Instant d'envoi du premier paquet*/
        if(j==0){
            t1 = clock();
        }
        
		/* Lecture de 960 échantillons (floats) à partir du fichier et stockage des échantillons dans buffer*/
		fread(buffer,sizeof(float),960,fs);

		/* Envoi des échantillons au serveur*/
		sendto(clientSocket,buffer,960*sizeof(float),0,(struct sockaddr *)&serverAddr,addr_size);

		/* Cote Serveur le thread de réception a besoin d'exécuter à chaque fois qu'il reçoit un paquet un nombre d'instructions pour permettre au thread de traitement d'accéder au nouveau paquet reçu. Si le client envoie des paquets pendant que le serveur exécute ces instructions, ils sont perdus. C'est pour cela qu'on retarde un peu l'envoi d'un nouveau paquet. La durée à donner en paramètre de la fonction usleep est exprimée en microsecondes.*/
    	usleep(5000);

		/* Instant d'envoi du dernier paquet*/    
        if(j==49){
            t2 = clock();
        }
        
    }
  
	/*Affichage du temps écoulé pour la réception*/
    printf("Temps consomme (ms) : %lf \n",(double)(t2-t1)*1000/(double)clk_tck);

	/* Fermeture du fichier*/    
    fclose(fs);

    return 0;
}

