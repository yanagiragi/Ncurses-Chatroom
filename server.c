#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h> // avoid dest size not found

// For FreeBSD
#include <netinet/in.h>
#include <sys/types.h>

int port=2323;
int prior=0,q_index = 0;
char buf[1024];
char player_name[10][20];
int player_fd[10]; 

int main(int argc,char **argv)
{
	system("clear");

	int i=0,tmp=0,res=1024; // byte length of resp message
	int sockfd,maxfd,sock_opt=1;

	fd_set active_fd_set;
	struct sockaddr_in dest;

	int client_amm=0;

	if( (sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1){ 
		perror("socket()");
		exit(1);
	}

	dest.sin_family = AF_INET;
	dest.sin_port = htons(port);
	dest.sin_addr.s_addr = htonl(INADDR_ANY); 
	bzero(&(dest.sin_zero),8);

	sock_opt=1;
	if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(void*)&sock_opt,sizeof(sock_opt))== -1 )
		perror("setsockopt()");

	if ( bind(sockfd, (struct sockaddr*)&dest, sizeof(struct sockaddr)) == -1 ){
		perror("bind()");
		exit(1);
	} 

	if ( listen(sockfd, 10) == -1 ){
		perror("listen()");
		exit(1);
	}
	printf("socket via port %d initialization complete\n",port);

	FD_ZERO(&active_fd_set);
	FD_SET(sockfd,&active_fd_set);
	maxfd = sockfd;

	while(1){
		int ret;
		struct timeval 	tv;
		fd_set	read_fds;

		read_fds = active_fd_set;
		ret = select(maxfd+1,&read_fds,NULL,NULL,NULL);

		if(ret == -1){
			perror("select()");
			return -1;
		}
			else{
			for(i=0;i<FD_SETSIZE;i++){			
				if(FD_ISSET(i,&read_fds)){
					if(i == sockfd){ //Connection at original socket
						struct sockaddr_in client_addr;
						int new_fd;
						socklen_t len = sizeof(client_addr);

						new_fd = accept(sockfd,(struct sockaddr*)&client_addr,&len);
						if(new_fd == -1){
							perror("accept()");
							return -1;
						}
						else{
							if(res == -1){
								perror("recv()");
								return -1;
							}
							else if(res == 0){
								printf("client %d abort",i);
								strcpy(buf,"EXIT()");
								int fi;
								for(fi=0;fi<client_amm;fi++)
									res=send(player_fd[fi],buf,sizeof(buf),0);
								return 0;
							}
							else{
								printf("Accpet client come from [%s:%u] by fd [%d]\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port), new_fd);
								FD_SET(new_fd,&active_fd_set);
								if(new_fd > maxfd)
									maxfd = new_fd;

								player_fd[client_amm] = new_fd;

								++client_amm;

								break; // break when new client
							}		
						}
					}
					else{
						memset(buf,0,sizeof(buf));
						res = recv(i,buf,sizeof(buf),0);
						
						if(strcmp(buf,"testreserved")!=0 && strcmp(buf,"")!=0){
							printf("res=%d buf=%s\n",res,buf);
						}
							
						if(res == -1){
							perror("recv()");
							return -1;
						}
						else if(res == 0){
							printf("client %d abort.\n",i);
							return 0;
						}
						else{
							int fi;	
							for(fi=0;fi<FD_SETSIZE;fi++){
								if(player_fd[fi] == i)
									break;
							}
							if(fi%2){
								strcpy(player_name[fi],buf);
							}
							for(fi=0;fi<client_amm;fi++){
								if(player_fd[fi]!=sockfd){
									if(buf[0] == '['){
										res=send(player_fd[fi],buf,sizeof(buf),0);
										if(res == -1){
											perror("send()");
											return -1;
										}
										else if(res ==0){
											printf("res ==0");
											return -1; //abort?
										}
										else{
											printf("send to child sock %d: %s\n",player_fd[fi],buf);
										}
									}
								}
							}
						}
					}
				}// end if of FD_ISET
			}// end for
		}// end else
	}
	return 0;
}