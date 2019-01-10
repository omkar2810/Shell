#include "headers.h"
char home[305];
int proc_arr[1000]; 
int proc_no=0;
pid_t pid;
int pip_in=0;
int grp_no;
int status;
int killed =0;
int stopped = 0;
void handler()
{
	clk = 0;	
	killed = 1;
	return;
}

void stop_proc()
{
	stopped = 1;
	return;
}

void hand()
{
	wait(NULL);
	return;
}
void execute_cmd(char* cmd[])
{

	int x = execvp(cmd[0],cmd);
	if(x<0)
		printf("command not found\n");
	return;
}

void exec_cd(char *cur[])
{
	int x=0;
	if(cur[1]==NULL)
		x=chdir(home);
	else
		 x = chdir(cur[1]);
	if(x<0)
	{
		perror("cd");
		return;
	}	
	return;
}

void exec_main(char * args[]);

int call=0;

void redir(char* args[],int start)
{
	int i=start;
	for(i=start;args[i]!=NULL;i++)
	{
		if(strcmp(args[i],"<")==0)
		{
			int fd = open(args[i+1],O_RDONLY);
			if(fd<0)
			{
				perror("can't open file");
			}	
			if(dup2(fd,0)!=0)
			{
				perror("dup failed");
			}	
			close(fd);
		}
		else if(strcmp(args[i],">")==0||strcmp(args[i],">>")==0)
		{
			int fd;
			if(strcmp(args[i],">")==0)
				fd = open(args[i+1],O_WRONLY|O_CREAT|O_TRUNC,0644);
			else
				fd = open(args[i+1],O_APPEND|O_CREAT|O_WRONLY,0644);

			if(fd<0)
			{
				perror("can't open file");
			}

			if(dup2(fd,1)!=1)
			{
				perror("dup failed");
			}	
			// fprintf(stderr, "error with something %d in call %d \n",i,call);
			close(fd);						
		}
		else if(strcmp(args[i],"|")==0)
			break;
	
	}	
	call++;
	return ;
}

int pipe_cmd(char* args[],int start)
{
	int file_des[2];

	if(pipe(file_des)!=0)
	{
		perror("pipe error");
	}

	int i = start+1;

	for(;args[i]!=NULL;i++)
	{
		if(strcmp(args[i],"|")==0)
		{
			if(dup2(file_des[1],STDOUT_FILENO)!=1)
			{
				perror("dup failed");
			}	
			close(file_des[1]);
			pip_in = file_des[0];
			return i+1;
		}	
	}

	// printf("%d\n",i);
	return i;
}

int main()
{
	killed = 0;
	stopped = 0;
	signal(SIGINT,SIG_IGN);
	signal(SIGTSTP,SIG_IGN);

	struct utsname data;
	grp_no = getpgrp();
	char uid[205];
	char pwd[3005];
	char *hm = getcwd(home,300);
	int x = getlogin_r(uid,200);    //get username
	uname(&data);				   //get system name 	
	int running = 1;
	int status;
	int std_in = dup(0);
	int std_out = dup(1);

	while(running==1)
	{
		dup2(std_in,0);
		dup2(std_out,1);
		

		char* args[100];
		char* semi[100];
		char cmd[100];
		char * ret = getcwd(pwd,3000);
		char* cur;
		char* full;
		int j;
		int m;
		
		printf("%s@%s:",uid,data.nodename);
	
		for(m=0;m<strlen(home);m++)
		{
			if(pwd[m]!=home[m])
			{
				m=0;
				break;
			}
		}

		if(m)	
			printf("~");
		for(;m<strlen(pwd);m++)
			printf("%c",pwd[m]);
		printf("$ ");
		
		fgets(cmd,100,stdin);
		sort_proc(proc_arr);
		for(j=0;j<proc_no;j++)   //check for terminating processes
		{
			if(proc_arr[j]<0)
				continue;
			if(waitpid(proc_arr[j], &status, WNOHANG)==proc_arr[j])
			{	
  				printf("Process with PID %d exited \n",proc_arr[j]);
  				proc_arr[j] = -3;
  			}	
  		}
  		
		full = strtok(cmd,";\n");
		int in=0;

		while(full!=NULL) //tokenise based on semicolon 
		{
			semi[in++]=full;
			full = strtok(NULL,";\n");
		}

		semi[in]=NULL;
		in=0;

		while(semi[in]!=NULL)
		{
			if(dup2(std_in,0)!=0)
			{
				perror("dup in");
			}	
			dup2(std_out,1);
			
			cur = strtok(semi[in++],"\t\n ");
			if(cur==NULL)	
				continue;
			int start = 0;
			get_args(args,cur);
			while(args[start]!=NULL)
			{	
				dup2(pip_in,STDIN_FILENO);
				dup2(std_out,STDOUT_FILENO);
			
				int new_start = start;
				start = pipe_cmd(args,start);
				redir(args,new_start);
				char* temp_args[100];
				// printf("new: %d\n",new_start);			
		   		
				for(int i=new_start;args[i]!=NULL;i++)
				{
					if((strcmp(args[i],">")!=0)&&(strcmp(args[i],">>")!=0)&&(strcmp(args[i],"<")!=0)&&(strcmp(args[i],"|")!=0))
					{
						temp_args[i-new_start] = args[i];
						temp_args[i-new_start+1]=NULL;
					}
					else
						break;	
				}	
				// printf("%s\n",temp_args[0]);
				exec_main(temp_args);
			}	
		}	
			
	}	
	close(std_out);
	close(std_in);

	return 0;
}



void exec_main(char* args[])   //func to execute commands
{
	if(strcmp(args[0],"cd")==0)
	{
		exec_cd(args);
	}
	else if(strcmp(args[0],"pwd")==0)
	{
		exec_pwd(args);
	}	
	else if(strcmp(args[0],"ls")==0)
	{		
		exec_ls(args);
	}	
	else if(strcmp(args[0],"echo")==0)
	{
		exec_echo(args);
	}	
	else if(strcmp(args[0],"clock")==0)
	{
		if(args[1]==NULL||args[2]==NULL)
		{
			perror("Less no of arguments");
			return;
		}
		pid = fork();

		if(pid==0)
		{
			show_time(args);
			exit(0);
		}
		else
		{
			int st;
			while(wait(&st)!=pid);
		}
	}	
	else if(strcmp(args[0],"jobs")==0)
	{
		if(args[1]!=NULL)
		{
			perror("jobs");
		}	
		exec_jobs(proc_arr);
	}	
	else if(strcmp(args[0],"kjob")==0)
	{
		if(args[1]==NULL||args[2]==NULL)
		{
			perror("use proper format:kjob <jobno> <signal>");
			return;
		}	
		int p_name = get_jobs(proc_arr,atoi(args[1]));
		int sig_name = atoi(args[2]);
		if(p_name<0)
		{
			perror("Invalid Job");
			return;
		}	
		printf("%d %d\n",p_name,sig_name);
		if(kill(p_name,sig_name)<0)
			perror("kill");
	}	
	else if(strcmp(args[0],"pinfo")==0)
	{
		exec_pinfo(args);
	}
	else if(strcmp(args[0],"setenv")==0)
	{
		if(args[1]==NULL && args[3]!=NULL)
		{
			perror("setenv");
			return;
		}
		int err;
		if(args[2]==NULL)
			err=setenv(args[1],"",1);
		else
			err=setenv(args[1],args[2],1);
		if(err<0)
		{
			perror("error setenv");
		}	
		return;
	}	
	else if(strcmp(args[0],"unsetenv")==0)
	{
		
		if(args[1]==NULL)
		{
			perror("error unsetenv");
			return;
		}	
		int err = unsetenv(args[1]);
		return;	
	}
	else if(strcmp(args[0],"fg")==0)
	{
		if(args[1]==NULL)
		{
			perror("fg");
			return;
		}
		int job_id = get_jobs(proc_arr,atoi(args[1]));	
		if(job_id<0)
		{
			perror("Invalid Job");
			return;
		}
		killed =0 ;
		stopped = 0;
		add_fore(job_id,proc_arr);
		kill(job_id,SIGCONT);
		
		signal(SIGINT,handler);
		signal(SIGTSTP,stop_proc);	
		while(waitpid(job_id,&status,WNOHANG)!=job_id&&killed==0&&stopped==0);
		if(stopped==1)
		{
			setpgid(job_id,job_id);
			proc_arr[proc_no++] = job_id;
			kill(job_id,SIGSTOP);
			printf("Stopped %d\n",job_id);
		}
		if(killed==1)	
		{
			printf("\nkilled %d\n",job_id);
			kill(job_id,SIGCONT);
			kill(job_id,SIGKILL);
			wait(NULL);
		}
		killed =0 ;
		stopped = 0;
		return;
	}	
	else if(strcmp(args[0],"bg")==0)
	{
		if(args[1]==NULL)
		{
			perror("bg");
			return;
		}
		int job_id = get_jobs(proc_arr,atoi(args[1]));	
		if(job_id<0)
		{
			perror("Invalid Job");
			return;
		}
		setpgid(job_id,job_id);
		kill(job_id,SIGCONT);
		return;
	}
	else if(strcmp(args[0],"overkill")==0)
	{
		if(args[1]!=NULL)
		{
			perror("Invalid Command , Use: overkill");
		}
		for(int i=0;i<proc_no;i++)
		{
			if(proc_arr[i]>0)
			{
				// kill(proc_arr[i],SIGCONT);
				kill(proc_arr[i],9);
				wait(NULL);
			}	
		}
	}	
	else if(strcmp(args[0],"quit")==0)
	{
		exit(0);
	}
	else 
	{
		int type=0;
		char copy[500];
		int delay;
		
		if(strcmp(args[0],"remindme")==0)
		{
			if(args[1]==NULL || args[2]==NULL)
			{
				perror("no argument");
				return;
			}	
				delay =atoi(args[1]);	
				type=3;
		}	
		else
			type = check_ptype(args);
		if(type==1)
		{
			int f = 0;
			while(args[f]!=NULL)
			{
				if(strcmp(args[f],"&")==0)
					args[f]=NULL;
				f++;
			}	
		}	

		pid = fork();

		if(type!=0)
		{
			proc_arr[proc_no++]=pid;
			setpgid(pid,pid);	
		}		
	
		if(pid==0)
		{
	
			if(type==3)
			{	
				sleep(delay);
				printf("%s\n",args[2]);	
			}	
			else	
				execute_cmd(args);
			exit(EXIT_FAILURE);
		}
		else
		{
			if(type==0)
			{
				killed=0;
				stopped=0;
				signal(SIGINT,handler);
				signal(SIGTSTP,stop_proc);
				while(waitpid(pid,&status,WNOHANG)!=pid&&killed==0&&stopped==0);
				if(stopped==1)
				{
					setpgid(pid,pid);
					kill(pid,SIGSTOP);
					proc_arr[proc_no++] = pid;
					printf("\n%d Stopped \n",pid);
				}
				if(killed==1)
				{
					printf("killed 481");
					kill(pid,SIGCONT);
					kill(pid,SIGKILL);
					wait(NULL);
				}
					killed =0 ;
					stopped = 0;
			}
			else
			{
				signal(SIGCHLD,SIG_DFL);
			}
		}

		// if(killed==1)
		// {
		// 	if(setpgid(pid,pid)<0)
		// 		perror("setpgid");	
		// }
	}	
			
	return;
}
