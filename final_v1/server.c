#include<stdio.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
int main()
{
	int sockdesc;
	struct sockaddr_in servaddr,cliaddr;

	sockdesc=socket(AF_INET,SOCK_STREAM,0);
	if(sockdesc==-1)
	{
		printf("Socket not created");
		return -1;
	}

	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(1025);		// PORT number ranges from 1024 to 49151
	servaddr.sin_addr.s_addr= inet_addr("127.0.0.1");//htonl(INADDR_ANY);	// Accept requests coming through any interface
							// if requests to only a specific interface (with IP address say 192.168.1.1) are to be accepted, then use inet_addr("192.168.1.1") available in <arpa/inet.h>

	if(bind(sockdesc,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0)
	{
		printf("Bind Failed");
		return -1;
	}

	if(listen(sockdesc,15)<0)
	{
		printf("Listen Failed");
		return -1;
	}
	

	while(1)
	{
		int len=sizeof(cliaddr);
		int connfd=accept(sockdesc,(struct sockaddr*)&cliaddr,&len);
		if (connfd<0)
		{
			printf("Accept failed");
			return -1;
		}

				double temp=100;
				double * buffer= &temp;
				//char buffer[10]="123456789\0";

				read(connfd,buffer,sizeof(*buffer));
				printf("Message received from client: %lf",*buffer);
				printf("Forwarding the same message to the client...\n");
				write(connfd,buffer,sizeof(*buffer));
	//	close(connfd);
	}

	close(sockdesc);
	
	return 0;
}




 
