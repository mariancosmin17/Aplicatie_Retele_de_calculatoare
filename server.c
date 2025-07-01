#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <utmp.h>


int main() {

char comanda[4096],raspuns[4096],deexec[4096];
int confirmare_login=0; 
int socketp[2],child1p[2],child2p[2],child3p[2],child4p[2];
pid_t copil1fork,copil2fork,copil3fork,copil4fork,socketc;

if (mkfifo("client_la_server", 0666) == -1) {
    if (errno == EEXIST)
      fprintf(stdout, "Nota: canalul fifo 'client_la_server' exista deja!\n");
    else {
      perror("Eroare la crearea canalului fifo. Cauza erorii");
      exit(1);
    }
  }

  if (mkfifo("server_la_client", 0666) == -1) {
    if (errno == EEXIST)
      fprintf(stdout, "Nota: canalul fifo 'server_la_client' exista deja!\n");
    else {
      perror("Eroare la crearea canalului fifo. Cauza erorii");
      exit(1);
    }
  }
  

  int rfd = open("client_la_server", O_RDONLY);
  if (rfd == -1) {
    perror("^srv^ Eroare la deschiderea canalului fifo");
    exit(2);
  }

 int wfd = open("server_la_client", O_WRONLY);
  if (wfd == -1) {
    perror("^srv^ Eroare la deschiderea canalului fifo");
    exit(2);
  }
  
while(1)
{

  memset(comanda, 0, sizeof(comanda)); 
  while(read(rfd, comanda, sizeof(comanda)) != 0)
  {printf("Am citit comanda %s \n",comanda);

	if(strncmp(comanda,"clnt:",5)==0)
	{strcpy(deexec,comanda+5);
	printf("^srv^:Am primit comanda %s de lungime %d \n",comanda+5,strlen(comanda+5));
	break;
	}
  memset(comanda, 0, sizeof(comanda)); 
  }
        
   if (socketpair(AF_UNIX, SOCK_STREAM, 0, socketp) < 0) 
      { 
        perror("^srv^ Eroare la socketpair"); 
        exit(1); 
      }
     
   if ((socketc = fork()) == -1) {
        perror("^srv^ Eroare la crearea unui proces fiu pnetru socket");
        exit(2);
    }
   if(socketc!=0)//parinte
    {
    wait(NULL);
    close(socketp[0]);
    memset(raspuns, 0, sizeof(raspuns)); 
    read(socketp[1],raspuns,sizeof(raspuns));
    close(socketp[1]);
    	
    	if(strcmp(raspuns,"srv:Login succes")==0)
    	{
    	confirmare_login=1;
    	}
    	else if(strcmp(raspuns,"srv:Logout cu succes")==0)
    	confirmare_login=0;
    
    write(wfd,raspuns,strlen(raspuns));
    printf("^srv^: Am trimis raspunsul: %s\n", raspuns);
    
    if(strcmp(raspuns,"srv:Quitting")==0)
    exit(EXIT_SUCCESS);
    
    }
    else if(socketc==0)
    {
  
    close(socketp[1]);
    if(strncmp(deexec,"login : ",8)==0)
    {
    
    
    if (pipe(child1p) == -1) {
        perror("^srv^ Eroare la crearea canalului anonim copilul 1");
        exit(1);
    }
    if ((copil1fork = fork()) == -1) {
        perror("^srv^ Eroare la crearea unui proces fiu in copilul 1");
        exit(2);
    }
    if(copil1fork!=0)
    	{write(child1p[1],deexec,strlen(deexec));
    	close(child1p[1]);
    	wait(NULL);
    	
    	read(child1p[0],deexec,sizeof(deexec));
    	printf("comandaLogin: am primit %s de la copil \n",deexec);
    	close(child1p[0]);
    	write(socketp[0],deexec,strlen(deexec));
    	}
    else 
    	{
    	read(child1p[0],deexec,sizeof(deexec));
    	close(child1p[0]);
    	
    	if(confirmare_login==1)
    	{
    	write(child1p[1],"srv:Esti logat deja!",21);
    	close(child1p[1]);
    	exit(EXIT_SUCCESS);
    	}
    	
    	FILE *useri = fopen("useri.txt", "r");
    		if (!useri) {
        	perror("^srv^ Eroare la deschiderea read-only a fisierului useri");
        	exit(EXIT_FAILURE);
    		}
    	char line[4096];
    	while (fgets(line, sizeof(line), useri))
    	{
    	  char *user=strtok(line,",");
    	  while(user!=NULL)
    	  {
    	    if(strcmp(user,deexec+8)==0)
    	    confirmare_login=1;
    	    user=strtok(NULL,",");
    	  }
    	}
    	fclose(useri);
    	if(confirmare_login==1)
    	write(child1p[1],"srv:Login succes",17);
    	else
    	write(child1p[1],"srv:Login fail",15);
    close(child1p[1]);
    exit(EXIT_SUCCESS);
    	}
    }
    
    else if(strcmp(deexec,"get-logged-users")==0)
    {
    
    
    if (pipe(child2p) == -1) {
        perror("^srv^ Eroare la crearea canalului anonim copilul 2");
        exit(1);
    		             }
    
    if ((copil2fork = fork()) == -1) {
        perror("^srv^ Eroare la crearea unui proces fiu in copilul 2");
        exit(2);
                                     }
    
    if(copil2fork!=0)
        {
        write(child2p[1],deexec,strlen(deexec));
    	close(child2p[1]);
    	wait(NULL);
    	
    	read(child2p[0],deexec,sizeof(deexec));
    	printf("comandaLogusers: am primit %s de la copil \n",deexec);
    	close(child2p[0]);
    	write(socketp[0],deexec,strlen(deexec));
    	}
    else
    {
    
    read(child2p[0],deexec,sizeof(deexec));
    close(child2p[0]);
    	
    	if(confirmare_login==0)
    	{
    	write(child2p[1],"srv:Nu este logat userul",25);
    	close(child2p[1]);
    	}
  	  else
    	  { 
    	  char info[4096];
    	  strcpy(info,"srv:");
    	  struct utmp *ut;
    	  setutent();
    	  ut=getutent();
    	     while(ut!=NULL)
    	     {
    		if(ut->ut_type==USER_PROCESS)//proces normal
    		{strcat(info,"user:");
    		strcat(info,ut->ut_user);
    		strcat(info,",host:");
    		strcat(info,ut->ut_host);
    		strcat(info,",time:");
    		strncat(info,ctime(&ut->ut_tv.tv_sec),20);
    		}
    	   ut=getutent();
    	   }
    	  endutent();
    	  write(child2p[1],info,strlen(info));
    	  close(child2p[1]);
    	  }
    	  exit(EXIT_SUCCESS);
    }
    
    }
    
    else if(strncmp(deexec,"get-proc-info : ",16)==0)
    {
    
    if (pipe(child3p) == -1) {
        perror("^srv^ Eroare la crearea canalului anonim copilul 3");
        exit(1);
    		             }
    
    if ((copil3fork = fork()) == -1) {
        perror("^srv^ Eroare la crearea unui proces fiu in copilul 3");
        exit(2);
                                     }
    
    if(copil3fork!=0)
        {
        write(child3p[1],deexec,strlen(deexec));
    	close(child3p[1]);
    	wait(NULL);
    	
    	memset(deexec, 0, sizeof(deexec));
    	read(child3p[0],deexec,sizeof(deexec));
    	printf("comandaGetpid: Am primit %s de la copil \n",deexec);
    	close(child3p[0]);
    	write(socketp[0],deexec,strlen(deexec));
    	}
    else
        {
    
        read(child3p[0],deexec,sizeof(deexec));
        close(child3p[0]);
    	
    	if(confirmare_login==0)
    	{
    	write(child3p[1],"srv:Nu este logat userul",25);
    	close(child3p[1]);
    	}
    	     
    	     else
    	     {
    	     char cale[4096];
    	     char pidprimit[256];
    	     char info1[4096];
    	     strcpy(info1,"srv:");
    	     strcpy(pidprimit,deexec+16);
    	     strcpy(cale,"/proc/");
    	     strcat(cale,pidprimit);
    	     strcat(cale,"/status");
    	     
    	     if(atoi(pidprimit)<=0)
    	     {
    	     write(child3p[1],"srv:Pid-ul primit este invalid",31);
    	     close(child3p[1]);
    	     }
    	    
    	        else
    	        { 
    	        char nume[256],state,vmsize[256];
    	        int ppid,uid;
    	        
    	     	FILE *procinfo = fopen(cale, "r");
    		
    		if (!procinfo) 
    		{
        	perror("^srv^ Eroare la deschiderea read-only a fisierului din proc");
        	exit(2);
    		}
    		
    	        char line[4096];
    	        while(fgets(line, sizeof(line), procinfo))
    	        {
    	         
    	         if(sscanf(line,"Name: %s",nume)==1)
    	         {
    	         strcat(info1,"name:");
    	         strcat(info1,nume);
    	         }
    	         
    	         else if(sscanf(line,"State: %c",&state)==1)
    	         {
    	         strcat(info1,",state:");
    	         info1[strlen(info1)]=state;
    	         }
    	         
    	         else if(sscanf(line,"PPid: %d",&ppid)==1)
    	         {char pppid[256];
    	         strcat(info1,",ppid:");
    	         sprintf(pppid,"%d",ppid);
    	         strcat(info1,pppid);
    	         }
    	         
    	         else if(sscanf(line,"Uid: %d",&uid)==1)
    	         {
    	         char uuid[256];
    	         strcat(info1,",uid:");
    	         sprintf(uuid,"%d",uid);
    	         strcat(info1,uuid);
    	         }
    	         
    	         else if(sscanf(line,"VmSize: %s",vmsize)==1)
    	         {
    	         strcat(info1,",vmsize:");
    	         strcat(info1,vmsize);
    	         }
    	         
    	         }
    	         fclose(procinfo);
    	         write(child3p[1],info1,strlen(info1));
    	         close(child3p[1]);
    	        }
    	              
    	       }
         
         exit(EXIT_SUCCESS);
        }
    
    }
    
    else if(strcmp(deexec,"logout")==0)
    {
    
    if (pipe(child4p) == -1) {
        perror("^srv^ Eroare la crearea canalului anonim copilul 4");
        exit(1);
    		             }
    
    if ((copil4fork = fork()) == -1) {
        perror("^srv^ Eroare la crearea unui proces fiu in copilul 4");
        exit(2);
                                     }
    
    if(copil4fork!=0)
        {
        write(child4p[1],deexec,strlen(deexec));
    	close(child4p[1]);
    	wait(NULL);
    	
    	memset(deexec, 0, sizeof(deexec));
    	read(child4p[0],deexec,sizeof(deexec));
    	printf("comandaLogout: Am primit %s de la copil \n",deexec);
    	close(child4p[0]);
    	write(socketp[0],deexec,strlen(deexec));
    	}
    	  else
            {
            read(child4p[0],deexec,sizeof(deexec));
            close(child4p[0]);
    
    	    if(confirmare_login==1)
    	     {
    	    write(child4p[1],"srv:Logout cu succes",21);
    	     }
    	    else
    	    write(child4p[1],"srv:Nu esti logat!",19);
    	    
    	    close(child4p[1]);
    	    exit(EXIT_SUCCESS);
    	    }
    }
    
    else if(strcmp(deexec,"quit")==0)
    {
    	write(socketp[0],"srv:Quitting",13);
    }
    
    else
    {
    	write(socketp[0],"srv:Comanda invalida",21);
    }
    close(socketp[0]);
    exit(EXIT_SUCCESS);
}
 
}
  close(rfd);
  close(wfd);
return 0;
}
