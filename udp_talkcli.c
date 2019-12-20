udp_talkcli.c
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#define MAXLINE 511

char *EXIT_STRING = "exit";
int recv_and_print(int sd, struct sockaddr_in servaddr);
int input_and_send(int sd, struct sockaddr_in servaddr);

int main(int argc, char *argv[]) {
   pid_t pid;
   int s,nbyte;
   struct sockaddr_in servaddr;
   char buf[MAXLINE+1];
   int addrlen = sizeof(servaddr);

   if(argc != 3) {
      printf("사용법:%s server_ip port\n", argv[0]);
      exit(0);
   }
   if((s = socket(PF_INET, SOCK_DGRAM,0))<0) {
      printf("Client : cant open stream socket.\n");
      exit(0);
   }
   bzero((char *)&servaddr, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr = inet_addr(argv[1]);
   servaddr.sin_port = htons(atoi(argv[2]));

   if((pid=fork())>0) {
      input_and_send(s,servaddr);
   } else if(pid == 0) {
      recv_and_print(s,servaddr);
   }
   close(s);
   return 0;
}



int input_and_send(int sd,struct sockaddr_in servaddr) {
   char buf[MAXLINE+1];
   int nbyte, addrlen = sizeof(servaddr);

   while(fgets(buf,sizeof(buf),stdin) != NULL) {
      nbyte = strlen(buf);
      if(sendto(sd,buf, nbyte,0,(struct sockaddr *)&servaddr,addrlen)<0) {
         perror("sendto fail");
         exit(0);
      }
      if(strstr(buf, EXIT_STRING) !=NULL) {
         puts("Good bye");
         close(sd);
         exit(0);
      }
   }
   return 0;

}



int recv_and_print(int sd, struct sockaddr_in servaddr) {
   char buf[MAXLINE+1];
   int nbyte;
   int addrlen = sizeof(servaddr);

   while(1) {
      nbyte = recvfrom(sd,buf,MAXLINE,0,(struct sockaddr *)&servaddr, &addrlen);
      if(nbyte<0) {
         perror("recvfrom fail");
         exit(0);
      }
      buf[nbyte] = 0;
      if(strstr(buf, EXIT_STRING) != NULL) {
         break;
      }
      printf("%s",buf);
   }

   return 0;
}
