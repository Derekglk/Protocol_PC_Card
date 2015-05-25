/************************** UDP SERVER CODE *********************************
Le serveur reçoit les échantillons envoyés par le client dans le thread 
réception. A chaque fois qu'un paquet est reçu, le thread de traitement l'écrit
dans un fichier. Un systeme de verrous permet de synchroniser les threads.

Etapes:
On crée les threads dans le main
On crée ensuite la socket UDP
On configure tous les attributs de la structure contenant l'adresse du serveur
On verouille le mutex j jusqu'à la réception du paquet j.
On écrit le paquet dans un fichier une fois le mutex déverouillé

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

/* Structure contenant le buffer à partager entre les threads et les conditions d'excution ou d'arret du thread traitement*/
typedef struct {
  int buffer[50][960]; // Buffer contenant les échantillons
  int start[50]; // Tableau de conditions initialement  à 0. Tant que start[j]=0 le thread traitement n'accède pas à buffer[j]. Lorsque le serveur reçoit le paquet j en entier il met la valeur 1 dans start[j] pour permettre au thread réception l'accès au paquet j. Ainsi le traitement ne s'effectuera pas sur des lignes vides du tableau mais attendra qu'elles soient remplies. 
  int k; // 
}rec;

/* Les mutex servent à verouiller l'accès à des cases mémoires, les conditions servent à envoyer un signal à l'autre thread pour lui permettre de continuer son exécution. On a donc autant de verrous et de conditions que de paquets cad 50*/
pthread_cond_t cond[50] = PTHREAD_COND_INITIALIZER; /* Création de la condition */
pthread_mutex_t mutex[50] = PTHREAD_MUTEX_INITIALIZER; /* Création du mutex */

/* Thread traitement*/
void *t_trait (void * buff){



	/* Cast du pointeur buff de type void en un pointeur sur la structure rec*/
    rec *r=buff;

    int i,j=0;
    
    FILE *sortie; 
    sortie=fopen("sortie.txt","w+");
    
    while(j<50){     
		
		/* On verouille le mutex et on attend que la condition (*r).start[j] )= 1 soit réalisée. Des qu'elle est réalisée on déverouille le mutex */
        pthread_mutex_lock(&mutex[j]);  
      
        while ((*r).start[j] == 0) {
            pthread_cond_wait(&cond[j], &mutex[j]);
        }

        pthread_mutex_unlock(&mutex[j]);

		/* Ecriture dans le fichier de sortie : c'est ce en quoi consiste notre traitement dans un premier temps */
        for (i=0; i<960; i++) {
            fprintf(sortie," %d\n",(*r).buffer[j][i]);
        }

        printf("trait paquet %d\n",j);
        j++;
    }

    fclose(sortie);
}   

/* Thread réception*/
void *t_rec (void * buff){

	/* Cast du pointeur buff de type void en un pointeur sur la structure rec*/
    rec *r=buff;

    int udpSocket, nBytes;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    /*Create UDP socket*/
    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

    /*Configure settings in address struct*/
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7891);
    serverAddr.sin_addr.s_addr = inet_addr("192.168.0.100");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

	/*Association de l'adresse du serveur à la socket créée*/
    bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

    while((*r).k<50){
    	
		/* Verouillage du mutex tant que le paquet n'est pas entièrement reçu*/
        pthread_mutex_lock(&mutex[(*r).k]);

		/* Réception du paquet */
        nBytes = recvfrom(udpSocket,(*r).buffer[(*r).k],960*sizeof(int),0,NULL,NULL);
        //printf("received %d\n", (*r).buffer[(*r).k][0]);

        /* Changement de la condition*/
        (*r).start[(*r).k]=1;
		
		/* Envoi du signal au thread traitement pour déverouiller le mutex et permettre l'accès au paquet*/
        pthread_cond_signal(&cond[(*r).k]); 

		/* Déverouillage du mutex */
        pthread_mutex_unlock(&mutex[(*r).k]);

        printf("recept paquet %d\n",(*r).k);
  
        (*r).k=(*r).k+1;
    } 

}    

int main(){

	/*Déclaration des threads*/
    pthread_t mont_rec;
    pthread_t mont_trait;

	/*Création du buffer que les deux threads vont partager et initialisation de k*/    
    rec *args = malloc(sizeof *args);
    (*args).k=0;
    
	/*Création des threads*/
	// 1er paramètre : adresse de la variable pthread_t déclarée
	// 2eme paramètre : Attributs du thread ( NULL = attributs par défaut)
	// 3eme paramètre : nom de la fonction associée au thread 
	// 4eme parametre : argument à donner en entrée de cette fonction (il ne faut passer qu'un seul paramètre c'est pour cela qu'on met tout ce qu'on veut donner en paramètre dans une seule structure , ici rec 
    pthread_create(&mont_trait,NULL,t_trait,args);
    pthread_create(&mont_rec,NULL,t_rec,args);

	/*pthread_join suspeds l'execution du main jusqu'à ce que l'exécution du thread mis en paramètre soit terminé*/   
	// Nous n'utilisons pas le statut de l'exit du thread mis en paramètre donc le 2ème param est mis à NULL   
    pthread_join (mont_rec, NULL);
    pthread_join (mont_trait, NULL);
    
    return 0;
}






