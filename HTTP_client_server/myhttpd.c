/*
* myhttpd.c - A simple HTTP server that sends resources to any HTTP client that requests it.
*
* ICT310 Assignment 2
* Aisha Khalif
* 32381361
*
*/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<signal.h>
#include<fcntl.h>

#define MAX_CONNECTIONS 1000
#define BYTES 1024

char *ROOT;
int socket_desc, clients[MAX_CONNECTIONS];
void error(char *);
void startup(char *);
void server_response(int);

int main(int argc, char* argv[])
{
	struct sockaddr_in client_addr;
	socklen_t addrlen;
	char command;
	int i, slot = 0;

	//Set a default port, root and log file path
	char PORT[5];
	ROOT = getenv("PWD"); //current working directory
	strcpy(PORT, "40705");


	//Parsing the command line arguments
	while ((command = getopt(argc, argv, "p:d:")) != -1)
		switch (command)
		{
		case 'd':
			ROOT = malloc(strlen(optarg));
			strcpy(ROOT, optarg);
			break;
		case 'p':
			strcpy(PORT, optarg);
			break;
		case '?':
			fprintf(stderr, "Usage: [ -p <port number> ] [ -d <document root> ] [ -l <log file> ]\n");
			exit(1);
		default:
			exit(1);
		}

	//print out details about connection - i.e. port and root
	printf("Server started at port no. %s%s%s with root directory as %s%s%s\n", "\033[92m", PORT, "\033[0m", "\033[92m", ROOT, "\033[0m");

	// -1 for no clients connected to server
	for (i = 0; i < MAX_CONNECTIONS; i++) {
		clients[i] = -1;
	}
	startup(PORT);

	// infinite while loop to accept connections from clients
	while (1)
	{
		addrlen = sizeof(client_addr);
		clients[slot] = accept(socket_desc, (struct sockaddr *) &client_addr, &addrlen);

		if (clients[slot] < 0) { error("Error accepting client."); }

		else
		{
			if (fork() == 0)
			{
				server_response(slot);
				exit(0);
			}
		}

		while (clients[slot] != -1) slot = (slot + 1) % MAX_CONNECTIONS;
	}

	return 0;
}

//start server
void startup(char *port)
{

	struct addrinfo hints, *res, *p;

	// get address info
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(NULL, port, &hints, &res) != 0)
	{
		perror("getaddrinfo() error");
		exit(1);
	}

	//creating socket and binding
	for (p = res; p != NULL; p = p->ai_next)
	{
		socket_desc = socket(p->ai_family, p->ai_socktype, 0);

			if (socket_desc == -1) { continue; }

			if (bind(socket_desc, p->ai_addr, p->ai_addrlen) == 0) { break; }
	}

	//error
	if (p == NULL)
	{
		perror("Error creating connection");
		exit(1);
	}

	freeaddrinfo(res);

	// listen for incoming connections
	if (listen(socket_desc, 5) != 0)
	{
		perror("listen() error");
		exit(1);
	}
	
}

//client connection
void server_response(int n)
{
	// open a file to serve to client
	FILE *html_data;
	html_data =  fopen("index.html", "r");

	char response_data[1024];
	fgets(response_data, 1024, html_data);

	char http_header[2048] = "HTTP/1.1 200 OK\r\n\n";
	strcat(http_header, response_data);

	send(clients[n], http_header, sizeof(http_header), 0);

	//closing the socket
	shutdown(clients[n], SHUT_RDWR);
	close(clients[n]);
	clients[n] = -1;
}
