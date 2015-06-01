/************* UDP CLIENT CODE *******************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#define nb 750
#define ech 960


int main(){

    int clientSocket;
    long buffer_rec[ech];
    
    int n1=0;
    int n2=0;

    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    
    //Lecture du fichier
    FILE *fs;
    fs = fopen("car.dat", "r");

    //Create UDP socket
    clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

    //Configure settings in address struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7891);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

    //Initialize size variable to be used later on
    addr_size = sizeof serverAddr;
        
     for(n2=0; n2<nb; n2++){  
              
        for(n1=0;n1<ech;n1++){
            fscanf(fs,"%li",&(buffer_rec[n1]));
        }    
            
        sendto(clientSocket,buffer_rec,ech*sizeof(long),0,(struct sockaddr *)&serverAddr,addr_size);
        printf("paquet envoyé %d\n",n2);
        usleep(6000);   

    }
    
    fclose(fs);
    return 0;
}

