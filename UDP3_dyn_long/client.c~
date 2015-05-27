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

#define nb 50 // Nombre de paquets à lire du fichier

/* Thread envoi*/
void *t_env(void * buff){

    int clientSocket, nBytes;
    long ** buffer_rec;

    int i, j;

    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    
	//Allocation memoire 
	printf("Debut allocation memoire env \n");
	buffer_rec = (long **) malloc(nb*sizeof(long* ));

	if (buffer_rec == NULL) {
		printf("Erreur memoire env \n");
		exit(-2);
	}
	else{
		printf("Memoire alloue env  \n");
	}   
	
	for(i=0;i<nb;i++){
		buffer_rec[i] = (long *) malloc(960*sizeof(long));
		if (buffer_rec[i] == NULL) {
			printf("Erreur memoire env %d \n",i);
			exit(-2);
		}
		else{
		printf("Memoire alloue env %d \n",i);
		}   
	}
	

	
	
	/*Ouverture du fichier et lecture*/
    FILE *fs;
    fs = fopen("car.dat", "r");
	if (fs == NULL ){
		printf("Cannot open file  \n");

	}	
	
	printf("Fichier ouvert \n");
	  
	
	printf("Debut lecture fichier\n");
	/* Lecture de 960 échantillons (int) à partir du fichier et stockage des échantillons dans buffer_rec*/
	for(j=0; j<nb; j++){  
		
		for(i=0;i<960;i++){
		fscanf(fs,"%ld",&buffer_rec[j][i]);
		}
		
    }
    
	
       

	/*Fermeture du fichier*/
	fclose(fs);
	
	printf("Fichier lu \n");

	/*Creation de la socket UDP */
    clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

	/*Configuration des attributs de l'adresse du serveur*/
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7891);
    serverAddr.sin_addr.s_addr = inet_addr("192.168.0.100"); // Ici c'est l'adresse du serveur
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

	/*Initialisation de la variable où l'on va stocker l'adresse du client*/
    addr_size = sizeof serverAddr;
        
	j=0;
    while(j<nb){  
		
		/* Envoi des échantillons au serveur*/
        sendto(clientSocket,buffer_rec[j],960*sizeof(long),0,(struct sockaddr *)&serverAddr,addr_size);
		j++;	
        usleep(6000);   

    }
    
    printf("Nombre de paquets envoyés %d \n", j);
	
	/*Liberation de la memoire */

	for(i=0;i<nb;i++){
		free(buffer_rec[i]); 
    
	}
	free(buffer_rec);

}   

void *t_rec (void * buff){

    long ** b; // buffer où l'on va stocker les échantillons débruités
    
    int i,j=0;
    int udpSocket, nBytes;
	struct sockaddr_in serverAddr;
	socklen_t addr_size;

	/*Creation de la socket UDP */
    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

	/*Configuration des attributs de l'adresse du client*/
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7892);
    serverAddr.sin_addr.s_addr = inet_addr("192.168.0.2"); // Ici c'est l'adresse du client
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  
    
	/*Association de l'adresse du client à la socket créée*/
    bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
	
	//Allocation memoire 
	printf("Debut allocation memoire rec \n");
	b = (long **) malloc(nb*sizeof(long* ));

	if (b == NULL) {
		printf("Erreur memoire rec \n");
		exit(-2);
	}
	else{
		printf("Memoire alloue rec  \n");
	}   
	
	for(i=0;i<nb;i++){
		b[i] = (long *) malloc(960*sizeof(long));
		if (b[i] == NULL) {
			printf("Erreur memoire rec %d \n",i);
			exit(-2);
		}
		else{
		printf("Memoire alloue rec %d \n",i);
		}   
	}
    
	j=0;
    while(j<nb){ 
        
		/* Réception des paquets */
        nBytes = recvfrom(udpSocket,b[j],960*sizeof(long),0,NULL,NULL);

        j++;
   }
   
    printf("Nombre de paquets recus %d \n", j);

	/*Ouverture du fichier où l'on écrira tous les échantillons reçus*/
	printf("ouverture fichier pour reception  \n");    
    FILE *sortie; 
    sortie=fopen("sortie_cli.txt","w");
	
	if (sortie == NULL ){
		printf("Cannot open file sortie  \n");

	}	
	
	printf("Fichier sortie ouvert \n");	
	
	/*Ecriture des échantillons dans un fichier*/
	for(j=0;j<nb;j++){
        for(i=0;i<960;i++){
            fprintf(sortie,"%ld \n",b[j][i]);
        }
    }  
	
	/*Fermeture du fichier*/
	fclose(sortie); 

	/*Liberation de la memoire */

	for(i=0;i<nb;i++){
		free(b[i]); 

	}
	free(b); 

}    

int main(int argc, char **argv){

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




