/*
* myhttp.c - A simple HTTP client that sends GET, HEAD and POST request to HTTP servers and recieves resources.
*
* ICT310 Assignment 2
* Aisha Khalif
* 
*
*/

#include <stdio.h>
#include"stdlib.h"
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#define PORT 80
#define BUF_SIZE 1024 

//function declarations
void GET_content(int socket_desc);
void GET_response(int socket_desc);
void HEAD_method(int socket_desc);
void TRACE_method(int socket_desc);

//main function
int main(int argc, char *argv[])
{
	int socket_desc, ret, i, j;
	char buffer[BUF_SIZE];
	char * request_method;
	char * ip_address;
	char * server_addr;
	char  command[50];
	struct hostent *he;
	struct in_addr **addr_list;
	struct sockaddr_in server, client;


	// if user hasn't entered url, show usage
	if (argc < 2 || argc > 5) {
		fprintf(stderr, "minimum usage: ghbn <url> OR maximum usage: ghbn [-m <method>] [-a] <url>\n");
		//fprintf(stderr,"OR maximum usage: ghbn [-m <method>] [-a] <url>\n");
		return 1;
	}


	// take url and get IP address
	for (i = 0; i < argc; i++)
	{
		if (strstr(argv[i], ".com") != NULL)
		{
			if ((he = gethostbyname(argv[i])) == NULL)
			{
				herror("gethostbyname");
				return 2;
			}
		}
	}

	// Assign the IP address found to "ip_address"
	addr_list = (struct in_addr **)he->h_addr_list; //the list of ip addresses for that host
	ip_address = inet_ntoa(*addr_list[0]); // but we're only going to need one

	
	// creating a socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);

	if (socket_desc < 0)
	{
		printf("Error creating socket!\n");
		exit(1);

	}//puts("Socket created...\n"); //for testing purposes

	//establishing a connection using ip address retrieved above
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	inet_aton(ip_address, &server.sin_addr);

	//connect sys call
	ret = connect(socket_desc, (struct sockaddr *) &server, sizeof(server));
	if (ret < 0)
	{
		printf("Error connecting to the server!\n");
		exit(1);
	}

	printf("Connected to the server %s...\n\n", ip_address);

	// look for -m and method
	for (i = 1; i < argc; i++)
	{

		if (!strcmp(argv[i], "-m"))
		{

			for (j = i + 1; j < argc; j++)
			{

				if (!strcmp(argv[j], "HEAD") || !strcmp(argv[j], "head"))
				{

					HEAD_method(socket_desc);
					break;
				}

				else if (!strcmp(argv[j], "TRACE") || !strcmp(argv[j], "trace"))
				{
					TRACE_method(socket_desc);
					break;
				}

				else
				{
					printf("\nusage: ghbn <url> \nOR ghbn [-a] <url>\n OR ghbn [-m <method>] [-a] <url>\n");
					break;
				}

			}

		}

		else {
			// look for -a
			if (!strcmp(argv[i], "-a"))
			{

				GET_response(socket_desc);
				break;

			}
			else { GET_content(socket_desc); break; }
		} break;

	}

	return socket_desc;

}

//if there is no -a, so only content
void GET_content(int socket_desc) {

	char buffer[BUF_SIZE];

	send(socket_desc, "GET / HTTP/1.1\r\n\r\n", sizeof("GET / HTTP/1.1\r\n\r\n"), 0);
	recv(socket_desc, &buffer, sizeof(buffer), 0);

	//print only the content

	printf("%s", buffer);

	//close(socket_desc);

}


void GET_response(int socket_desc) {

	char buffer[BUF_SIZE];

	send(socket_desc, "GET / HTTP/1.1\r\n\r\n", sizeof("GET / HTTP/1.1\r\n\r\n"), 0);
	recv(socket_desc, &buffer, sizeof(buffer), 0);

	printf("%s", buffer);

	close(socket_desc);

}



void HEAD_method(int socket_desc) {


	char buffer[BUF_SIZE];

	send(socket_desc, "HEAD / HTTP/1.1\r\n\r\n", sizeof("HEAD / HTTP/1.1\r\n\r\n"), 0);
	recv(socket_desc, &buffer, sizeof(buffer), 0);

	printf("%s", buffer);

	close(socket_desc);

}

void TRACE_method(int socket_desc) {


	char buffer[BUF_SIZE];

	send(socket_desc, "TRACE / HTTP/1.1\r\n\r\n", sizeof("TRACE / HTTP/1.1\r\n\r\n"), 0);
	recv(socket_desc, &buffer, sizeof(buffer), 0);

	printf("%s", buffer);

	close(socket_desc);

}

