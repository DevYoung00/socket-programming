#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <fcntl.h>

void error(char *msg)
{
   perror(msg);
   exit(1);
}


char *getType(char *ch){
	char *fileType = ch;
	if (strcmp(ch, "jpg") == 0) {
		strcpy(fileType, "image/jpg");
	      	
	}
	else if(strcmp(ch, "gif") == 0) {
		strcpy(fileType, "image/gif"); 

	}
	else if(strcmp(ch,"pdf") ==0){
		strcpy(fileType, "application/pdf"); 
		
	}
	else if (strcmp(ch, "mp3") == 0) {
		strcpy(fileType, "audio/mpeg");
		
         }
         else {
		 strcpy(fileType, "text/html");
	
         }
        return fileType;
  
}  

int main(int argc, char *argv[])
{
   int sockfd, newsockfd;
   int portno;
   socklen_t clilen;

   char buffer[256];
   char myFileType[30];
   /*sockaddr_in: Structure Containing an Internet Address*/
   struct sockaddr_in serv_addr, cli_addr;
   char *tmp, *filename, *rep, *res,  *header;
   int length, n,file;
   bzero(buffer, 256);

     if (argc < 2) {
      fprintf(stderr,"ERROR, no port provided\n");
      exit(1);
   }
     
   
   sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (sockfd < 0) 
      error("ERROR opening socket");
   
   bzero((char *) &serv_addr, sizeof(serv_addr));
   portno = atoi(argv[1]); //atoi converts from String to Integer
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //for the server the IP address is always the address that the server is running on
   serv_addr.sin_port = htons(portno); //convert from host to network byte order
   
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //Bind the socket to the server address
      error("ERROR on binding");

   listen(sockfd,5); // Listen for socket connections. Backlog queue (connections to wait) is 5

   while(1) {

      clilen = sizeof(cli_addr);
      /*accept function: 
         1) Block until a new connection is established
         2) the new socket descriptor will be used for subsequent communication with the newly connected client.
      */
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
      if (newsockfd < 0) 
         error("ERROR on accept");
      
      bzero(buffer,256);
      n = read(newsockfd,buffer,255); //Read is a block function. It will read at most 255 bytes

    
      if (n < 0) error("ERROR reading from socket");
   
      printf("HTTP Request is %s  \n",buffer);
  
      filename = strtok(buffer," ");
      filename = strtok(NULL, " ");
      filename = strtok(filename , "/");
      
     
      if (strstr(filename, ".") == NULL) {
         printf("file path ERROR!!!!\n");
        
      } 
      else {

        tmp = strtok(filename, ".");
        tmp = strtok(0, ".");
        
	sprintf(filename, "%s.%s", filename,tmp); 

	 file =open(filename, O_RDONLY);

	 if(file<0)
	 {
		 file = open("404.html",O_RDONLY);
		 header = malloc(128); 
		   sprintf(header, "HTTP/1.1 500 Internal Server Error\n" "Content-Type: %s\n" "Content-Length: %d\n" "Connection: close\n" "\n",
            getType(tmp) , 0 ); 
		    printf("\nreponse message!\n\n");
		       printf("%s\n",header);


	 }
	 length = lseek(file, 0, SEEK_END); 
            lseek(file, 0, SEEK_SET);
	    header=malloc(128);
	                sprintf(header, "HTTP/1.1 200 OK\n" "Content-Type: %s\n" "Content-Length: %d\n" "Connection: close\n" "\n",
                            getType(tmp) , length);
               printf("\nresponse message!\n\n");
	       printf("%s\n",header);
	 
	       if(write(newsockfd, header, strlen(header)) < 0){
		       error("ERROR writing to socket");
	       }
	 
	       memset(buffer,0,sizeof(buffer));

	       while((length = read(file,buffer,sizeof(buffer)) > 0))
			       {
			       write(newsockfd, buffer, sizeof(buffer));
			       }
      }
	 

      close(file);
      printf("========================================\n");
      
   }
close(newsockfd);
   close(sockfd);
   
   return 0; 
}
