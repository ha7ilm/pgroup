#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MYNAME "pgroup"


pid_t pgrp = 0;
int force = 0;
pid_t childpid = 0;

char usage[] = MYNAME ": tool for grouping processes to terminate together\n\n"
	"Example of usage:\n\n"
	"\tpgroup bash -c \"any_process | any_other_process | some_more_processes\"\n\n"
	"Now it is enough to kill the parent process to kill all the child processes as well.\n\n"
	"If you want to send SIGKILL to all child processes instead of forwarding the original signal caught by pgroup, add the -9 switch:\n\n"
	"\tpgroup -9 bash -c \"rtl_sdr - | csdr convert_u8_f > ~/float_iqdata\"\n\n"
;
void sig_handler(int signo)
{
	if(!force) killpg(pgrp, signo);
	else killpg(pgrp, force);
	exit(0);
}

int main(int argc, char *argv[])
{
	if(argc==1) { fprintf(stderr,"%s",usage); return 0; }
	if(argc>=2 && !strcmp(argv[1],"-9") ) { force=SIGKILL; }
	pgrp = setpgrp();

	childpid = fork();
	if(!childpid) if(execvp(argv[1+!!force],argv+1+!!force)) { fprintf(stderr, MYNAME ": can't start subprocess, error in execvp().\n"); return -1; }
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sig_handler;
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGKILL, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);

	int wait_stat;
	wait(&wait_stat);
	sig_handler(SIGTERM);
}




//original idea was to get a list of the child processes from /sys/proc and then kill them one by one:

#if 0

#include <dirent.h>
#include <sys/types.h>

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
