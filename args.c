#include "headers.h"
extern int proc_no;
void get_args(char* args[],char* cur)
{
	int l=0;
	while(cur!=NULL)
	{
		args[l++] = cur;
		cur = strtok(NULL,"\t\n ");
	}	
	args[l]=NULL;
	return;
}

int check_ptype(char* copy[])
{
	int type = 0;
	int i=0;
	while(copy[i]!=NULL)
	{
		if(strcmp(copy[i++],"&")==0)
		{
			type=1;
			break;
		}
	} 
	if(type==1)
	{
		if(copy[i]!=NULL)
			type=0;
	}
	return type;	
}



void exec_pwd(char *cur[])
{
	char pwd[300];	
	char * ret = getcwd(pwd,300);

	if(ret==NULL)
	{
		perror("pwd");
		return;
	}	
	printf("%s\n",pwd);
	return;
}

void exec_echo(char *args[])
{
	if(args[1]==NULL)
	{
		perror("No argument");
		return;
	}	
	int k=1;
	while(args[k]!=NULL)
		printf("%s ",args[k++]);
	printf("\n");
	fflush(stdout);
	return;
}

void sort_proc(int proc_arr[])
{
	while(1)
	{
		int flag=0;
		for(int i=0;i<proc_no-1;i++)
		{
			if(proc_arr[i]>proc_arr[i+1])
			{
				int temp = proc_arr[i+1];
				proc_arr[i+1] = proc_arr[i];
				proc_arr[i] = temp;
				flag=1;
			}	
		}
		if(flag==0)
			break;	
	}

	return;
}

void get_name(char name[],int procid)
{
	char dir[1000]="/proc/";
		char pno[1000];
		snprintf(pno,100,"%d",procid);
		strcat(dir,pno);
		strcat(dir,"/cmdline");	
		int fd = open(dir,O_RDONLY);
		if(fd<0)
		{
			perror("error is readind file");
			return;
		}	
		read(fd,name,1000);
		close(fd);
	return;	
}	

void exec_jobs(int proc_arr[])
{
	
	sort_proc(proc_arr);
	int count=1;
	for(int i=0;i<proc_no;i++)
	{
		if(proc_arr[i]<0)
			continue;
		int j = 1;
		char dir[1000]="/proc/";
		char pno[1000];
		snprintf(pno,100,"%d",proc_arr[i]);
		strcat(dir,pno);
		strcat(dir,"/status");	
		int fd = open(dir,O_RDONLY);
		if(fd<0)
		{
			continue;
		}	
		char out[100000];
		read(fd,out,100000);
		char name[1500];
		get_name(name,proc_arr[i]);
		char* token = strtok(out,"\t\n ");

		while(strcmp(token,"State:")!=0&&token!=NULL)
			token = strtok(NULL,"\t\n ");
		token = strtok(NULL,"\t\n ");	

		printf("[%d] ",count++);
		if(strcmp(token,"T")==0)
		{
			printf("Stopped ");
		}
		else
		{
			printf("Running ");
		}
		printf("%s %d\n",name,proc_arr[i]);
		close(fd);
	}	
	return;
}

int get_jobs(int proc_arr[],int jobno)
{
	sort_proc(proc_arr);
	int reqjob=-1;
	
	for(int i=0;i<proc_no&&jobno>0;i++)
	{
		if(proc_arr[i]>0)
		{
			reqjob = proc_arr[i];
			jobno--;
		}
	}
	if(jobno>0)
		reqjob = -1;
	return reqjob;
}

void add_fore(int p_id,int proc_arr[])
{
	printf("%d\n",p_id);
	for(int i=0;i<proc_no;i++)
	{
		if(proc_arr[i]==p_id)
		{
			proc_arr[i]=-3;
			break;
		}	
	}
	return;
}