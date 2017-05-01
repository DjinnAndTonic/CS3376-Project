// Log Server
// Receives messages from the echo server
// and prints out the messages into a log file
// By: Jimmy Nhes
// Formatting: Chance Ball
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <time.h>
#include <arpa/inet.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
   int sock, length, n;
   // portno should be "9999" upon submission; use "55556" for testing purposes
   int portno;
   socklen_t fromlen;
   struct sockaddr_in server;
   struct sockaddr_in from;
   char buf[1024];
   
   char s[1024];
	
//	char* server_ip = inet_ntoa(server.sin_addr);
//	printf("Log ip: %s\n", server_ip);
   // file pointer needed to write to a log text file
   FILE *fp;
   
   if(argc < 2){
	   printf("Using default port number (9999). \n");
	   portno = 9999;
   }
   else if(argc == 3){
	   if(strcmp(argv[1], "-port") == 0)
		   portno = atoi(argv[2]);
	   else
		   error("Unnknown option.");
   }
   else{
	   error("ERROR: Proper usage: log_s -port <port_no>");
   }
   
   sock=socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0) error("Opening socket");
   length = sizeof(server);
   bzero(&server,length);
   server.sin_family=AF_INET;
   server.sin_addr.s_addr=INADDR_ANY;
   
   // changing server.sin_port=htons(atoi(argv[1])) into htons(portno)
   server.sin_port=htons(portno);
   if (bind(sock,(struct sockaddr *)&server,length)<0) 
       error("binding");
   fromlen = sizeof(struct sockaddr_in);
   while (1) {
       n = recvfrom(sock,buf,1024,0,(struct sockaddr *)&from,&fromlen);
       if (n < 0) error("recvfrom");
       write(1,"Received a datagram: ",21);
       write(1,buf,n);
	   // this line cleans out the fluff in the buffer
	   // so that only the msg is printed out
       buf[n] = '\0';
	   // open log text file
	   fp = fopen("echo.log", "a");
	   if (fp == NULL) {
		   fprintf(stderr, "Can't open output file log.txt");
		   exit(1);
	   }
	   time_t t = time(NULL);
           struct tm tm = *localtime(&t);
	   
	   // write to the log text file
	   fprintf(fp,"%d-%d-%d %d:%d:%d \t %s was received from %s\n",
	   tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, 
	   tm.tm_hour, tm.tm_min, tm.tm_sec, buf,
	   inet_ntoa(from.sin_addr));
	   // close file
	   fclose(fp);
	   
       n = sendto(sock,"Got your message\n",17,
                  0,(struct sockaddr *)&from,fromlen);
       if (n  < 0) error("sendto");
   }
   return 0;
 }

