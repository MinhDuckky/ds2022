//Adapted from https://github.com/amitpathania/P2P-file-sharing
//Because I'm dumb
//The way the system works with data (both input and output) is different from the original

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <netinet/in.h>
#include<netdb.h>
#include<errno.h>
#include<arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#define ERROR     -1  // defines error message
#define MAX_CLIENTS    4 //defines maximum number of clients that can connect simultaneously
#define MAX_BUFFER    512 //used to set max size of buffer for send recieve data 
#define PORT    8080 //used to set max size of buffer for send recieve data 

int add_IP(char*, char*); // function to add IP to the list
int del_IP(char*, char*);//function to update IP_list once client disconnected


main(int argc, char **argv)  
{
	int sock; // sock is socket desriptor for server 
	int new; // socket descriptor for new client
	struct sockaddr_in server; //server structure 
	struct sockaddr_in client; //structure for server to bind to particular machine
	int sockaddr_len=sizeof (struct sockaddr_in);	//stores length of socket address
	char output[MAX_BUFFER];


	//variables for publish operation
	char buffer[MAX_BUFFER]; // Receiver buffer; 
	char buffer2[MAX_BUFFER]; // Receiver buffer; 
	char file_name[MAX_BUFFER];//Buffer to store filename,path and port recieved from peer
	char *peer_ip;//variable to store IP address of peer
	char peer_port[MAX_BUFFER];//variable to store IP address of peer

	//varriable for search operation
	char user_key[MAX_BUFFER];//file keyword for search by user
	int len;// variable to measure MAX_BUFFER of incoming stream from user

	int pid;// to manage child process

	char* peerinfo = "PeerStatus.txt";
	FILE *peerdet = fopen(peerinfo, "w");
	fclose(peerdet);
	
	char* fileinfo = "FileList.txt";
	FILE *filedet = fopen(fileinfo, "w");
	fclose(filedet);

	/*get socket descriptor */
	if ((sock= socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
	{ 
		perror("Server Socket Error: ");  // error checking the socket
		exit(-1);  
	} 
	 
	/*server structure */ 
	server.sin_family = AF_INET; // protocol family
	server.sin_port =htons(PORT); // Port No and htons to convert from host to network byte order. atoi to convert asci to integer
	server.sin_addr.s_addr = INADDR_ANY;//INADDR_ANY means server will bind to all netwrok interfaces on machine for given port no
	bzero(&server.sin_zero, 8); //padding zeros
	
	/*binding the socket */
	if((bind(sock, (struct sockaddr *)&server, sockaddr_len)) == ERROR) //pointer casted to sockaddr*
	{
		perror("Bind");
		exit(-1);
	} else {
		printf("Server Binded Successfully: port no %d and IP %s\n", ntohs(server.sin_port), 
		inet_ntoa(server.sin_addr));
	}

	/*listen the incoming connections */
	if((listen(sock, MAX_CLIENTS)) == ERROR) // listen for max connections
	{
		perror("Listen");
		exit(-1);
	}

	while(1)
	{
		if ((new = accept(sock, (struct sockaddr *)&client, &sockaddr_len)) == ERROR) // accept takes pointer to variable containing len of struct
		{
			perror("ACCEPT.Error accepting new connection");
			exit(-1);
		}

		pid=fork(); //creates separate process for each client at server

		if (!pid) // For multiple connections.this is the child process
		{ 
        close(sock); // close the socket for other connections

		len=recv(new, peer_port , MAX_BUFFER, 0);
		peer_port[len] = '\0';

		printf("New client connected from port no %d and IP %s\n", ntohs(client.sin_port), 
		inet_ntoa(client.sin_addr));
	 	peer_ip = inet_ntoa(client.sin_addr);

	 	add_IP(peer_ip, peer_port); //wadding Client IP into IP List
		
		while(1)
		{
		len=recv(new, buffer , MAX_BUFFER, 0);
		buffer[len] = '\0';

		//conenctionerror checking
			if(len<=0)//connection closed by client or error
				{
				if(len==0)//connection closed
				{
					printf("Peer %s hung up\n",inet_ntoa(client.sin_addr));
					del_IP(peer_ip, peer_port);
				}
				else //error
				{
					perror("ERROR IN RECIEVE");
				}
			close(new);//closing this connection
			exit (0);
			}//clos if loop

			//PUBLISH OPERATION
		if(buffer[0]=='p' && buffer[1]=='u' && buffer[2]=='b') // check if user wants to publish a file
		{
	
		/*Recieve publish files from peer */
		//adding publised file details to publish list
		len = recv(new, buffer2, MAX_BUFFER, 0); // recieve confirmation message from server
		buffer2[len] = '\0';
		printf("%s\n" , buffer2); // display confirmation message
		 // pad buffer with zeros
		peer_ip = inet_ntoa(client.sin_addr);
		len=recv(new, peer_port , MAX_BUFFER, 0);
		peer_port[len] = '\0';
		printf("%s\n",peer_port);

		char* fileinfo = "FileList.txt";
		FILE *filedet = fopen(fileinfo, "a+");
		bzero(buffer,MAX_BUFFER);
			//adding peer IP address to given file			
	

		if(filedet==NULL) // if unable to open file
        {
            printf("Unable to open File.Error");
            return 1;  
        }   
        else
		{		
			
		if((len=recv(new, buffer, MAX_BUFFER, 0))>0)
			{
			if (strlen(buffer2) >= 8) {
			fwrite(buffer2, 1, strlen(buffer2), filedet);
			fwrite("\t", 1, sizeof(char), filedet);	
			}
			else {
				fwrite(buffer2, 1, strlen(buffer2), filedet);
				fwrite("\t", 1, sizeof(char), filedet);	
				fwrite("\t", 1, sizeof(char), filedet);	
			}
			bzero(buffer2,MAX_BUFFER);
			fwrite(peer_ip, 1, strlen(peer_ip), filedet);			
			fwrite("\t", 1, sizeof(char), filedet);
			// write details of IP of client
			fwrite(peer_port, 1, strlen(peer_port), filedet);	
			fwrite("\n", 1, sizeof(char), filedet);			
			//len=recv(sock, output, BUFFER, 0);
			buffer[len] = '\0'; // checking null for end of data
			printf("%s\n", buffer);
			fwrite(buffer, 1, strlen(buffer), filedet);
			bzero(buffer,MAX_BUFFER);
			fwrite("\n", sizeof(char), 1, filedet);	
			//fwrite("\n", 1, sizeof(char), filedet);	
			fclose(filedet);
			}	
		}

		}// close if pub check loop

		//SEARCH OPERATION
		else if(buffer[0]=='s' && buffer[1]=='e' && buffer[2]=='a') //check keyword for search sent by client
		{
		
		bzero(buffer,MAX_BUFFER); // clearing the buffer by padding

		len=recv(new, peer_port , MAX_BUFFER, 0);
		peer_port[len] = '\0';

		usleep(500000);
		char Status[] = "Request recieved\nServer responding.......\n\nPeers Status\tIP\t\tPort No\n-----------------------------------------"; 
		send(new,Status,sizeof(Status),0);

		char* peer_status = "PeerStatus.txt";
		char buffer[MAX_BUFFER];

		FILE *file_status = fopen(peer_status, "r"); // open and read file conataining result of searchscript.sh
		if(file_status == NULL)
		    {
		        fprintf(stderr, "ERROR while opening file on server.");
				exit(1);
		    }

		    bzero(buffer, MAX_BUFFER); 
		    int file_status_send; 
		    while((file_status_send = fread(buffer, sizeof(char), MAX_BUFFER, file_status))>0) //send search result to peer
		    {
		        len=send(new, buffer, file_status_send, 0);

		        if(len < 0)
		        {
		            fprintf(stderr, "ERROR: File not found");
		            exit(1);
		        }
		        bzero(buffer, MAX_BUFFER);
		    }
		    fclose(file_status);

		usleep(500000);

		char FList[] = "\n\nNickname\tIP\t\tPort No\n-----------------------------------------"; 
		send(new,FList,sizeof(FList),0);

		char* search_result = "FileList.txt";
		
		FILE *file_search = fopen(search_result, "r"); // open and read file conataining result of searchscript.sh
		if(file_search == NULL)
		    {
		        fprintf(stderr, "ERROR while opening file on server.");
				exit(1);
		    }

		    bzero(buffer, MAX_BUFFER); 
		    int file_search_send; 
		    while((file_search_send = fread(buffer, sizeof(char), MAX_BUFFER, file_search))>0) //send search result to peer
		    {
		        len=send(new, buffer, file_search_send, 0);

		        if(len < 0)
		        {
		            fprintf(stderr, "ERROR: File not found");
		            exit(1);
		        }
		        bzero(buffer, MAX_BUFFER);
		    }
		    fclose(file_search);
        	
		    printf("Client disconnected from port no %d and IP %s\n", ntohs(client.sin_port), 
inet_ntoa(client.sin_addr));
		    peer_ip = inet_ntoa(client.sin_addr); // return the IP

			del_IP(peer_ip, peer_port);
		    //kill(pid,SIGKILL);
			close(new); // disconnect this client so that other users can connect server
			exit(0);
		}// close search condition

		//TERMINATE OPERATION:when user want to disconnect from server
		else if(buffer[0]=='t' && buffer[1]=='e' && buffer[2]=='r')
		{
		printf("Client disconnected from port no %d and IP %s\n", ntohs(client.sin_port), 
inet_ntoa(client.sin_addr));
		peer_ip = inet_ntoa(client.sin_addr);
		len=recv(new, peer_port , MAX_BUFFER, 0);
		
		peer_port[len] = '\0';
		del_IP(peer_ip, peer_port);
		//kill(pid,SIGKILL);
		close(new);// close the connection
		exit(0);
		} //close terminate loop
	
	}// close while loop inside fork.server will keep listening client till disconnected
	} // close if loop for fork function
	close(new);
	}// close main while loop
	close(sock);
}

int del_IP(char *peer_ip, char* peer_port)
{
// Client IP in IP List
		char* peerinfo = "PeerStatus.txt";
		FILE *peerdet = fopen(peerinfo, "a+");

		if(peerdet==NULL) // if unable to open file
        {
            printf("Unable to open IPList File.Error");
            return -1;  
        }   
        
        else
		{
		char* str = "Disonnected: \t";
		fwrite(str, 1, strlen(str), peerdet);			
		fwrite(peer_ip, 1, strlen(peer_ip), peerdet);			
		fwrite("\t", 1, sizeof(char), peerdet);			
		// write details of IP of client
		fwrite(peer_port, 1, strlen(peer_port), peerdet);		//adding peer IP address to given file	
		fwrite("\n", 1, sizeof(char), peerdet);			

		fclose(peerdet);
	}
}

int add_IP(char *peer_ip, char* peer_port)
{
//adding Client IP in IP List
		char* peerinfo = "PeerStatus.txt";
		FILE *peerdet = fopen(peerinfo, "a+");

		if(peerdet==NULL) // if unable to open file
        {
            printf("Unable to open IPList File.Error");
            return -1;  
        }   
        
        else
		{
		char* str = "Connected: \t";
		fwrite(str, 1, strlen(str), peerdet);			
		fwrite(peer_ip, 1, strlen(peer_ip), peerdet);			
		fwrite("\t", 1, sizeof(char), peerdet);			
		// write details of IP of client
		fwrite(peer_port, 1, strlen(peer_port), peerdet);		//adding peer IP address to given file	
		fwrite("\n", 1, sizeof(char), peerdet);			

		fclose(peerdet);
	}
}