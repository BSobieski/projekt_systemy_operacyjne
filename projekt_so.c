#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <signal.h>
#include <sys/sem.h>
#include <sys/shm.h>

int pid0,pid1,pid2,pid3;
key_t key; 
static struct sembuf buf;
int semid,shmid;
int sleeping=1;
int dzialanie=1;
int licznik=0;

//dzielona pamiec=====================================================
void podnies(int semid, int semnum) //funkcja podnoszaca semafor
{ 
	buf.sem_num = semnum; 
	buf.sem_op = 1; 
	buf.sem_flg = 0; 
	while(semop(semid, &buf, 1) == -1)
	{ 
		//perror("Blad podczas podnoszenia semafora"); 
		//exit(1); 
	} 
} 

void opusc(int semid, int semnum) // funkcja opuszczajaca semafor
{ 
	buf.sem_num = semnum; 
	buf.sem_op = -1; 
	buf.sem_flg = 0; 
	while (semop(semid, &buf, 1) == -1)
	{ 
		//perror("Blad podczas opuszczenia semafora"); 
		//exit(1); 
	} 
}
//deklaracje kolejki komunikatów======================================
struct mymsgbuf 
{
	long    mtype;          /* typ wiadomości */
	int     request;        /* numer żądania danego działania */
	char	znak;
} msg;

struct mymsgbuf bufmess;

int open_queue( key_t keyval ) //funkcja otwierająca kolejkę i zwracająca jej numer identyfikacyjny
{
	int     qid;
          
	if((qid = msgget( keyval, IPC_CREAT | 0660 )) == -1)
		return(-1);
          
	return(qid);
}

int send_message( int qid, struct mymsgbuf *qbuf ) //funkcja wysyłająca kolejkę
{
	int result, length;
  
	/* lenght jest rozmiarem struktury minus sizeof(mtype) */
	length = sizeof(struct mymsgbuf) - sizeof(long);        
  
	if((result = msgsnd( qid, qbuf, length, 0)) == -1)
          return(-1);
          
	return(result);
}

int remove_queue( int qid ) //funkcja usuwająca kolejkę
{
	if( msgctl( qid, IPC_RMID, 0) == -1)
		return(-1);
          
	return(0);
}

int read_message( int qid, long type, struct mymsgbuf *qbuf ) //funkcja czytająca wiadomość
{
	int     result, length;
  
    /* lenght jest rozmiarem struktury minus sizeof(mtype) */
	length = sizeof(struct mymsgbuf) - sizeof(long);        
  
	if((result = msgrcv( qid, qbuf, length, type,  0)) == -1)
		return(-1);
          
	return(result);
}
//sygnaly=========================================================
void maskuj_sygnaly()
{
    int i;
    for(i = 1 ; i <= SIGRTMAX; i++) 
    	sighold(i);
}

void kontynuuj_procesy()
{
    sleeping=1;
}

void zeruj_licznik()
{
    licznik=0;
}

void uspij_procesy()
{
    sleeping=0;
}

void Zakoncz()
{
    printf("\nSystem zostanie zamkniety\n");
    kill(pid1,SIGKILL);
    kill(pid2,SIGKILL);
    kill(pid3,SIGKILL);
    dzialanie=0;
    sleep(2);
}

void Kontynuuj()
{
    if(sleeping==0)
    {
        kill(pid1,SIGUSR2);
        kill(pid2,SIGUSR2);
        kill(pid3,SIGUSR2);
        sleeping=1;
        printf("\nSystem zostanie obudzony\n");
    }
    else
        printf("\nOdebrano sygnal kontynuacji, lecz program juz jest obudzony\n");
}

void Uspij()
{
    if(sleeping!=0)
    {
        kill(pid1,SIGUSR1);
        kill(pid2,SIGUSR1);
        kill(pid3,SIGUSR1);
        sleeping=0;
        printf("\nSystem zostanie uspiony\n");
    }
    else
        printf("\nOdebrano sygnal uspienia, lecz program juz jest uspiony\n");
}

void wykonaj_sygnal( int sygnal)  //jezeli to nie macierzytsy wyslij macierzystemu
{
    if(getpid()==pid0)
    {
        switch(sygnal)
        {
        case SIGTERM:
            Zakoncz();
            break;
        case SIGALRM:
            Kontynuuj();
            break;
        case SIGTSTP:
            Uspij();
            break;
        }
        return;
    }
    else
    {
        kill(getppid(),sygnal);
    }
}

void odblokuj_sygnaly()
{
	//sygnały do odebrania
    sigrelse(SIGALRM);
    (void)sigset(SIGALRM, wykonaj_sygnal);
    sigrelse(SIGTSTP);
    (void)sigset(SIGTSTP, wykonaj_sygnal);
    sigrelse(SIGTERM);
    (void)sigset(SIGTERM, wykonaj_sygnal);
	//sygnał do komunikacji
    (void)sigset(SIGUSR1, uspij_procesy);
    (void)sigset(SIGUSR2, kontynuuj_procesy);
    (void)sigset(SIGINT, zeruj_licznik);
}
//===================================================================
int main()
{
	//wygenerowanie klucza dla semaforów
	if ((key = ftok("projekt_so.c", 'B')) == -1)
    {
        perror("ftok error");
        exit(1);
    }
    semid = semget(key, 2, IPC_CREAT|0600);
	if (semid == -1)
	{
		perror("Blad podczas tworzenia tablicy semaforów");
		exit(1);
	}

	if (semctl(semid, 0, SETVAL, (int)1) == -1)
	{
	 		perror("Blad podczas nadania wartosci semaforowi 0"); //1
			exit(1);
	}
	if (semctl(semid, 1, SETVAL, (int)0) == -1)
	{
	 		perror("Blad podczas nadania wartosci semaforowi 1"); //1
			exit(1);
	}
	key_t  msgkey;
	int qid;
	msgkey = ftok(".", 'E');
  
	/* otwieramy/tworzymy kolejkę */
	if(( qid = open_queue(msgkey)) == -1) 
	{
		perror("Otwieranie_kolejki");
		exit(1);
	}
	//zapisanie wartości procesu macierzystego oraz oedycja sygnałów by przyjmował wybrane sygnały
	pid0=getpid();
	maskuj_sygnaly();
	odblokuj_sygnaly();
	//pidy==========================================================
	pid1=fork();
	if(pid1==0) //proces 1
	{
		
			//sleep(1);
			int wprowadzanie;
			printf("Podaj tryb wprowadzania danych: \n");
			printf("1. Z klawiatury\n");
			printf("2. Z pliku\n");
			printf("3. Z /dev/urandom\n");
			scanf("%d",&wprowadzanie);
			if(wprowadzanie < 1 || wprowadzanie > 3)
			{
				printf("Nie ma takiej opcji\n");
				exit(0);
			}
			FILE * plik;
			if(wprowadzanie==2)
		    {
		    	char path[200];
		        printf("Podaj nazwe pliku pliku\n");
		        scanf("%s",path);
		        plik=fopen(path, "r");
		        if (plik == NULL)
		    	{
		    	    perror("Nie udalo sie otworzyc pliku");
		     	   exit(0);
		   		}
		    }
		    if(wprowadzanie==3)
		    {
		        plik=fopen("/dev/urandom", "r");
		        if (plik == NULL)
		    	{
		    	    perror("Nie udalo sie otworzyc pliku");
		     	   exit(0);
		   		 }
		    }
		    if(wprowadzanie==1)
		    {
		    	for(;;)
		    	{
		    		while(sleeping==0)
						pause();
			    	char pobierz[30];
					scanf("\n%s",pobierz);
					kill(getpid()+2,SIGINT);
					int i;
			       for(i=0;i<strlen(pobierz);i++)
					{
						msg.mtype   = 1;        /* typ wiadomości musi być dodatni */   
						msg.request = 1;
						msg.znak = pobierz[i];
						//printf("%c",msg.znak);
						send_message( qid, &msg );
					}
		    	}
		    }
		    else
		    {
		    	for(;;)
				{
					while(sleeping==0)
						pause();
					char pobierz;
					while(!feof(plik))
					{
						if((pobierz=fgetc(plik))!=EOF)
						{
							msg.mtype   = 1;        /* typ wiadomości musi być dodatni */   
							msg.request = 1;
							msg.znak = pobierz;
							send_message( qid, &msg );
						}
						else
						{
							if(wprowadzanie==2)
							{
								sleep(2);
								kill(pid0,SIGTERM);
							}			
						}
					}
				}
			}
		exit(0);
	}
	pid2=fork();
	if(pid2==0) //proces 2
	{
		char *shtab;
		shmid = shmget(key, 2*sizeof(char), IPC_CREAT|0600);
		if (shmid == -1)
		{
			perror("Blad podczas tworzenia segmentu pamieci wspoldzielonej");
			exit(1);
		}
		shtab = (char*)shmat(shmid, NULL, 0);
		char hexa[2];
		bufmess.mtype   = 1;        /* typ wiadomości musi być dodatni */   
		bufmess.request = 1;
		unsigned char znak;
		for(;;)
		{
			opusc(semid,0);
			while(sleeping==0)
				pause();
			read_message(qid, bufmess.mtype, &bufmess);
			znak=bufmess.znak;
			sprintf(hexa,"%02X",znak);
			//printf("%s\n",hexa);
			//fflush(NULL);
			shtab[0]=hexa[0];
			shtab[1]=hexa[1];
			podnies(semid,1);
		}
		exit(0);
	}
	pid3=fork();
	if(pid3==0) //proces 3
	{
		//signal
		char *shtab;
		shmid = shmget(key, 2*sizeof(char), IPC_CREAT|0600);
		if (shmid == -1)
		{
			perror("Blad podczas tworzenia segmentu pamieci wspoldzielonej");
			exit(1);
		}
		shtab = (char*)shmat(shmid, NULL, 0);
		char hexa[2];
		for(;;)
		{
			opusc(semid,1);
			while(sleeping==0)
				pause();
			hexa[0]=shtab[0];
			hexa[1]=shtab[1];
			printf("%s ",hexa);
			fflush(NULL);
			licznik++;
			if(licznik>=15)
			{
				licznik=0;
				printf("\n");
			}
			podnies(semid,0);				
		}
		exit(0);
	}
	//przekazanie pidow====================================================
	FILE *przekaz_pid;
	przekaz_pid=fopen("przekaz_pid", "a");
	if (przekaz_pid == NULL)
	{
		perror("Nie udalo sie otworzyc pliku");
		exit(0);
	}
	fprintf(przekaz_pid,"%d\n",pid0);
	fprintf(przekaz_pid,"%d\n",pid1);
	fprintf(przekaz_pid,"%d\n",pid2);
	fprintf(przekaz_pid,"%d\n",pid3);
	fclose(przekaz_pid);
	//======================================================================
	while(dzialanie)
		pause();
	
	//sprzątanie===============================================================
	remove_queue(qid);
	shmctl(shmget(key, 2*sizeof(char), IPC_CREAT|0600),IPC_RMID,NULL);
	semctl(semid,0,IPC_RMID);
	semctl(semid,1,IPC_RMID);
	remove("przekaz_pid");
	return 0;
}