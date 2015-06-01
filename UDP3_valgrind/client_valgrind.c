/************* UDP CLIENT CODE *******************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define nb 750

pthread_cond_t cond[nb] = PTHREAD_COND_INITIALIZER; /* Création de la condition */

pthread_mutex_t mutex[nb] = PTHREAD_MUTEX_INITIALIZER; /* Création du mutex */



void *t_env(void * buff){

    printf("thread envoi \n");


    int clientSocket, portNum, nBytes;
   //int buffer_rec[nb][960];
   long buffer_rec[960];
    int taille_fichier;
    
    int n1;
    int n2=0;

    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    
    //Lecture du fichier
    FILE *fs;
    fs = fopen("car.dat", "r");
    printf("car.dat opened\n");
    //Create UDP socket
    clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

    //Configure settings in address struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7891);
    serverAddr.sin_addr.s_addr = inet_addr("192.168.0.100");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

    //Initialize size variable to be used later on
    addr_size = sizeof serverAddr;
    printf("begin to envoyer packet by packet\n");
    for(n2=0; n2<nb; n2++){  

               
        for(n1=0;n1<960;n1++){
            fscanf(fs,"%li",&(buffer_rec[n1]));
        }        
        sendto(clientSocket,buffer_rec,960*sizeof(long),0,(struct sockaddr *)&serverAddr,addr_size);
        //printf("paquet envoyé %d\n",n2);
        usleep(6000);   

    }
    fclose(fs);
}   


void *t_rec (void * buff){

    printf("thread reception \n");
    int p=sizeof(long);
    printf("%d",p);
    

    
    long b[nb][960];
    printf("b[750][960] defined\n");
    FILE *sortie2; 
    sortie2=fopen("sortie_cli.txt","wt");
    
    int i,j=0;
        int udpSocket, portNum, nBytes;
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
    printf("reception begin\n");
    while(j<750){ 
        
        nBytes = recvfrom(udpSocket,b[j],960*sizeof(long),0,(struct sockaddr *)&serverStorage, &addr_size);
        
    j++;
   }
   printf("reception finished\n");
   for(j=0;j<750;j++){
        for(i=0;i<960;i++){
            fprintf(sortie2,"%li \n",b[j][i]);
        }
        //printf("paquet reçu %d \n",(*r).k);
        //printf("receive %d \n",j);
    }
    printf("writing sortie finished\n");
    fclose(sortie2);
}    


int main(){

    pthread_t mont_rec;
    pthread_t mont_env;

    
    pthread_create(&mont_env,NULL,t_env,NULL);
    pthread_create(&mont_rec,NULL,t_rec,NULL);

      
    pthread_join (mont_rec, NULL);
    pthread_join (mont_env, NULL);
    
    //free(args);
    return 0;
}




