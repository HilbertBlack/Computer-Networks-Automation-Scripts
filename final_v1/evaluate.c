#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 


int evalute(int sockdesc,struct sockaddr_in clientaddr,int program_number,int client_or_server)
{
	//struct sockaddr_in clientaddr;


	if(client_or_server == 0)
	{
		return 0;
	}
	
	printf("%d",program_number+5000);
	//clientaddr.sin_family=AF_INET;
	clientaddr.sin_port=htons(5000+program_number);
        //clientaddr.sin_addr.s_addr=inet_addr("127.0.0.1");

        int opt=1;
        setsockopt(sockdesc,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

        while(bind(sockdesc,(struct sockaddr *)&clientaddr,sizeof(clientaddr)) < 0)
        {
                printf("wait");
        }
        
	printf("binded successfully\n");
       
}

