/************************** UDP CLIENT CODE *********************************
Le client lit un fichier ASCII où sont stockés des échantillons bruités. 
Il envoie des paquets de 960 échantillons au serveur. Il reçoit en parallèle
les échantillons débruités envoyés par le serveur. Ici pas de variable partagée,
donc pas de synchronisation nécessaire => pas de mutex ni de conditions.

Etapes:
On crée les threads dans le main
	Dans le thread envoi:
On crée la socket UDP 
On configure les attributs des variables contenant l'adresse du serveur
On parcourt le fichier et on lit 960 échantillons à chaque fois
On envoie chaque 960 échantillons récupérés du fichier en un seul paquet
	Dans le thread réception:
On crée la socket UDP 
On configure les attributs des variables contenant l'adresse du client
On reçoit les paquets débruités envoyés par le serveur
On les écrit dans un fichier quand on les a tous reçus.

Note:
Pour compiler ce programme : gcc -pthread -o nom_exec nom_fichier.c
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <pthread.h> // Bibliothèque pour créer et gérer les threads

#define nb 750 // Nombre de paquets à lire du fichier

/* Thread envoi*/
void *t_env(void * buff){

    int clientSocket, nBytes;
    int buffer_rec[nb][960];
    
    int i, j=0;

    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    
    /*Ouverture du fichier*/
    FILE *fs;
    fs = fopen("car.dat", "rb");

	/*Creation de la socket UDP */
    clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

	/*Configuration des attributs de l'adresse du serveur*/
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7891);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Ici c'est l'adresse du serveur
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

	/*Initialisation de la variable où l'on va stocker l'adresse du client*/
    addr_size = sizeof serverAddr;
        

    for(j=0; j<nb; j++){  

   		/* Lecture de 960 échantillons (int) à partir du fichier et stockage des échantillons dans buffer_rec*/
        for(i=0;i<960;i++){
            fscanf(fs,"%d",&buffer_rec[j][i]);
        }        

		/* Envoi des échantillons au serveur*/
        sendto(clientSocket,buffer_rec[j],960*sizeof(int),0,(struct sockaddr *)&serverAddr,addr_size);

        usleep(6000);   

    }
    
    printf("Nombre de paquets envoyés %d \n", j);
    
	/*Fermeture du fichier*/
	fclose(fs);

}   


void *t_rec (void * buff){

    int b[nb][960]; // buffer où l'on va stocker les échantillons débruités

	/*Ouverture du fichier où l'on écrira tous les échantillons reçus*/    
    FILE *sortie; 
    sortie=fopen("sortie_cli.txt","w+");
    
    int i,j=0;
    int udpSocket, nBytes;
	struct sockaddr_in serverAddr;
	socklen_t addr_size;

	/*Creation de la socket UDP */
    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

	/*Configuration des attributs de l'adresse du client*/
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7892);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Ici c'est l'adresse du client
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  
    
	/*Association de l'adresse du client à la socket créée*/
    bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    
    while(j<nb){ 
        
		/* Réception des paquets */
        nBytes = recvfrom(udpSocket,b[j],960*sizeof(int),0,NULL,NULL);

        j++;
   }
   
   printf("Nombre de paquets recus %d \n", j);

	/*Ecriture des échantillons dans un fichier*/
	for(j=0;j<nb;j++){
        for(i=0;i<960;i++){
            fprintf(sortie,"%d \n",b[j][i]);
        }
    }  
	
	/*Fermeture du fichier*/
	fclose(sortie);  

}    


int main(){

	/*Déclaration des threads*/
    pthread_t mont_rec;
    pthread_t mont_env;
    
	/*Création des threads*/
    pthread_create(&mont_env,NULL,t_env,NULL);
    pthread_create(&mont_rec,NULL,t_rec,NULL);

	/*Suspension du main en attente de l'exit des threads mont_rec et mont_rec*/
    pthread_join (mont_rec, NULL);
    pthread_join (mont_env, NULL);
    
    return 0;
}




