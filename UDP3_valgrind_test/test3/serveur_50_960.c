#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#define nb 50
#define nb_trames 750
#define ech 960

typedef struct {
  int buffer[nb][ech];
  int start[nb];
  int k;
}rec;


pthread_cond_t cond[nb_trames] = PTHREAD_COND_INITIALIZER; /* Création de la condition */
pthread_mutex_t mutex[nb_trames] = PTHREAD_MUTEX_INITIALIZER; /* Création du mutex */
    
void *t_trait (void * buff){

  struct sockaddr_in serverAddr;
  socklen_t addr_size;
  int udpSocket;

  rec *r=buff;


  int n1=0,n2=0,i;
    int k;

    
    //Creation de la socket UDP
    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

    //Initialisation de l'adresse du client
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr =inet_addr("192.168.0.2"); 
    serverAddr.sin_port = htons(7892);
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

    //Initialisation de la taille de l'adresse du client
    addr_size = sizeof serverAddr; 
   
	for (n1 = 0; n1 < nb_trames; n1++)
	{
	    k= n1 % nb;
	  /*Verouillage du mutex et attente de la réception du paquet entier*/
             pthread_mutex_lock(&mutex[k]);   
             while ((*r).start[k] == 0) {
             pthread_cond_wait(&cond[k], &mutex[k]);
	     }
             pthread_mutex_unlock(&mutex[k]);
            

		sendto(udpSocket,(*r).buffer[k],960*sizeof(int),0,(struct sockaddr *)&serverAddr,addr_size);	     
       printf("Paquet envoyé %d\n",n1);
       (*r).start[k]=0;
        }
       
        
    
}   

    
void *t_rec (void * buff){
    rec *r =buff;
    
    int udpSocket, nBytes ,n1,n2;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    int p;
    
    //Création de la socket UDP
    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

    //Initialisation de l'adresse du serveur 
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr =1677764800; 
    serverAddr.sin_port = htons(7891);
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

	//Association de l'adresse du serveur à la socket créée
    bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
   
    //Réception des paquets
    while((*r).k<nb_trames){
        p= (*r).k % nb;
      //Verouillage du mutex en attente de la réception complète du paquet*/
        pthread_mutex_lock(&mutex[p]);
        nBytes = recvfrom(udpSocket,(*r).buffer[p],960*sizeof(int),0,NULL,NULL);

        (*r).start[p]=1;

        printf("Paquet recu %d\n",(*r).k);

		//Déverouillage du mutex
        pthread_cond_signal(&cond[p]); 
        pthread_mutex_unlock(&mutex[p]);
  
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






