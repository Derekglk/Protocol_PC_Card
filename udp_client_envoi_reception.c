#include <sys/mount.h>

/************* UDP CLIENT CODE *******************/
/* Original version, there are some redundant lines*/
/* Send size-reduced car.dat*/
/* The version that modified in batiment K2*/
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define nb 10
//Déclaration des verrous et conditions pour la synchronisation des threads 
pthread_cond_t cond[nb] = PTHREAD_COND_INITIALIZER; /* Création de la condition */
pthread_mutex_t mutex[nb] = PTHREAD_MUTEX_INITIALIZER; /* Création du mutex */

//Structure contenant le buffer  
typedef struct {
  int start[nb];
    int star[nb];
  int k;
}rec;

//|=======================================================================================
/// Method name: t_rec(void * buff)
/// Description:Traitement Thread Envoi  
 
//|=======================================================================================

void *t_env(void * buff)
{

    printf("create send thread \n");
    rec *r=buff;

    int clientSocket, portNum, nBytes;
    int buffer_rec[nb][960];
    int taille_fichier;
    
    int i;
    int j=0;

    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    
    //Lecture du fichier
    FILE *fs;
    fs = fopen("car.dat", "r");
    printf("open car.dat \n");
    //Create UDP socket
    clientSocket = socket(PF_INET, SOCK_DGRAM, 0);
    printf("create UDP socket \n");
    //Configure settings in address struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7891);
    serverAddr.sin_addr.s_addr = inet_addr("192.168.0.100");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  
    printf("complete configure settings in address struct \n");
    //Initialize size variable to be used later on
    addr_size = sizeof serverAddr;
    
    //Remplir le buffer
    for(j=0; j<nb; j++)
    {
        for(i=0;i<960;i++)
        {
            fscanf(fs,"%d",&buffer_rec[j][i]);
        }
    }
    fclose(fs);
    printf("buffer filled \n");
    //Envoie des nb paquets
    for(j=0; j<nb; j++)
    {

        //recupération d'un paquet          
        /*
        for(i=0;i<960;i++){
            fscanf(fs,"%d",&buffer_rec[j][i]);
        }
        */  

        //envoi par paquet de 960 échantillons 
        sendto(clientSocket,buffer_rec[j],960*sizeof(int),0,(struct sockaddr *)&serverAddr,addr_size);
        printf("paquet envoyé %d\n",j);
        //Assurer la synchronisation entre envoi coté client et reception coté serveur 
        usleep(5000);

    }
}   



//|=======================================================================================
/// Method name: t_rec(void * buff)
/// Description:Traitement Thread Reception  
 
//|=======================================================================================

void *t_rec (void * buff)
{

    printf("thread reception \n");
    
    rec *r=buff;
    
    int b[nb][960];
    
    FILE *sortie2; 
    sortie2=fopen("sortie_cli.txt","w+");
    printf("create sortie_cli.txt \n")
    int i,j=0;
    int udpSocket, portNum, nBytes;

    struct sockaddr_in serverAddr, clientAddr;

    struct sockaddr_storage serverStorage;
    socklen_t addr_size, client_addr_size;
    //Creation d'un socket
    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);
    printf("create reception socket\n")

    //Configure settings in address struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7892);
    serverAddr.sin_addr.s_addr = inet_addr("192.168.0.2");
    printf("thread reception \n")
    
    //Enregistrement de l'adresse 
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    //lier le socket crée avec une adresse IP et un port  
    bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
 
    //Initialize size variable to be used later on
    addr_size = sizeof serverAddr;

    //Réception des paquets aprés debruitage
    while((*r).k<nb)
    {

        nBytes = recvfrom(udpSocket,b[(*r).k],960*sizeof(int),0,(struct sockaddr *)&serverStorage, &addr_size);
        for(i=0;i<960;i++)
        {
            fprintf(sortie2,"%d \n",b[(*r).k][i]);
        }
        printf("paquet reçu %d\n",(*r).k);

    (*r).k++;
   }

}




//|=======================================================================================
/// Method name: main()
/// Description: Main processing
//|=======================================================================================

int main()
{

    //Initialisation des Threads
    pthread_t mont_rec;
    
    pthread_t mont_env;

    //Création et initialisation du buffer
    rec *args = malloc(sizeof *args);
    (*args).k=0;
    (*args).start[0]=1;
    
    //Creation des threads d'envoie et de reception    
    pthread_create(&mont_env,NULL,t_env,args);
    pthread_create(&mont_rec,NULL,t_rec,args);

    //Association des threads d'envoi et reception au thread main      
    pthread_join (mont_rec, NULL);
    pthread_join (mont_env, NULL);
    

    return 0;
}
// wget ftp://uestclx:liangxu@192.168.0.2/Protocol_PC_Card/UDP3/client_udp_test