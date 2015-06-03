/************* UDP CLIENT CODE *******************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define nb 1000
#define nb_trames 7500
#define ech 960


typedef struct {
  long buffer[nb][ech];
  int start[nb];
  int k;
}rec;

pthread_cond_t cond[nb] = PTHREAD_COND_INITIALIZER; /* Création de la condition */

pthread_mutex_t mutex[nb] = PTHREAD_MUTEX_INITIALIZER; /* Création du mutex */



void *t_env(void * buff){

    printf("thread envoi \n");
    
    FILE *fs; 
    fs=fopen("car.dat","r");
    


    int clientSocket, portNum, nBytes;
   //int buffer_rec[nb][960];
   long buffer_rec[960];
    int taille_fichier;
    
    int n1;
    int n2=0;

    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    

    //Create UDP socket
    clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

    //Configure settings in address struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7891);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

    //Initialize size variable to be used later on
    addr_size = sizeof serverAddr;
        
     for(n2=0; n2<nb_trames; n2++){  

               
        for(n1=0;n1<960;n1++){
            fscanf(fs,"%li",&(buffer_rec[n1]));
        }       
         
        sendto(clientSocket,buffer_rec,960*sizeof(long),0,(struct sockaddr *)&serverAddr,addr_size);
      printf("paquet envoyé %d \n",n2);
        usleep(6000);   



    }
    
  //printf("paquet envoyés %d\n",n2);
}   


void *t_rec (void * buff){
    printf("thread reception \n");

     rec *r =buff;

    int i,j=0;
    int udpSocket, portNum, nBytes;
    int p; 
    
    struct sockaddr_in serverAddr, clientAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size, client_addr_size;
    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);


    //Configure settings in address struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7892);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  
    
    bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
 
    //Initialize size variable to be used later on
    addr_size = sizeof serverAddr;  
    
    while((*r).k< nb_trames){
        p= (*r).k % nb;
      //Verouillage du mutex en attente de la réception complète du paquet*/
        pthread_mutex_lock(&mutex[p]);
        
        nBytes = recvfrom(udpSocket,(*r).buffer[p],960*sizeof(long),0,(struct sockaddr *)&serverStorage, &addr_size);
              (*r).start[p]=1;

        printf("Paquet recu %d\n",(*r).k);

		//Déverouillage du mutex
        pthread_cond_signal(&cond[p]); 
        pthread_mutex_unlock(&mutex[p]);
  
        (*r).k=(*r).k+1;
   }
   
     // printf("paquets reçu %d \n",j);
      
      

}    

void *t_ecr (void * buff){

  rec *r=buff;
  printf("thread ecriture");
    FILE *sortie2; 
    sortie2=fopen("sortie_cli.txt","wt");

  int n1=0,n2=0,i;
    int k;

	for (n1 = 0; n1 < nb_trames; n1++)
	{
	    k= n1 % nb;
	  /*Verouillage du mutex et attente de la réception du paquet entier*/
             pthread_mutex_lock(&mutex[k]);   
             while ((*r).start[k] == 0) {
             pthread_cond_wait(&cond[k], &mutex[k]);
	     }
             pthread_mutex_unlock(&mutex[k]);

       for(i=0;i<ech;i++){
           fprintf(sortie2,"%li \n",(*r).buffer[k][i]);
       }
        
       printf("Paquet ecrit %d\n",n1);
       (*r).start[k]=0;
        }
       
        
    

}

int main(){

    pthread_t mont_rec;
    pthread_t mont_env;
    pthread_t mont_ecr;
    
    rec *args = malloc(sizeof *args);
    (*args).k=0;
    
    pthread_create(&mont_env,NULL,t_env,NULL);
    pthread_create(&mont_rec,NULL,t_rec,args);
    pthread_create(&mont_ecr,NULL,t_ecr,args);
      
    pthread_join (mont_rec, NULL);
    pthread_join (mont_env, NULL);
    pthread_join (mont_ecr, NULL);

    return 0;
}




