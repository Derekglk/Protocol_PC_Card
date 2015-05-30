/************************** UDP CLIENT CODE *********************************
L'utilisateur tape une chaine de caractère dans le terminal où l'on a lancé
le client. Le client la met dans un paquet et l'envoie au serveur. Il reçoit
ensuite la chaine de caractère en majuscule que lui renvoie le serveur.

Etapes:
On crée d'abord la socket UDP
On configure tous les attributs de la structure contenant l'adresse du serveur
On récupère la chaine de caractères entrée par l'utilisateur 
On reçoit la chaine en majuscule envoyée par le serveur
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>  //Bibliothèque pour créer la socket UDP
#include <netinet/in.h>  //Bibliothèque pour créer et stocker les adresses


int main(){
	
	int clientSocket, nBytes;
	char buffer[1024]; // Variable pour stocker la chaine de caractère entrée par l'utilisateur
	struct sockaddr_in serverAddr; // Variable adresse du serveur
	socklen_t addr_size; // Variable taille de l'adresse du serveur

	/*Creation de la socket UDP*/
	// 1er paramètre : Famille d'adresse PF_INET = adresses IP
	clientSocket = socket(PF_INET, SOCK_DGRAM, 0);
	

	/*Configuration des attributs de la variable serverAddr */
	// Famille d'adresses AF_INET = adresses IP
	// Utiliser AF_INET6 pour les IPv6 ...	
	serverAddr.sin_family = AF_INET; 
	// Numéro de port
	serverAddr.sin_port = htons(7891);
	// Affectation de l'adresse IP  
	// La fonction inet_addr convertit une chaine de caractère contenant l'adresse IP en décimal en un format compatible avec la structure IN_ADDR
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 

	/*Initialisation de l'attribut sin_zero par 0 (On remplit tous les octets alloués à la variable à 0)*/
	// 1er paramètre : la variable à remplir
	// 2eme paramètre : la valeur à mettre dans la variable 
	// 3eme paramètre : nombre d'octets alloués à la vaiable à remplir par la valeur  
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

	/*Stockage de la taille de l'adresse*/
	addr_size = sizeof serverAddr;

	while(1){
		printf("Type a sentence to send to server:\n");

		/*Récupération la chaine de caractère dans buffer*/
		fgets(buffer,1024,stdin); 
		printf("You typed: %s",buffer);

		/*Calcul de la taille de la chaine de cactères y compris \0 */
		nBytes = strlen(buffer) + 1; 

		/*Envoi de la chaine au serveur*/
		// Cette fonction retourne le nombre d'octets envoyés quand l'envoi réussit -1 sinon
		// 1er paramètre : socket
		// 2eme paramètre : message à envoyer
		// 3eme paramètre : taille du message
		// 4eme paramètre : flags (ici aucun)
		// 5eme paramètre : adresse destination
		// 6eme paramètre : taille de l'adresse
		sendto(clientSocket,buffer,nBytes,0,(struct sockaddr *)&serverAddr,addr_size);

		/*Reception de la chaine en majuscules envoyée par le serveur*/
		// Cette fonction retourne le nombre d'octets reçus
		// 1er paramètre : socket
		// 2eme paramètre : variable ou stocker le message
		// 3eme paramètre : taille de la variable
		// 4eme et 5eme paramètres : paramètres optionnels à remplir dans le cas où l'on souhaite stocker l'adresse source ( 4 = l'adresse et 5 = sa taille)
		nBytes = recvfrom(clientSocket,buffer,1024,0,NULL, NULL);

		printf("Received from server: %s\n",buffer);

	}

	return 0;
}
