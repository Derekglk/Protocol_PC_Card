# Protocol_PC_Card

Younes,

To use github, we only need several commands

1. git clone URL	//This is to copy the projet to local repository. for example, git clone https://github.com/uestclx/Protocol_PC_Card.git

2. git status		//To know the differences between the codes online and the local code

3. git add file     //This is to add a file to the synchronisation list

4. git commit -m "message"		//This is to save the changement, but not synchronized yet with the server

5. git push			//This is to push the local commited files to the server

6. git pull			//This is to download the online files, same function with "svn update"

* And to note that all these commands except the "clone", have to be runned under the git repository.
* The connection is a little bit slow, be patient ;)
* It seems like that everytime before "push", we need to "add" then "commit", and we can "push".

Log 23 mai
* UDP2 tous les fichiers sont originals sauf que j'ai changé l'adresse IP dans le fichier serveur.c à 192.168.0.100
* UDP2 udp2.c est ta version qui marche très bien
* UDP3 client.c J'ai changé nb=10, et dans le fichier car.dat il n'y a que 9600 échantions. car_original.dat est le fichier original. Et j'ai changé le type de tableau de int à float, aussi dans la lecture de fichier. J'ai commenté des lignes qui créent et démarent le thread de réception dans le main.
* Dans cette version, la carte marche une ou deux fois, elle peut envoyer toutes les données(sais pas si elles sont bonnes), mais après il y a une blocage de système, je peux plus la controler; ou elle se redémare automatiquement.
* UDP3 client_udp3_test est la version sur laquelle je suis en train de travailler. Le protocole transmet le fichier signal1.bin; je définis nb=50, et utlise 'fread' au lieu de 'fscanf', aussi 'rb' au lieu de 'r'. Dans ce cas-là, il a réussi une fois, s'arrete au milieu quelle que fois, en blocage quelle que fois. En fait, je suspecte la manière dont on transmet les exécutables.
* En effet, c'est la partie où on ouvre un ficher, stocke les données dans un buffer qui nous pose toujours les problèmes.
* A voir si on a besoin d'utiliser un tableau dynamique au lieu d'un tableau statique.
