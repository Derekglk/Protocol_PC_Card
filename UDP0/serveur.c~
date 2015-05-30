/************************** UDP SERVER CODE *********************************
Le serveur reçoit la chaine de caractères envoyée par le client, la met en
majuscules puis la renvoie.

Etapes:
On crée d'abord la socket UDP
On configure tous les attributs de la structure contenant l'adresse du serveur
On reçoit la chaine en minuscules envoyée par le client et on stocke l'adresse
du client
On met la chaine reçue en majuscules
On la renvoie au client en utilisant l'adresse stockée
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>  //Bibliothèque pour créer la socket UDP
#include <netinet/in.h>  //Bibliothèque pour créer et stocker les adresses


int main(){
	int udpSocket, nBytes;
	char buffer[1024]; // Variable pour stocker la chaine de caractère envoyée par le client
	struct sockaddr_in serverAddr, clientAddr; // Variable adresse du serveur
	struct sockaddr_storage serverStorage; // Variable adresse du client à récupérer par la suite
	socklen_t addr_size, client_addr_size;
	int i;

	/*Creation de la socket UDP */
	udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

	/*Configuration des attributs de l'adresse du serveur*/
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(7891);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

	/*Association de l'adresse du serveur à la socket créée*/
	// 1er paramètre : socket 
	// 2eme paramètre : adresse du serveur
	// 3eme paramètre : taille de l'adresse
	bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

	/*Initialisation de la variable où l'on va stocker l'adresse du client*/
	addr_size = sizeof serverStorage;

	while(1){
		/*Reception de la chaine en minuscules envoyée par le client*/
		// Cette fonction retourne le nombre d'octets reçus
		// 1er paramètre : socket
		// 2eme paramètre : variable ou stocker le message
		// 3eme paramètre : taille de la variable
		// 4eme paramètre : adresse source 
		// 5eme paramètre : taille de l'adresse source
		nBytes = recvfrom(udpSocket,buffer,1024,0,(struct sockaddr *)&serverStorage, &addr_size);

		/*Conversion du message reçu en majuscules*/
		for(i=0;i<nBytes-1;i++)
			buffer[i] = toupper(buffer[i]);

		/*Envoi de la chaine au serveur en utilisant comme adresse destination l'adresse stockée serverStorage*/
		// Cette fonction retourne le nombre d'octets envoyés quand l'envoi réussit -1 sinon
		// 1er paramètre : socket
		// 2eme paramètre : message à envoyer
		// 3eme paramètre : taille du message
		// 4eme paramètre : flags (ici aucun)
		// 5eme paramètre : adresse destination
		// 6eme paramètre : taille de l'adresse
		sendto(udpSocket,buffer,nBytes,0,(struct sockaddr *)&serverStorage,addr_size);
	}

	return 0;
}


