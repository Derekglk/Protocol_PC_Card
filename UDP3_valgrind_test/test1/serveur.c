#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#define nb 750
#define ech 960

typedef struct {
  long buffer[nb][ech];
  int start[nb];
  int k;
}rec;


pthread_cond_t cond[nb] = PTHREAD_COND_INITIALIZER; /* Création de la condition */
pthread_mutex_t mutex[nb] = PTHREAD_MUTEX_INITIALIZER; /* Création du mutex */
    
void *t_trait (void * buff){


  long sortie[nb][ech];
  long tab[ech]; //Tableau pour mettre en forme les échantillons avant de les envoyer

    rec *r =buff;
    
    FILE *sortie2; 
    sortie2=fopen("sortie.txt","w+");

    int n1=0,n2=0;
   
	for (n1 = 0; n1 < nb; n1++)
	{
             pthread_mutex_lock(&mutex[n1]);   
             while ((*r).start[n1] == 0) {
             pthread_cond_wait(&cond[n1], &mutex[n1]);
	     }
             pthread_mutex_unlock(&mutex[n1]);

        for(n2=0;n2<960;n2++){
            fprintf(sortie2,"%li \n",(*r).buffer[n1][n2]);
        }
     
		printf("Paquet traité %d\n",n1);

        }
    fclose(sortie2);
}   

    
void *t_rec (void * buff){
    rec *r =buff;
   
    int udpSocket, nBytes;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    
    //Création de la socket UDP
    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

    //Initialisation de l'adresse du serveur 
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr =16777343; 
    serverAddr.sin_port = htons(7891);
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

	//Association de l'adresse du serveur à la socket créée
    bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
   
    //Réception des paquets
    while((*r).k<nb){

      //Verouillage du mutex en attente de la réception complète du paquet*/
        pthread_mutex_lock(&mutex[(*r).k]);
        nBytes = recvfrom(udpSocket,(*r).buffer[(*r).k],960*sizeof(long),0,NULL,NULL);
	printf("Paquet recu %d\n",(*r).k);

        (*r).start[(*r).k]=1;

		//Déverouillage du mutex
        pthread_cond_signal(&cond[(*r).k]); 
        pthread_mutex_unlock(&mutex[(*r).k]);
  
        (*r).k=(*r).k+1;
    } 
    
    // printf("Nombre de paquets recus %d \n", ((*r).k));
        
}    

int main(){

    pthread_t mont_rec;
    pthread_t mont_trait;
    
    rec *args = malloc(sizeof *args);
    (*args).k=0;
    
    pthread_create(&mont_trait,NULL,t_trait,args);
    pthread_create(&mont_rec,NULL,t_rec,args);

      
    pthread_join (mont_rec, NULL);
    pthread_join (mont_trait, NULL);
    
    return 0;
}






