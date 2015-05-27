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
* D'après les résultats de plusieurs essaies, le programme s'arrete juste après l'ouverture du fichier car.dat, et par fois ça entraine le blocage de système. Donc on pense que le problème vient de la taille de fichier, peut-etre l'ouverture de ce gros fichier épuise les ressources de la carte. Donc on déminue la taille de car.bin
* UDP3 client.c J'ai changé nb=10, et dans le fichier car.dat il n'y a que 9600 échantions. car_original.dat est le fichier original. Et j'ai changé le type de tableau de int à float, aussi dans la lecture de fichier. J'ai commenté des lignes qui créent et démarent le thread de réception dans le main.
* Dans cette version, la carte marche une ou deux fois, elle peut envoyer toutes les données(sais pas si elles sont bonnes), mais après il y a une blocage de système, je peux plus la controler; ou elle se redémare automatiquement.
* UDP3 client_udp3_test est la version sur laquelle je suis en train de travailler. Le protocole transmet le fichier signal1.bin; je définis nb=50, et utlise 'fread' au lieu de 'fscanf', aussi 'rb' au lieu de 'r'. Dans ce cas-là, il a réussi une fois, s'arrete au milieu quelle que fois, en blocage quelle que fois. En fait, je suspecte la manière dont on transmet les exécutables.
* En effet, c'est la partie où on ouvre un ficher, stocke les données dans un buffer qui nous pose toujours les problèmes.
* A voir si on a besoin d'utiliser un tableau dynamique au lieu d'un tableau statique.


Log 24 mai
* Hier j'ai pensé que le problème vient de la manière dont on transmet le fichier. Cette hypothèse est basée sur un résultat suivant:
  1) La carte est sous état initial, on transmet les deux fichiers: client_udp3_test et signal1.bin
  2) Il marche pas, il affiche une error(ça peut etre une des trois errors), et la meme error nous arrive lors de plusieurs lancements.
  3) On redémarre la carte ou bien supprime les anciens fichiers, retransmet les deux fichiers et les lance
  4) Différentes errors apparaissent.
* Donc je cherche sur Internet et trouve que wget marche en mode binaire par défaut. Et basé sur le fait que, meme si les fichiers sont mal transmis, les résuiltats de l'exécutions deveront etre identiques. De ce fait, on déduit que ce problème ne vient pas de la manière dont on transmet les fichiers.
* Et après on se concentre sur la lecture de fichier et buffering des données.
* Avec [50][960] ça n'a pas pu marcher, s'arretait avant buffering des données.
* Juste pour essayer, je met la taille de tableau [50][10], ça a très bien marché, sans problème.
* Donc maintenant on a déterminé d'où vient le problème.
* La demande d'une espace de mémoire 50*960*int est trop grande pour la carte. Donc pour essayer la limite, je l'ai changé à [50][96], c'est à dire il y a 50 trame, chaque trame contient 96 échantillons. Mais je constate prèsque une faille parmis une vintaine exécutions. Je change la méthode de compilation 
* "arm-uclinuxeabi-gcc -o hello hello.c -I /home/uestclx/projetS4/linux-cortexm-1.12.0/A2F/root/usr/include -mcpu=cortex-m3 -mthumb -L /home/uestclx/projetS4/linux-cortexm-1.12.0/A2F/root/usr/lib -pthread"
* Et je constate plus ce problème, par ailleur, la compilation est plus vite.
* Si on met la taille de tableau statique [50][96], soit on envoit 96 échantillons par paquet; soit chercher comment envoyer plusieurs trames dans un seul paquet. Je suis en train de chercher la dexième solution.
* A voir le tableau dynamique.
* Si on utilise un tableau statique qui est plus petit que la taille de fichier, il faut buffering des données plusieurs fois, par exemple, pour un fichier qui a 9600 valeurs, il faut buffering deux fois un tableau de taille [50][96]. A voir comment gérer ce processus.


Log 24 Mai
* UDP2_dat est la répertoire que j'ai crée ce soir.
* Avec ce client et serveur, il peut transmettre correctement les données dans le fichier car.dat.
* Mais il ne peut transmettre que 50 paquets.
* Et il lit une trame, il l'envoit; une autre trame, l'envoit. Bien-sur qu'il ne respecte pas la contrainte de temps réel. Mais, fin, on ne travaille pas avec cette version-là.
* A remarquer:
* coté client
* Le fichier est à lire en mode 'r'
* Le buffer coté client est de type 'long'
* use fscanf, et l'attribut doit etre cohérent avec le type de buffer, ici '%ld'
* sizeof(long)
* j'ai suprime usleep(5000), juste pour voir le résultat, pas grand changement, pas d'error
* coté serveur:
* buffer de type 'int'
* fprintf avec l'attribut '%d'
* sizeof(int)
* 
La principe c'est que, le format que le serveur utilise vaut la moitié de celui que le client utilise.
Puis que le meme type de données dans la carte et sur le PC ne vaut pas la meme espace.

Log 25 Mai
* Apply the same modifications to the UDP3 version which contains the dynamic table
* And as the result, we can correctly send 50*960 values from the client to the server
* The server can receive correctly these values and do the noise reduction, then send back the results to the client
* The client receives these noise-reduced samples and writes them into a file.
* The memory allocation takes about 2s, and the rest of the process runs very fast, just a blink of an eye according to my observation
* To continue: how to send a file continuously; and push the datas toward the exit.
* 
Log 27 Mai
* Mise à jour du document tests.odt 
* J'ai testé la dernière version d'udp3, ça marche (j'ai mis le fichier de sortie dans le dossier) (voir remarques dans tests.odt)
* J'ai ajouté du code au client pour calculer le temps d'envoi et de récéption, mais on trouve toujours 0 (voir tests.odt)
* J'ai mis cette version du client dans le dossier UDP3_dyn_long
