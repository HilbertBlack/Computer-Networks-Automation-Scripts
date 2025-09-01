#include<stdio.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
int main()
{
	int sockdesc;
	struct sockaddr_in servaddr;
	sockdesc=socket(AF_INET,SOCK_STREAM,0);
	if(sockdesc==-1)
	{
		printf("Socket not created");
		return -1;
	}

	servaddr.sin_family=AF_INET;
    	servaddr.sin_port=htons(1025);			
    	servaddr.sin_addr.s_addr=inet_addr("127.0.0.1");

	if (connect(sockdesc,(struct sockaddr*)&servaddr,sizeof(servaddr)) < 0)
	{
		printf("Connect Failed");
		return -1;
	}

	printf("connected successfully ");
	
	char buffer[10];
	printf("Enter the message to be sent to the proxy: ");
	fgets(buffer,sizeof(buffer),stdin);
	write(sockdesc,buffer,sizeof(buffer));
	
	read(sockdesc,buffer,10);
	printf("Message from proxy: %s", buffer);
	

	close(sockdesc);

	scanf("%s",buffer);
	scanf("%s",buffer);
	return 0;

}
