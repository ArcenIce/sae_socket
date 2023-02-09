# SAE PT3N

#### Projet de SAE BUT2, création d'un jeu du pendu en réseau via les sockets en C sans l'aide de librairies externes.

## Sujet

### PT3N version 0 :
Le client incarne le joueur qui doit deviner le mot :\
&nbsp;• se connecte au serveur  
&nbsp;• attend le message "start x" du serveur qui indique que la partie peut commencer avec un mot comportant x lettres  
&nbsp;• affiche le mot (x tirets pour commencer)  
&nbsp;• permet à l'utilisateur de choisir une lettre, l'envoie au serveur  
&nbsp;• attend le message du serveur qui indique si la lettre fait partie du mot ou non ou alors si il faut rejouer dans le cas où la lettre a déjà été choisie. Ce message doit aussi permettre d’indiquer si la partie est gagnée ou perdue.  
&nbsp;• En fonction des cas, la partie se termine ou alors le client boucle sur une nouvelle lettre  

Le serveur incarne le joueur qui fait deviner le mot :  
&nbsp;• accepte la connexion d'un client  
&nbsp;• envoie le message "start x" (x = nombre de lettres dans le mot)  
&nbsp;• attend le message indiquant la lettre du client et vérifie par rapport au mot à deviner, met à jour les données pertinentes pour la gestion de la partie.  
&nbsp;• répond au client pour lui indiquer ce qu’il doit faire ensuite  
&nbsp;• remarque : une seule connexion est attendue pour ensuite jouer la partie qui, une fois terminée, permet au serveur de boucler en attente d’une nouvelle connexion car il ne quitte jamais, … c’est un serveur  
&nbsp;• il fait deviner toujours le même mot dans cette version car il n’y a pas d’interaction avec la console (en dehors d’informations de log dans la console ou dans un fichier)

<p align="center">
  <img src="https://cdn.discordapp.com/attachments/1072527874462072833/1073159091922403340/image.png">
</p>

### PT3N version 1 (après avoir terminé et testé V0) :
Dans cette version, 2 joueurs vont se connecter pour trouver le même mot.
Attention à séparer vos fichiers sources de la V0 et de la V1 pour continuer !

&nbsp;• Le premier client se connecte et attend l’accusé de réception du serveur qui lui indique de patienter pour trouver le mot de x lettres  
&nbsp;• Le deuxième client se connecte et attend l’accusé de réception du serveur qui lui indique de commencer pour trouver un mot de x lettres  
&nbsp;• le serveur fait donc jouer en premier le 2e client connecté puis alterne avec le 1er client  
&nbsp;• le premier des 2 clients a avoir trouver le mot à gagné  
&nbsp;• le serveur ne gère qu’une partie à la fois  

<p align="center">
  <img src="https://cdn.discordapp.com/attachments/1072527874462072833/1073159728760356894/image.png">
</p>

### PT3N version 2 (après avoir terminé et testé V0) :
Dans cette version, les deux joueurs sont des clients. Le serveur ne joue donc pas mais fait transiter
les messages entre les 2 clients.

&nbsp;• Le premier client qui se connecte au serveur incarne le joueur qui fait deviner le mot  
&nbsp;• Le deuxième client qui se connecte doit deviner le mot  
&nbsp;• Le serveur gère les communications à tour de rôle entre les 2 joueurs mais sans intervenir dans la partie  
&nbsp;• le serveur ne gère qu’une partie à la fois  

<p align="center">
  <img src="https://cdn.discordapp.com/attachments/1072527874462072833/1073159946767695922/image.png">
</p>

### PT3N version 3 (après avoir terminé et testé V2) :
Même principe que la version 2 mais le serveur doit gérer plusieurs parties donc il :

&nbsp;• est perpétuellement en attente de connexion pour de nouvelles parties  
&nbsp;• gère les échanges des parties en cours  

### PT3N version 4 (après avoir terminé et testé V2) :
Même principe que la version 2 mais le serveur doit mettre directement les 2 joueurs connectés en communication pour qu’ils échangent leurs messages sans passer par le serveur.  
Ce mécanisme doit du coup permettre au serveur d’être perpétuellement en attente de nouvelles
parties.


## L'équipe

- [@Pierrecrp1](https://github.com/Pierrecrp1)  
- [@Tom6213](https://github.com/Tom6213)  
- [@Zyker465](https://github.com/zyker465)  
- [@ArcenIce](https://github.com/ArcenIce)  
