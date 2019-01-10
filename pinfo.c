#include "headers.h"
void exec_pinfo(char *cur[])
{
	int i = 1;
	char dir[1000]="/proc/";
	char pno[1000];

	if(cur[1]!=NULL)
		strcat(dir,cur[1]);
	else
	{
		snprintf(pno,100,"%d",getpid());
		strcat(dir,pno);
	}
	strcat(dir,"/status");	
	int fd = open(dir,O_RDONLY);
	if(fd<0)
	{
		perror("error is readind file");
		return;
	}	
	char out[100000];
	read(fd,out,100000);
	char* token = strtok(out,"\t\n ");

	while(strcmp(token,"State:")!=0&&token!=NULL)
		token = strtok(NULL,"\t\n ");
	token = strtok(NULL,"\t\n ");
	printf("State %s\n",token);

	while(strcmp(token,"Pid:")!=0&&token!=NULL)
		token = strtok(NULL,"\t\n ");
	token = strtok(NULL,"\t\n ");
	printf("Pid %s\n",token);

	while(strcmp(token,"VmSize:")!=0&&token!=NULL)
		token = strtok(NULL,"\t\n ");
	token = strtok(NULL,"\t\n ");
	printf("Virtual Memory %s\n",token);

	dir[strlen(dir)-7]='\0';
	strcat(dir,"/exe");
	char execut[10000];
	char *a = realpath(dir,execut);
	
	printf("Executable %s\n",a);
	return;
}

