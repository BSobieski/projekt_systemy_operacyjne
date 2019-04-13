#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

int main()
{
	int pid0,pid1,pid2,pid3;
	//pobranie PIDów procesów utworzonych w programie projekt_so.c
	FILE *przekaz_pid;
	przekaz_pid=fopen("przekaz_pid", "r");
	if (przekaz_pid == NULL)
	{
		perror("Nie udalo sie otworzyc pliku");
		exit(0);
	}
	fscanf(przekaz_pid,"%d\n",&pid0);
	fscanf(przekaz_pid,"%d\n",&pid1);
	fscanf(przekaz_pid,"%d\n",&pid2);
	fscanf(przekaz_pid,"%d\n",&pid3);
	fclose(przekaz_pid);
	//================================================================
	int wybor;
	int wybor2;
	int sygnal;
	while(1)
	{
		printf("Wybierz sygnal:\n");
		printf("1.Zapauzuj\n");
		printf("2.Kontynuuj\n");
		printf("3.Zakoncz\n");
		scanf("%d",&wybor);
		if(wybor>3||wybor<1)
			printf("Nie ma takiej opcji\n");
		else
		{
			//wybór akcji=============================================
			switch(wybor)
			{
				case 1:
				sygnal=SIGTSTP;
				break;
				case 2:
				sygnal=SIGALRM;
				break;
				case 3:
				sygnal=SIGTERM;
				break;
			}
			printf("Wybierz proces:\n");
			printf("1.Macierzysty\n");
			printf("2.Potomny 1\n");
			printf("3.Potomny 2\n");
			printf("4.Potomny 3\n");
			scanf("%d",&wybor2);
			if(wybor2>4||wybor2<1)
				printf("Nie ma takiej opcji\n");
			else
			{
				//wybór procesu=====================================
				switch(wybor2)
				{
					case 1:
					kill(pid0,sygnal);
					break;
					case 2:
					kill(pid1,sygnal);
					break;
					case 3:
					kill(pid2,sygnal);
					break;
					case 4:
					kill(pid3,sygnal);
					break;
				}
			}
		}
		
	}
	return 0;
}