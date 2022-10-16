#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/io.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#define PCI_MAX_BUS 255
#define PCI_MAX_DEV 31
#define PCI_MAX_FUN 7
 
#define PCI_BASE_ADDR 0x80000000L
 
#define CONFIG_ADDR 0xcf8
#define CONFIG_DATA 0xcfc
 
typedef unsigned long DWORD;
typedef unsigned int WORD;
 
char* perevod(int n)
{
    char* c;
    c = (char *)malloc(10 * sizeof(char)); 
    int v = 0; 
    while (n > 15)
    {
        c[v] = (n % 16);
        if(c[v]<10) c[v]=c[v]+'0';
        else if (c[v]>=10 && c[v]<16) c[v]=c[v]+'a'-10; 
        n = n / 16;
        v++;
    }
    
    if(n<10) c[v] = n+'0';
    else if (n>=10 && n<16) c[v] = n + 'a'-10;
    v++;
    c[v] = '\0';
    char t;
  
    for (int i = 0; i < v / 2; i++)
    {
        t = c[i];
        c[i] = c[v - 1 - i];
        c[v - 1 - i] = t;
    }
    return c;
}
void out_after_ven(char* buf)
{
	char* temp=buf+4;
	while(*temp!='\n')
	{
		printf("%c",*temp);
		temp++;
	}
	printf("\n");
}
int search_dev(char* buf,char* dev)
{
	if(*buf==dev[0] && *(buf+1)==dev[1] && *(buf+2)==dev[2] && *(buf+3)==dev[3])
		return 1;
	else return 0;
}
void out_after_dev(char* buf,char* sym, char* dev)
{
	int tmp;
	int k=1;
	while(k==1)
	{
		buf=strtok(NULL,sym);
		if(*(buf-1)=='\t')
		{
			tmp=search_dev(buf,dev);
			if(tmp==1) 
			{
				out_after_ven(buf);
				return;
			}
			else continue;
		}
		else continue;
	}
}
char* get_name(DWORD veni,DWORD devi)
{
	int fd;
	char* dev,* ven;
	dev=perevod(devi);
	ven=perevod(veni);
	char* buf;
	char sym[]=" \t\n,.!?-";
	fd=open("pci",O_RDWR);
	if(fd==-1) perror("open   ");
	char* k;
	k=(char*)calloc(1327519,sizeof(char));
	if(read(fd,k,1327519)==-1) perror("read   ");
	buf=strtok(k,sym);
	while(buf!=NULL)
	{
		if(strcmp(buf,ven)==0 && *(buf-1)!='\t')
		{
			char* tmp;
			out_after_ven(buf);
			out_after_dev(buf,sym,dev);
			break;	
		}
		else
			buf=strtok(NULL,sym);	
	}
	close(fd);
	return buf;
}
int main()
{
    WORD bus, dev, fun;
    DWORD addr, data;
 
    if ( iopl(3) < 0 )
    {
        printf("iopl set error\n");
        return -1;
    }
    for (bus = 0; bus <= PCI_MAX_BUS; bus++)
        for (dev = 0; dev <= PCI_MAX_DEV; dev++)
            for (fun = 0; fun <= PCI_MAX_FUN; fun++)
            {
                addr = PCI_BASE_ADDR | (bus << 16) | (dev << 11) | (fun << 8);
                
                outl(addr, CONFIG_ADDR);
                data = inl(CONFIG_DATA);
                
                if (((data & 0xFFFF) != 0xFFFF) && (data != 0))
                {
                    printf("bus# %02d\ndev# %02d\nfun# %02d\n", bus, dev, fun);
                    DWORD ven=data & 0xFFFF, dev=(data & 0xFFFF0000) >> 16;
                    printf("ven_id# %04x\ndev_id# %04x\n", ven,dev);
                    get_name(ven,dev);
                    printf("\n\n");
                    printf("\n");
                }
            }
    if (iopl(0) < 0 )
    {
        printf("iopl set error\n");
        return -1;
    }
    return 0;
}
