#include<stdio.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
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
// CONNECTING TO THE SERVER AS A PROXY
	if (connect(sockdesc,(struct sockaddr*)&servaddr,sizeof(servaddr)) < 0)
	{
		printf("Connect Failed");
		return -1;
	}

	char buffer[10];
	
	//printf("Enter the message to be sent to the server: ");
	//fgets(buffer,sizeof(buffer),stdin);
	//write(sockdesc,buffer,sizeof(buffer));
	
	//read(sockdesc,buffer,10);
	//printf("Message from server: %s", buffer);
// CONNECTING TO THE SERVER IS COMPLETE // TO ACCESS THE CONNECTION USE " write (sockdesc,..) / read (socdesc,..) "

  

// START OF THE PROXY SERVER	
	int proxysockdesc;
        struct sockaddr_in proxyservaddr,cliaddr;

        proxysockdesc=socket(AF_INET,SOCK_STREAM,0);
        if(proxysockdesc==-1)
        {
                printf("Socket not created");
                return -1;
        }

        proxyservaddr.sin_family=AF_INET;
        proxyservaddr.sin_port=htons(1035);          // PORT number ranges from 1024 to 49151
        proxyservaddr.sin_addr.s_addr=htonl(INADDR_ANY);     // Accept requests coming through any interface
                                                        // if requests to only a specific interface (with IP address say 192.168.1.1) are to be accepted, then use inet_addr("192.168.1.1") available in <arpa/inet.h>

        if(bind(proxysockdesc,(struct sockaddr *)&proxyservaddr,sizeof(proxyservaddr)) < 0)
        {
                printf("Bind Failed");
                return -1;
        }

        if(listen(proxysockdesc,5)<0)
        {
                printf("Listen Failed");
                return -1;
        }


        while(1)
        {
                int len=sizeof(cliaddr);
                int connfd=accept(proxysockdesc,(struct sockaddr*)&cliaddr,&len);
                if (connfd<0)
                {
                        printf("Accept failed");
                        return -1;
                }


                char buffer[10];
                strcpy(buffer," ");
                read(connfd,buffer,10);
                printf("Message received from client: %s", buffer);
          
		write(sockdesc,buffer,sizeof(buffer));
	  	printf("Forwarding the same message to the server...\n");

		read (sockdesc, buffer,sizeof(buffer));
		printf("message received from the server : %s",buffer);

                write(connfd,buffer,sizeof(buffer));
		printf("sending message from the server to the client\n");
        }

        close(proxysockdesc);

// END OF PROXY SERVER

// KILLING MAIN SERVER	
	close(sockdesc);
	return 0;

}
