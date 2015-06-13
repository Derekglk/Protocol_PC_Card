/************************** UDP CLIENT CODE *********************************
Le client lit un fichier binaire où est stockée 1sec d'une sinuoide 
échantillonée à 48KHz. Il envoie des paquets de 960 échantillons au serveur.

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
#include <time.h> // Bibliothèque pour déterminer le temps d'exécution (Variable clock_t et constante CLOCKS_PER_SEC)

int main(){
    int clientSocket;
    float buffer[960]; // Buffer où l'on stocke les 960 échantillons lus à partir du fichier et qu'on enverra au serveur 
    
    long clk_tck = CLOCKS_PER_SEC; // Fréquence de l'horloge 
    clock_t t1, t2; // Tics d'horloge écoulés depuis le début de l'exécution

    int j=0;

    /*Ouverture du fichier*/
    FILE *fs;
    fs = fopen("signal1.txt", "rb");

    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    
	/*Creation de la socket UDP*/
    clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

	/*Configuration des attributs de la variable serverAddr */
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7891);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	/*Initialisation de l'attribut sin_zero par 0 (On remplit tous les octets alloués à la variable à 0)*/
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

		/* Instant d'envoi du dernier paquet*/    
        if(j==49){
            t2 = clock();
        }
        
    }
    
	/*Affichage du temps écoulé pour l'envoi*/
    printf("Nb ticks/seconde = %ld,  Nb ticks écoulés %ld\n",clk_tck, (long)(t2-t1));
    printf("Temps consomme (ms) : %lf \n",(double)(t2-t1)*1000/(double)clk_tck);
   
	/* Fermeture du fichier*/    
    fclose(fs);

    return 0;
}

