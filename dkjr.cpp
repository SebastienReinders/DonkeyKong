#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <SDL/SDL.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "./presentation/presentation.h"

#define VIDE        		0
#define DKJR       		1
#define CROCO       		2
#define CORBEAU     		3
#define CLE 			4

#define AUCUN_EVENEMENT    	0

#define LIBRE_BAS		1
#define LIANE_BAS		2
#define DOUBLE_LIANE_BAS	3
#define LIBRE_HAUT		4
#define LIANE_HAUT		5

void* FctThreadEvenements(void *);
void* FctThreadCle(void *);
void* FctThreadDK(void *);
void* FctThreadDKJr(void *);
void* FctThreadScore(void *);
void* FctThreadEnnemis(void *);
void* FctThreadCorbeau(void *);
void* FctThreadCroco(void *);

void initGrilleJeu();
void setGrilleJeu(int l, int c, int type = VIDE, pthread_t tid = 0);
void afficherGrilleJeu();

void HandlerSIGUSR1(int);
void HandlerSIGUSR2(int);
void HandlerSIGALRM(int);
void HandlerSIGINT(int);
void HandlerSIGQUIT(int);
void HandlerSIGCHLD(int);
void HandlerSIGHUP(int);

void DestructeurVS(void *p);

pthread_t threadCle;
pthread_t threadDK;
pthread_t threadDKJr;
pthread_t threadEvenements;
pthread_t threadScore;
pthread_t threadEnnemis;
pthread_t threadCorbeau;
pthread_t threadCroco;

pthread_cond_t condDK;
pthread_cond_t condScore;

pthread_mutex_t mutexGrilleJeu;
pthread_mutex_t mutexDK;
pthread_mutex_t mutexEvenement;
pthread_mutex_t mutexScore;

pthread_key_t keySpec;

bool MAJDK = false;
int  score = 0;
bool MAJScore = true;
int  delaiEnnemis = 4000;
int  positionDKJr = 1;
int  evenement = AUCUN_EVENEMENT;
int etatDKJr;

pthread_key_t cle;

typedef struct
{
  int type;
  pthread_t tid;
} S_CASE;

S_CASE grilleJeu[4][8];

typedef struct
{
  bool haut;
  int position;
} S_CROCO;

// ------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	int retCle, retEvent, retDKJr[3], retDK, retScore, retEnemi;
	int dcd = 0;

	ouvrirFenetreGraphique();
	pthread_mutex_init(&mutexEvenement, NULL);
	pthread_mutex_init(&mutexGrilleJeu, NULL);
	pthread_mutex_init(&mutexScore, NULL);
	pthread_mutex_init(&mutexDK, NULL);
	pthread_cond_init(&condDK, NULL);
	pthread_cond_init(&condScore, NULL);

	pthread_key_create(&cle, DestructeurVS);

	retCle = pthread_create (&threadCle, NULL, (void*(*)(void*)) FctThreadCle, NULL);
	retEvent = pthread_create(&threadEvenements, NULL, (void*(*)(void*)) FctThreadEvenements, NULL);
	retDK = pthread_create(&threadDK, NULL, (void*(*)(void*)) FctThreadDK, NULL);
	retScore = pthread_create(&threadScore, NULL, (void*(*)(void*)) FctThreadScore, NULL);
	retEnemi = pthread_create(&threadEnnemis, NULL, (void*(*)(void*)) FctThreadEnnemis, NULL);
	
	while (dcd < 3)
	{
		retDKJr[dcd] = pthread_create(&threadDKJr, NULL, (void*(*)(void*)) FctThreadDKJr, NULL);
		printf ("*********************\nMAIN : J'ai lancé le JEU no %d\n", (dcd+1));
		retDKJr[dcd] = pthread_join(threadDKJr, NULL);
		dcd++;
		afficherEchec(dcd);
	}

	//afficherCle(3);
/*
	afficherCroco(11,2);
	afficherCroco(17, 1);
	afficherCroco(0, 3);
	afficherCroco(12, 5);
	afficherCroco(18, 4);

//	afficherDKJr(11, 9, 1);

	
	afficherCorbeau(10, 2);
	afficherCorbeau(16, 1);
	
	effacerCarres(9, 10, 2, 1);*/


	retCle = pthread_join(threadCle, NULL);
}

// -------------------------------------

void initGrilleJeu()
{
  int i, j;   
  
  pthread_mutex_lock(&mutexGrilleJeu);

  for(i = 0; i < 4; i++)
    for(j = 0; j < 7; j++)
      setGrilleJeu(i, j);

  pthread_mutex_unlock(&mutexGrilleJeu);
}

// -------------------------------------

void setGrilleJeu(int l, int c, int type, pthread_t tid)
{
  grilleJeu[l][c].type = type;
  grilleJeu[l][c].tid = tid;
}

// -------------------------------------

void afficherGrilleJeu()
{   
   for(int j = 0; j < 4; j++)
   {
       for(int k = 0; k < 8; k++)
         // printf("%d  ", grilleJeu[j][k].type);
       printf("\n");
   }

   printf("\n");   
}

void* FctThreadCle(void *)
{
	int i, j = 1;
	struct timespec tempsCle;
	tempsCle.tv_sec = 0;
	tempsCle.tv_nsec = 700000000;

	while (1)
	{
		//if (grilleJeu[0][1].type != DKJR)
		//{
			if (j%2 == 1)
			{
				for (i = 0; i < 4; i++)
				{
				
					if (i == 0)
					{
					//effacer grille jeu précédent :

					//affiche clé à position 1
						effacerCarres(3, 12, 2, 4);
						afficherCle(1);
					}
					else if (i == 1)
					{
					//effacer grille jeu précédent :

					//affiche clé à postion 2
						effacerCarres(3, 12, 2, 4);
						afficherCle(2);
					}
					else if (i == 2)
					{
					//effacer grille jeu précédent :

					//affiche clé à positon 3
						effacerCarres(3, 12, 2, 4);
						afficherCle(3);

					//mettre à jour grilleJeu
					}
					else
					{
					//effacer grille jeu précédent :

					//affiche clé à position 4
						effacerCarres(3, 12, 2, 4);
						afficherCle(4);

					//Et on recommence un tour de manege.
					
					}
				//dodo 0.7 sec.
					nanosleep(&tempsCle, NULL);

				//MAJ grille jeu

					pthread_mutex_lock(&mutexGrilleJeu);
					if (i == 0)
					{
						setGrilleJeu(0,1,CLE,pthread_self());
					}
					else
					{
						setGrilleJeu(0,1);
					}
					pthread_mutex_unlock(&mutexGrilleJeu);
				}
				j++;
				
			}
			else
			{
				for (i = 3; i > 0; i--)
				{
					
					if (i == 0)
					{
						//effacer grille jeu précédent :

						//affiche clé à position 1
						effacerCarres(3, 12, 2, 4);
						afficherCle(1);
					}
					else if (i == 1)
					{
						//effacer grille jeu précédent :

						//affiche clé à postion 2
						effacerCarres(3, 12, 2, 4);
						afficherCle(2);
					}
					else if (i == 2)
					{
						//effacer grille jeu précédent :

						//affiche clé à positon 3
						effacerCarres(3, 12, 2, 4);
						afficherCle(3);

						//mettre à jour grilleJeu
					}
					
					//dodo 0.7 sec.
					if (i != 3)
					{
						nanosleep(&tempsCle, NULL);
					}
					
						//MAJ grille jeu

					pthread_mutex_lock(&mutexGrilleJeu);
					if (i == 0)
					{
						setGrilleJeu(0,1,CLE,pthread_self());
					}
					else
					{
						setGrilleJeu(0,1);
					}
					pthread_mutex_unlock(&mutexGrilleJeu);
				} 
				j++;

			}
		}
		/*else
		{
			effacerCarres(3, 12, 20, 20);
		}*/
	}
//}


void* FctThreadEvenements(void *)
{
	int evt;
	struct timespec tempsEvent;
	tempsEvent.tv_sec = 0;
	tempsEvent.tv_nsec = 100000000;

	while (1)
	{
	    evt = lireEvenement();

	    switch (evt)
	    {
		case SDL_QUIT:
			exit(0);
		case SDLK_UP:
			pthread_mutex_lock (&mutexEvenement);
			evenement = SDLK_UP;
			pthread_mutex_unlock (&mutexEvenement);
			//printf("KEY_UP\n");
			break;
		case SDLK_DOWN:
			pthread_mutex_lock (&mutexEvenement);
			evenement = SDLK_DOWN;
			pthread_mutex_unlock (&mutexEvenement);
			//printf("KEY_DOWN\n");
			break;
		case SDLK_LEFT:
			pthread_mutex_lock (&mutexEvenement);
			evenement = SDLK_LEFT;
			pthread_mutex_unlock (&mutexEvenement);
			//printf("KEY_LEFT\n");
			break;
		case SDLK_RIGHT:
			pthread_mutex_lock (&mutexEvenement);
			evenement = SDLK_RIGHT;
			pthread_mutex_unlock (&mutexEvenement);
			//printf("KEY_RIGHT\n");
	    }

		    	//envoi signal
		pthread_kill(threadDKJr, SIGQUIT);

		//attend 100ml sec
		nanosleep(&tempsEvent, NULL);
	}


}

void* FctThreadDKJr(void *)
{
	struct sigaction sigAct;
	sigAct.sa_handler = HandlerSIGQUIT;
	sigaction (SIGQUIT, &sigAct, NULL);

	struct sigaction sigAct2;
	sigAct2.sa_handler = HandlerSIGINT;
	sigaction (SIGINT, &sigAct2, NULL);


	struct timespec tempsCle;
	tempsCle.tv_sec = 1;
	tempsCle.tv_nsec = 400000000;

	struct timespec tempsattente;
	tempsattente.tv_sec = 0;
	tempsattente.tv_nsec = 100000000;


	int evt;
	int oldx, oldy;
	int gains = 0;
	afficherDKJr(11, 9, 1);
	pthread_mutex_lock(&mutexGrilleJeu);
	setGrilleJeu(3,1, DKJR);
	oldx = 3;
	oldy = 1;
	etatDKJr = LIBRE_BAS;
	positionDKJr = 1;
	pthread_mutex_unlock(&mutexGrilleJeu);


	while (1)
	{

		pause();
		pthread_mutex_lock (&mutexEvenement);
		pthread_mutex_lock(&mutexGrilleJeu);
		//printf ("*******FIN DE PAUSE : ETAT : etatDKJr = %d\n", etatDKJr);

			//On aura ici tout les cas possibles et le code des events.

		switch (etatDKJr)
		{

			case LIBRE_BAS:
			//printf ("***libre pas ok : \n");
			//printf ("evenement : %d \n", evenement);
				switch (evenement)
				{
					case SDLK_LEFT:
					//printf ("J'ai poussé sur la touche GAUCHE :");
						if (positionDKJr > 1)
						{
						setGrilleJeu(3, positionDKJr);
						effacerCarres(11, (positionDKJr * 2) + 7, 2, 2);
						positionDKJr--;
						setGrilleJeu(3, positionDKJr, DKJR);
						afficherDKJr(11, (positionDKJr * 2) + 7,
						((positionDKJr - 1) % 4) + 1);
						}
						break;

					case SDLK_RIGHT:
					//printf ("J'ai poussé sur la touche DROITE :");
						if (positionDKJr < 7)
						{
						setGrilleJeu(3, positionDKJr);
						effacerCarres(11, (positionDKJr * 2) + 7, 2, 2);
						positionDKJr++;
						setGrilleJeu(3, positionDKJr, DKJR);
						afficherDKJr(11, (positionDKJr * 2) + 7,
						((positionDKJr + 1) % 4) + 1);
						}
						break;

					case SDLK_UP:
					//printf ("J'ai poussé sur la touche HAUT :");
						if (grilleJeu[2][positionDKJr].type == CORBEAU)
						{
							pthread_kill (grilleJeu[2][positionDKJr].tid, SIGUSR1);
							pthread_mutex_unlock (&mutexEvenement);
							pthread_mutex_unlock(&mutexGrilleJeu);
							printf ("AAA J'exite DKJR\n");
							effacerCarres(11, (positionDKJr * 2) + 7, 2, 2);
							pthread_exit(0);
						}
						//ATTENTION AUX ELSE IF
						if ((positionDKJr == 1) || (positionDKJr == 5))
						{
							setGrilleJeu(3, positionDKJr);
							effacerCarres(11, (positionDKJr * 2) + 7, 2, 2);
							setGrilleJeu(2, positionDKJr, DKJR);
							afficherDKJr(10, (positionDKJr * 2) + 7, 7);
							etatDKJr = LIANE_BAS;
						}
						else if (positionDKJr == 7)
						{
							//Changement d'état vers double liane BAS
							setGrilleJeu(3, positionDKJr);
							effacerCarres(11, (positionDKJr * 2) + 7, 2, 2);
							setGrilleJeu(2, positionDKJr, DKJR);
							afficherDKJr(10, (positionDKJr * 2) + 7, 5);
							etatDKJr = DOUBLE_LIANE_BAS;

						}
						else
						{
							//Il monte d'un niveau
							setGrilleJeu(3, positionDKJr);
							effacerCarres(11, (positionDKJr * 2) + 7, 2, 2);
							setGrilleJeu(2, positionDKJr, DKJR);
							afficherDKJr(10, (positionDKJr * 2) + 7,
							((positionDKJr + 1) % 4) + 1);

							pthread_mutex_unlock(&mutexGrilleJeu);

							//Il reste en l'air X secondes
							nanosleep(&tempsCle, NULL);

							pthread_mutex_lock(&mutexGrilleJeu);
							//Il descend
							setGrilleJeu(2, positionDKJr);
							effacerCarres(10, (positionDKJr * 2) + 7, 2, 2);
							setGrilleJeu(3, positionDKJr, DKJR);
							afficherDKJr(11, (positionDKJr * 2) + 7,
							((positionDKJr + 1) % 4) + 1);
						}
						break;



				} // fin switch eventments -> LIBRE_BAS

			case LIANE_BAS:
				switch (evenement)
				{
					
					case SDLK_DOWN:
					//printf ("J'ai poussé sur la touche BAS :");
					setGrilleJeu(2, positionDKJr); // On met à 0
					effacerCarres(10, (positionDKJr * 2) + 7, 2, 2);
					setGrilleJeu(3, positionDKJr, DKJR);
					afficherDKJr(11, (positionDKJr * 2) + 7,
					((positionDKJr + 1) % 4) + 1);
					etatDKJr = LIBRE_BAS;
				} //fin swich liane bas
				break;
			case DOUBLE_LIANE_BAS :
				//printf ("---++6+---DOUBLE BAS***---***\n");
				switch (evenement)
				{
					case SDLK_DOWN:
					//printf ("J'ai poussé sur la touche BAS :");
					setGrilleJeu(2, positionDKJr); // On met à 0
					effacerCarres(10, (positionDKJr * 2) + 7, 2, 2);
					setGrilleJeu(3, positionDKJr, DKJR);
					afficherDKJr(11, (positionDKJr * 2) + 7,
					((positionDKJr + 1) % 4) + 1);
					etatDKJr = LIBRE_BAS;
					break;

					case SDLK_UP:
					//printf ("J'ai poussé sur la touche HAUT :");
					setGrilleJeu(2, positionDKJr); // On met à 0
					effacerCarres(10, (positionDKJr * 2) + 7, 2, 2);
					setGrilleJeu(1, positionDKJr, DKJR);
					afficherDKJr(7, (positionDKJr * 2) + 7,
					((positionDKJr + 1) % 4) + 1);
					etatDKJr = LIBRE_HAUT;
				}
			break;

			case LIBRE_HAUT :
			//printf ("***libre pas ok : \n");
			//printf ("evenement : %d \n", evenement);
				switch (evenement)
				{
					case SDLK_LEFT:
						if (positionDKJr == 3)
						{
							if (grilleJeu[0][1].type == CLE)
							{
								printf ("bravo\n");
								setGrilleJeu(1, positionDKJr);
								effacerCarres(7, (positionDKJr * 2) + 7, 2, 2);
								positionDKJr--;
								setGrilleJeu(0, positionDKJr, DKJR);// Plus de collision possible
								afficherDKJr(6, (positionDKJr * 2) + 6, 9);

								printf ("bravo\n");
								setGrilleJeu(1, positionDKJr);
								effacerCarres(5, (positionDKJr * 2) + 7, 3, 3);
								positionDKJr--;
								setGrilleJeu(0, 1, DKJR);
								pthread_mutex_unlock(&mutexGrilleJeu); //N4a rien c
								afficherDKJr(5, (positionDKJr * 2) + 4, 10);
								// -> Enlever une case de la cage

								pthread_mutex_lock(&mutexDK);
								MAJDK = true;
								pthread_mutex_unlock(&mutexDK);
								pthread_cond_signal(&condDK);

								// -> Mettre à jour les points
								gains++;
								if (gains % 4 != 0)
								{
									//printf ("DANS LE IF \n");
								pthread_mutex_lock(&mutexScore);
								score = score +10;
								MAJScore = true;
								pthread_mutex_unlock(&mutexScore);
								pthread_cond_signal(&condScore);
								}

								nanosleep(&tempsattente, NULL);
								effacerCarres(3, 11, 3, 3);
								afficherDKJr(6, 11, 11);
								nanosleep(&tempsattente, NULL);
								effacerCarres(6, 10, 2, 3);
								afficherDKJr(11, 9, 1);

								// -> On recommence la partie
								etatDKJr = LIBRE_BAS;
								positionDKJr = 1;
								setGrilleJeu(3, 1, DKJR);
							}
							else
							{
								printf ("PAS BRAVO \n");
								// Il saute
								setGrilleJeu(1, positionDKJr);
								effacerCarres(7, (positionDKJr * 2) + 7, 2, 2);
								positionDKJr--;
								setGrilleJeu(0, positionDKJr, DKJR);// Plus de collision possible
								afficherDKJr(6, (positionDKJr * 2) + 6, 9);
								nanosleep(&tempsattente, NULL);
								effacerCarres(5, 12, 3, 2);
							//	afficherDKJr(6, 11, 12);
								nanosleep(&tempsattente, NULL);
								effacerCarres(6, 10, 2, 3);
								afficherDKJr(11, 7, 13);
								nanosleep(&tempsattente, NULL);
								effacerCarres(11, 6, 3, 3);

								// -> On perd une vie
								pthread_cancel(threadDKJr);
								// -> Le faire tomber dans les buissons
								// -> Si vies OK relancer jeu (voir insctructions)
							}
						}
						else
							{
								setGrilleJeu(1, positionDKJr);
								effacerCarres(7, (positionDKJr * 2) + 7, 2, 2);
								positionDKJr--;
								setGrilleJeu(1, positionDKJr, DKJR);
								afficherDKJr(7, (positionDKJr * 2) + 7,
								((positionDKJr - 1) % 4) + 1);	
							}
						}






		} //fin de swich etatDKJr
		evenement = AUCUN_EVENEMENT;
		pthread_mutex_unlock (&mutexEvenement);
		pthread_mutex_unlock(&mutexGrilleJeu);	
	}
}


void HandlerSIGQUIT(int)
{
	//printf ("SIGNAL \n");
}


void* FctThreadDK(void *)
{
	int cage = 4;
	struct timespec attentecage;
	attentecage.tv_sec = 1;
	attentecage.tv_nsec = 7000000;

	afficherCage(1);
	afficherCage(2);
	afficherCage(3);
	afficherCage(4);
//-------------------------------------

	while (1)	//Nous permet de continuer/recommencer la partie
	{
		pthread_mutex_lock(&mutexDK);
		while(!MAJDK)
		{
			pthread_cond_wait(&condDK, &mutexDK);
		}
		pthread_mutex_unlock(&mutexDK);

		cage --;

		switch (cage)
		{
			case 0:
				effacerCarres(2, 7, 4, 4);
				afficherRireDK();
				nanosleep(&attentecage, NULL);			
				effacerCarres(2, 7, 4, 4);
				afficherCage(1);
				afficherCage(2);
				afficherCage(3);
				afficherCage(4);
				cage = 4;

				//MAJ POINTS

								pthread_mutex_lock(&mutexScore);
								score = score +10;
								MAJScore = true;
								pthread_mutex_unlock(&mutexScore);
								pthread_cond_signal(&condScore);

			break;

			case 1 :
				//Laisser 1 coté.
				effacerCarres(2, 7, 4, 4);
				afficherCage(4);
			break;

			case 2 :
				//Laisser 2 cotés.
				effacerCarres(2, 7, 4, 4);
				afficherCage(3);
				afficherCage(4);
			break;

			case 3 :
				//Laisser 3 cotés.
				effacerCarres(1, 6, 4, 4);
				afficherCage(2);
				afficherCage(3);
				afficherCage(4);
			break;
		}
		pthread_mutex_lock(&mutexDK);
				MAJDK = false;
		pthread_mutex_unlock(&mutexDK);
	}
}


void* FctThreadScore(void *)
{
	score = 0;
	while(1)
	{
			pthread_mutex_lock(&mutexScore);
			while(!MAJScore)
			{
				printf ("/*-/*-/*-SCORE/*-/*-/*-\n");
				pthread_cond_wait(&condScore, &mutexScore);

			}
			pthread_mutex_unlock(&mutexScore);

			pthread_mutex_lock(&mutexScore);
						afficherScore(score);
					MAJScore = false;
			pthread_mutex_unlock(&mutexScore);
	}
}

void* FctThreadEnnemis(void *)
{
	  struct sigaction A;
    A.sa_handler = HandlerSIGALRM;
    A.sa_flags = 0;
    sigaction(SIGALRM, &A, NULL);

	int choix;
	int temps;
	int reste = 15;
	int retCorbeau, retCroco;

	struct timespec tempsEnemis;

	alarm(reste);
	while (1)
	{
		reste = alarm(0);
		//1. envoyer alame
		if (reste == 0)
		{
			reste = 15;
		}
		printf  ("reste = %d\n", reste);
		alarm(reste);
		//2 maj strcuture
		temps = delaiEnnemis;
		tempsEnemis.tv_sec = temps / 1000;
		tempsEnemis.tv_nsec = (temps % 1000) * 100000;
		//3.nanosleep
		nanosleep(&tempsEnemis, NULL);
		printf ("4 secondes soit %d,%d\n",tempsEnemis.tv_sec, tempsEnemis.tv_nsec);
		//4. generer monstre
		srand(time(NULL));
		choix = rand()%100;
		printf ("Nombre vaut %d\n", choix);
		if (choix % 2 == 0)
		{
			printf ("Je lance un corbeau\n");
			retCorbeau = pthread_create(&threadCorbeau, NULL, (void*(*)(void*)) FctThreadCorbeau, NULL);
			pthread_detach(threadCorbeau);
		}
		else
		{
			printf ("Je lance un croco\n");
		}
	}



}


void HandlerSIGALRM(int)
{
	if (delaiEnnemis > 2500)
	{
		printf ("Je diminue de 0.25\n");
		delaiEnnemis = delaiEnnemis - 250;
	}
	
}


void* FctThreadCorbeau(void *)
{
	  struct sigaction A;
    A.sa_handler = HandlerSIGUSR1;
    A.sa_flags = 0;
    sigaction(SIGUSR1, &A, NULL);

	printf ("voila le corbeau \n");
	struct timespec tempsCle;
	tempsCle.tv_sec = 0;
	tempsCle.tv_nsec = 700000000;

	int * place = (int*) malloc (sizeof(int));
	//mettre dans la variable
	*place = 0;

	for (int i = 0; i < 8; i++)
	{
		
		pthread_setspecific(cle, place);

		printf ("\tCase no : %d\n", *place);
		if (i > 0)
		{
			pthread_mutex_lock(&mutexGrilleJeu);
			setGrilleJeu(2, i-1);
			pthread_mutex_unlock(&mutexGrilleJeu);
		}

		pthread_mutex_lock (&mutexEvenement);
		pthread_mutex_lock(&mutexGrilleJeu);

		if (grilleJeu[2][*place].type == DKJR)
						{
							printf ("///////////////////////////\nCOLLISION\n");
							pthread_kill (threadDKJr, SIGINT);
							printf ("SIGNAL ENVOYE \n");
							pthread_mutex_unlock (&mutexEvenement);
							pthread_mutex_unlock(&mutexGrilleJeu);
							pthread_exit(0);
						}
			else
			{
				pthread_mutex_unlock (&mutexEvenement);
				pthread_mutex_unlock(&mutexGrilleJeu);
			}			


		pthread_mutex_lock(&mutexGrilleJeu);
		setGrilleJeu(2, i, CORBEAU, pthread_self());
		pthread_mutex_unlock(&mutexGrilleJeu);

		if (i % 2 == 0)
		{
			afficherCorbeau(((*place) * 2 + 8), 2);
			nanosleep(&tempsCle, NULL);
			effacerCarres(9, (*place) * 2 + 8, 2, 1);	
		}
		else
		{
			afficherCorbeau(((*place) * 2 + 8), 1);
			nanosleep(&tempsCle, NULL);
			effacerCarres(10, (*place) * 2 + 8, 1, 1);
		}
			
		(*place) ++;
	}
	pthread_mutex_lock(&mutexGrilleJeu);
	setGrilleJeu(2, 8);
	pthread_mutex_unlock(&mutexGrilleJeu);
	printf ("ZZZZ je quitte corbeau\n");
	pthread_exit(0);

}

void HandlerSIGUSR1(int sig)
{
	int * zz = (int*) malloc (sizeof(int));
	printf ("Avant le *int*\n");
	zz = (int*)pthread_getspecific(cle);
	int position = *zz;
	printf("Apres le *int* position = %d\n", position);
	if ((position % 2) == 0)
	{
		printf ("Dans le IF : Position = %d", position);
		effacerCarres(9, position * 2 + 8, 2, 1);
	}
	else
	{
		printf ("Dans le ELSE : Position = %d", position);
		effacerCarres(10, position * 2 + 8, 1, 1);
	}
	pthread_mutex_lock(&mutexGrilleJeu);
	printf ("----------------------------\nDans le MUTEX : Position = %d", position);
	setGrilleJeu(2,position);
	pthread_mutex_unlock(&mutexGrilleJeu);
	printf ("Je quitte le corbeau");
	pthread_cancel(pthread_self());
}

void DestructeurVS(void *p)
{
	free(p);
	printf ("---------------------Destruction faite \n");
}

void HandlerSIGINT(int sig)
{
	printf ("---------------------\nSIGINT RECU \n");
	effacerCarres(9, (positionDKJr * 2) + 7, 3, 2);

	if (etatDKJr != LIANE_BAS)
	{
		pthread_mutex_unlock (&mutexEvenement);
		pthread_mutex_unlock(&mutexGrilleJeu);
	}
	pthread_cancel(threadDKJr);
}