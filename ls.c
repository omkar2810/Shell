#include "headers.h"
struct stat out_buff;

void get_perm(char* file,char* perm)
{
	
	mode_t info = out_buff.st_mode;
	int i=0;	
	for(i=0;i<10;i++)
		perm[i]='-';

	if(info&S_IFDIR)perm[0]='d';
	if(info&S_IRUSR)perm[1]='r';
	if(info&S_IWUSR)perm[2]='w';
	if(info&S_IXUSR)perm[3]='x';
	if(info&S_IRGRP)perm[4]='r';
	if(info&S_IWGRP)perm[5]='w';
	if(info&S_IXGRP)perm[6]='x';
	if(info&S_IROTH)perm[7]='r';
	if(info&S_IWOTH)perm[8]='w';
	if(info&S_IXOTH)perm[9]='x';

	return;
}

void get_lastmod(char * file,char* time)
{
	mode_t info = out_buff.st_mode;
	strftime(time, 50, "%b %d %H:%M %Y", localtime(&out_buff.st_mtime));
	return;
}

int get_links(char* file)
{
	int x = lstat(file,&out_buff);
	return out_buff.st_nlink;
}


void exec_ls(char *cur[])
{
	char flags[5]={0,0,0,0,0};
	int k=1;
	char fil[200];
	char * ret = getcwd(fil,200);
	while(cur[k]!=NULL)
	{
		if(strcmp(cur[k],"-a")==0)
			flags[1]=1;
		else if(strcmp(cur[k],"-l")==0)
			flags[0]=1;
		else if(strcmp(cur[k],"-la")==0)
			flags[1]=flags[0]=1;
		else if(strcmp(cur[k],"-al")==0)
			flags[1]=flags[0]=1;

		k++;
	}
	k--;
	if(cur[k]!=NULL && k>0)
	{
		if(cur[k][0]!='-')
			flags[2]=1;
	}	
	if(flags[2]==1)
	{
		int x = chdir(cur[k]);
		if(x<0)
		{
			perror("ls");
			return;
		}	
	}
	struct dirent *de;
	DIR *dr = opendir(".");
	
	if(dr==NULL)
	{
		perror("ls");
		int x = chdir(fil);
		return;
	}

	while((de = readdir(dr)) != NULL)
	{
		if(de->d_name[0]=='.'&& flags[1]==0)
			continue;
		if(flags[0]==1)
		{
			char perm[11];
			char time[100];
			for(int i=0;i<10;i++)
				perm[i]='-';

			int x = stat(de->d_name,&out_buff);
			if(x<0)
			{
				perror("stat error");
				continue;
			}
			get_perm(de->d_name,perm);
			get_lastmod(de->d_name,time);
			int links = get_links(de->d_name);
			struct passwd *pw = getpwuid(out_buff.st_uid);
			struct group  *gr = getgrgid(out_buff.st_gid);
			int size = out_buff.st_size;
			printf("%s %d %s %s %d %s ",perm,links,pw->pw_name,gr->gr_name,size,time);
			
		}

		printf("%s\n",de->d_name);		
	}

	closedir(dr);
	if(flags[2]==1)
	{
		int x = chdir(fil);
	}
	return;		
}
