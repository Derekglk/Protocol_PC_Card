/************* UDP CLIENT CODE *******************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define nb 500
#define nb_trames 750
#define ech 960


typedef struct {
  long buffer_env[nb][ech];
  long buffer_rec[nb][ech];
  int start[nb];
  int start1[nb];
  int k;
  int m;
}rec;

clock_t t1, t2;

pthread_cond_t cond[nb] = PTHREAD_COND_INITIALIZER; /* Création de la condition */
pthread_mutex_t mutex[nb] = PTHREAD_MUTEX_INITIALIZER; /* Création du mutex */

pthread_cond_t cond1[nb] = PTHREAD_COND_INITIALIZER; /* Création de la condition */
pthread_mutex_t mutex1[nb] = PTHREAD_MUTEX_INITIALIZER; /* Création du mutex */


void *t_lec(void * buff){

    printf("thread lecture \n");
    
    rec *r =buff;
    
    FILE *fs; 
    fs=fopen("car.dat","r");
    
    int p;
    int n1;

    while((*r).m< nb_trames){
        
        if((*r).m==0){
            t1=clock();
        }
        
        p = (*r).m % nb;
        
        pthread_mutex_lock(&mutex1[p]);
        
        for(n1=0;n1<960;n1++){
            fscanf(fs,"%li",&((*r).buffer_env[p][n1]));
        }
        
        (*r).start1[p]=1;

        if((*r).m==(nb_trames-1)){
            printf("Paquet lu %d\n",(*r).m);
        }
        
        usleep(6000);   

        pthread_cond_signal(&cond1[p]); 
        pthread_mutex_unlock(&mutex1[p]);
  
        (*r).m=(*r).m+1;
    }
    

}

void *t_env(void * buff){
    printf("thread envoi \n");
    int clientSocket;
    
    rec *r =buff;
    
    int n2,p;

    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    
    //Create UDP socket
    clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

    //Configure settings in address struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7891);
    serverAddr.sin_addr.s_addr = inet_addr("192.168.0.100");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

    //Initialize size variable to be used later on
    addr_size = sizeof serverAddr;
        
     for(n2=0; n2<nb_trames; n2++){  
     
        p=n2 % nb;
        
        pthread_mutex_lock(&mutex1[p]);   
             while ((*r).start1[p] == 0) {
             pthread_cond_wait(&cond1[p], &mutex1[p]);
	     }
             pthread_mutex_unlock(&mutex1[p]);

        sendto(clientSocket,(*r).buffer_env[p],960*sizeof(long),0,(struct sockaddr *)&serverAddr,addr_size);
        if(n2==(nb_trames-1)){
            printf("paquet envoyé %d \n",n2);
        }
        
        
        (*r).start1[p]=0;
        usleep(6000);   

    }
    
  //printf("paquet envoyés %d\n",n2);
}   


void *t_rec (void * buff){
    printf("thread reception \n");

     rec *r =buff;

    int i,j=0;
    int udpSocket, nBytes;
    int p; 
    
    struct sockaddr_in serverAddr, clientAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size, client_addr_size;
    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);


    //Configure settings in address struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7892);
    serverAddr.sin_addr.s_addr = inet_addr("192.168.0.2");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  
    
    bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
 
    //Initialize size variable to be used later on
    addr_size = sizeof serverAddr;  
    
    while((*r).k< nb_trames){
    
        p= (*r).k % nb;
        
      //Verouillage du mutex en attente de la réception complète du paquet*/
        pthread_mutex_lock(&mutex[p]);
        
        nBytes = recvfrom(udpSocket,(*r).buffer_rec[p],960*sizeof(long),0,(struct sockaddr *)&serverStorage, &addr_size);
        (*r).start[p]=1;
        if((*r).k== (nb_trames-1)){
            printf("Paquet recu %d\n",(*r).k);
        }
        

		//Déverouillage du mutex
        pthread_cond_signal(&cond[p]); 
        pthread_mutex_unlock(&mutex[p]);
  
        (*r).k=(*r).k+1;
   }
   
     // printf("paquets reçu %d \n",j);
      
      

}    

void *t_ecr (void * buff){

  rec *r=buff;
  printf("thread ecriture \n");
    FILE *sortie2; 
    sortie2=fopen("sortie_cli.txt","wt");

  int n1=0,n2=0,i;
    int p;

	for (n1 = 0; n1 < nb_trames; n1++)
	{
	    p= n1 % nb;
	  /*Verouillage du mutex et attente de la réception du paquet entier*/
             pthread_mutex_lock(&mutex[p]);   
             while ((*r).start[p] == 0) {
             pthread_cond_wait(&cond[p], &mutex[p]);
	     }
             pthread_mutex_unlock(&mutex[p]);

       for(i=0;i<ech;i++){
           fprintf(sortie2,"%li \n",(*r).buffer_rec[p][i]);
       }
       if(n1%50==0){
        printf("Paquet ecrit %d\n",n1);
       } 
       
       (*r).start[p]=0;
       
       if(n1==(nb_trames-1)){
            t2=clock();
       }
     
     }
     printf("Temps consomme (s) : %lf \n",(double)(t2-t1)/(double)CLOCKS_PER_SEC);
       
}

int main(){

    pthread_t mont_rec;
    pthread_t mont_env;
    pthread_t mont_ecr;
    pthread_t mont_lec;
    
    rec *args = malloc(sizeof *args);
    (*args).k=0;
    (*args).m=0;
        
    pthread_create(&mont_lec,NULL,t_lec,args);  
    pthread_create(&mont_env,NULL,t_env,args);
    pthread_create(&mont_rec,NULL,t_rec,args);
    pthread_create(&mont_ecr,NULL,t_ecr,args);

    
    pthread_join (mont_rec, NULL);
    pthread_join (mont_env, NULL);
    pthread_join (mont_ecr, NULL);
    pthread_join (mont_lec, NULL);

    return 0;
}




