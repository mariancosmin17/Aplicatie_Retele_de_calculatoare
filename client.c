#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>


int main() {
  char comanda[4096],comanda1[4096];
  char raspuns[4096];

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
  int wfd = open("client_la_server", O_WRONLY);
  if (wfd == -1) {
    perror("Eroare la deschiderea canalului fifo");
    exit(2);
  }

  int rfd = open("server_la_client", O_RDONLY);
  if (rfd == -1) {
    perror("Eroare la deschiderea canalului fifo");
    exit(2);
  }

  while (1) {printf("Introduceti comanda-> \n");
    scanf(" %[^\n]", comanda1);
    strcpy(comanda,"clnt:");
    strcat(comanda,comanda1);
    write(wfd, comanda, strlen(comanda));
    printf("Am trimis comanda %s \n",comanda+5);
    memset(raspuns, 0, sizeof(raspuns)); 
    
    while(read(rfd, raspuns, sizeof(raspuns))!=0)
      {
      if (strncmp(raspuns, "srv:", 4) == 0)
        {printf("^client^ Rezulatatul este: %d %s \n", strlen(raspuns),raspuns);
      
      if (strcmp(raspuns + 4, "Quitting") == 0) exit(EXIT_SUCCESS);
      break;
      }
      memset(raspuns, 0, sizeof(raspuns)); 
    }
  }
  close(wfd);
  close(rfd);

  return 0;
}

