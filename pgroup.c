#include <stdio.h>
//#include <dirent.h>
#include <unistd.h>
//#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#define MYNAME "pgroup"

#if 0

typedef struct child_s
{
	pid_t pid;
	struct child_s* next;
} child_t;

child_t* childs = NULL;

int only_numbers_in_string(char* str) 
{ 
	for(;*str;str++) 
		if(!(*str>='0'&&*str<='9')) 
			return 0;
	return 1; 
} 

#define PATHSIZE 256
#define FDATASIZE 10*1024

int getppid_from_status(char* data, int size)
{
	for(;;)
	{
		for(int i=0;i<size;i++) if(data[i]=='\t'
	}
}

int get_all_childs()
{
	int num_childs = 0;

	//free linked list of childs
	child_t* pnext;
	if(childs) for(child_t* p=childs; p; p=pnext) { pnext=p->next; free(p); } 

	pid_t mypid = getpid();
	DIR* procdir = opendir("/proc/");
	if(!procdir) { fprintf( stderr, MYNAME ": cannot open /proc/\n"); return -1;} 
	struct dirent* subdir;
	while((subdir = readdir(procdir)) != NULL) if(subdir->d_type == DT_DIR && only_numbers_in_string(subdir->d_name)) 
	{
		char path[PATHSIZE] = "/proc/";
		char fdata[FDATASIZE];
		strncat(path,subdir->d_name,PATHSIZE-6);
		strncat(path,"/status",PATHSIZE-strlen(path));
		FILE* statfile = fopen(path,"r");
		fread(fdata, sizeof(char), FDATASIZE, statfile);
		
		
	}

	return num_childs;
}


int main(int argc, char *argv[])
{
	get_all_childs();
}

#endif

pid_t pgrp = 0;
int force = 0;
pid_t childpid = 0;

char usage[] = MYNAME ": tool for grouping processes to terminate together\n";

void sig_handler(int signo)
{
	if(!force) killpg(pgrp, signo);
	else killpg(pgrp, SIGTERM);
	/* if(force) for(;;)
	{
		usleep(50000);
		if(!kill(childpid, 0)) { killpg(pgrp, SIGTERM); fprintf(stderr,"a"); }
		else break;
	} */
	exit(0);
}

int main(int argc, char *argv[])
{
	if(argc==1) { fprintf(stderr,"%s",usage); return 0; }
	if(argc>=2 && ( !strcmp(argv[1],"-f") || !strcmp(argv[1],"--force")) ) { fprintf(stderr, MYNAME ": force\n"); force=1; }
	pgrp = setpgrp();

	childpid = fork();
	if(!childpid) execvp(argv[1+force],argv+1+force);
    struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sig_handler;
	sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGKILL, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

	pause();
}
