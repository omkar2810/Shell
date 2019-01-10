#include "headers.h"
volatile sig_atomic_t clk=1;

void show_time(char *cmd[])
{
		
	clk = 1;
	if(strcmp(cmd[1],"-t")!=0)
	{
		perror("no such command");
		return;
	}	
	
	char buf[15];

	while(clk)
	{
		int fd = open("/sys/class/rtc/rtc0/time",O_RDONLY);
		read(fd,buf,15);
		printf("%s",buf);
		sleep(atoi(cmd[2]));
	}

}