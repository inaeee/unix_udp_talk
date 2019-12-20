udp_talkserv.c
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

char *EXIT_STRING = "exit";
int recv_and_print(int sd, struct sockaddr_in servaddr);
int input_and_send(int sd, struct sockaddr_in servaddr);

#define MAXLINE 511

int main(int argc, char *argv[]) {
   struct sockaddr_in cliaddr, servaddr;
   int s, nbyte, addrlen = sizeof(struct sockaddr);
   pid_t pid;
   char buf[MAXLINE+1];

   if(argc != 2) {
      printf("사용법 :%s port\n", argv[0]);
      exit(0);
   }
   if((s = socket(PF_INET, SOCK_DGRAM, 0))<0) {
      perror("socket fail");
      exit(0);
   }
   bzero((char*)&cliaddr, addrlen);
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
   servaddr.sin_port = htons(atoi(argv[1]));
   if(bind(s, (struct sockaddr*)&servaddr, addrlen)<0) {
      perror("bind fail");
      exit(0);
   }

   puts("서버가 클라이언트를 기다리고 있습니다.");
   nbyte=recvfrom(s,buf,MAXLINE,0,(struct sockaddr *)&cliaddr,&addrlen);
   puts("클라이언트가 연결되었습니다.");
   if(nbyte<0) {
      perror("recvfrom fail");
      exit(0);
   }
   buf[nbyte] = 0;
   printf("%s",buf);

   if((pid = fork())>0) {
      input_and_send(s,cliaddr);
   } else if(pid == 0) {
      recv_and_print(s,cliaddr);
   }
   close(s);
   return 0;
}



int input_and_send(int sd,struct sockaddr_in cliaddr) {
   char buf[MAXLINE+1];
   int nbyte, addrlen = sizeof(struct sockaddr);

   while(fgets(buf, sizeof(buf), stdin) != NULL) {
      nbyte = strlen(buf);
      if(sendto(sd,buf,nbyte,0,(struct sockaddr *)&cliaddr, addrlen)<0) {
         perror("sendto fail");
         exit(1);
      }
      if(strstr(buf, EXIT_STRING) != NULL) {
         puts("Good bye");
         close(sd);
         exit(0);
      }
   }

}



int recv_and_print(int sd, struct sockaddr_in cliaddr) {
   char buf[MAXLINE+1];
   int nbyte, addrlen = sizeof(struct sockaddr);

   while(1) {
      nbyte = recvfrom(sd,buf, MAXLINE,0,(struct sockaddr *)&cliaddr, &addrlen);
      if(nbyte < 0 ) {
         perror("recvfrom fail");
         exit(0);
      }
      buf[nbyte] = 0;
      if(strstr(buf,EXIT_STRING) !=NULL) {
         break;
      }
      printf("%s",buf);
   }

}

