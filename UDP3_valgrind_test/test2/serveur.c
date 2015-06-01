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
  int buffer[nb][ech];
  int start[nb];
  int k;
}rec;


pthread_cond_t cond[nb] = PTHREAD_COND_INITIALIZER; /* Création de la condition */
pthread_mutex_t mutex[nb] = PTHREAD_MUTEX_INITIALIZER; /* Création du mutex */
    
void *t_trait (void * buff){

  struct sockaddr_in serverAddr;
  socklen_t addr_size;
  int udpSocket;

  int sortie[nb][ech];
  int tab[ech]; //Tableau pour mettre en forme les échantillons avant de les envoyer

  rec *r=buff;


  int n1=0,n2=0,i;


    
    //Creation de la socket UDP
    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

    //Initialisation de l'adresse du client
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr =inet_addr("127.0.0.1"); 
    serverAddr.sin_port = htons(7892);
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

    //Initialisation de la taille de l'adresse du client
    addr_size = sizeof serverAddr; 
   
	for (n1 = 0; n1 < 750; n1++)
	{
	  /*Verouillage du mutex et attente de la réception du paquet entier*/
             pthread_mutex_lock(&mutex[n1]);   
             while ((*r).start[n1] == 0) {
             pthread_cond_wait(&cond[n1], &mutex[n1]);
	     }
             pthread_mutex_unlock(&mutex[n1]);
            

		sendto(udpSocket,(*r).buffer[n1],960*sizeof(int),0,(struct sockaddr *)&serverAddr,addr_size);	     
       printf("Paquet envoyé %d\n",n1);
        }
       
        
    
}   

    
void *t_rec (void * buff){
    rec *r =buff;
    
    int udpSocket, nBytes ,n1,n2;
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
        nBytes = recvfrom(udpSocket,(*r).buffer[(*r).k],960*sizeof(int),0,NULL,NULL);

        (*r).start[(*r).k]=1;

        printf("Paquet recu %d\n",(*r).k);

		//Déverouillage du mutex
        pthread_cond_signal(&cond[(*r).k]); 
        pthread_mutex_unlock(&mutex[(*r).k]);
  
        (*r).k=(*r).k+1;
    } 
 
    	//printf("Paquet recus %d\n",(*r).k);
    

        
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






