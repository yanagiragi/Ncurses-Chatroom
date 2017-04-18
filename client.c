// via port : 2323

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <netinet/in.h>
#include <ncurses.h>


/*****variable for ncurses****/
int parent_x, parent_y;
int new_x, new_y;
int score_size = 5;
int ch;
int ty=1,tx=1,strn=0,ry=1,rx=1;
char str[100];
WINDOW *field;
WINDOW *score;
char str_for_ncur[1024];
/*************************/

void sigroute(int dunno)
{
	if(dunno == 2 || dunno ==3){
		delwin(field);
		delwin(score);
		endwin();
		exit(0);
	}
}
void returns()
{

	delwin(field);
	delwin(score);
	endwin();
	exit(0);
}
void draw_borders(WINDOW *screen);
void start_and_read();
void refresh_all();
void  myprintf(char buf[]);

int main(int argc,char **argv)
{

	/*****variable for socket****/
	int port=2323;
	int i=0,q=0,res=0,msg_amm=0;
	char buf[1024],usrname[1024];
	char resp[1024];
	int sockfd,recv_fd= -1;
	char *ip_address="127.0.0.1";
	char ts;
	int flag=0,amm=0;
	pid_t pid;
	/*************************/

	int fd[2];
	pipe(fd);

	signal(SIGINT,sigroute);
	signal(SIGQUIT,sigroute);

	struct sockaddr_in dest,dest2;
	fd_set active_fd_set;
	system("clear");

	if( (sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1){ 
		perror("socket()");
		exit(1);
	}

	dest.sin_family = AF_INET;// initialize dest
	dest.sin_port = htons(port);
	dest.sin_addr.s_addr = inet_addr(ip_address);
	bzero( &(dest.sin_zero),8) ;


	if( (recv_fd = socket(AF_INET,SOCK_STREAM,0)) == -1){ 
		perror("socket()");
		exit(1);
	}

	dest2.sin_family = AF_INET;// initialize dest
	dest2.sin_port = htons(port);
	dest2.sin_addr.s_addr = inet_addr(ip_address);
	bzero( &(dest2.sin_zero),8) ;

	if(connect(recv_fd ,(struct sockaddr*)&dest2 ,sizeof(dest2))==-1){
		perror("connect()");
		exit(1);
	}
	if(connect(sockfd ,(struct sockaddr*)&dest ,sizeof(dest))==-1){
		perror("connect()");
		exit(1);
	}
	printf("%d Done initialization\n",port);
	printf("%d Done initialization\n",port);
	// connected.

	printf("Your Name: ");
	scanf("%s",usrname);
	strcpy(buf,usrname);
	res = send(sockfd,buf,sizeof(buf),0);
	if(res < 0){
		perror("send()");
		returns();
	}	

	FD_ZERO(&active_fd_set);
	FD_SET(sockfd,&active_fd_set);
	FD_SET(recv_fd,&active_fd_set);
	int maxfd = sockfd;

	pid = fork();
	if(pid > 0)
		main_start();

	while(1){
		if(pid==0){// Child Process
			main_start();
			while(1){ // fork process will do the trick to avoid can't read in scanf'
				start_and_read();
				sprintf(buf,"[%s]:%s",usrname,str);
				if((res = send(sockfd,buf,sizeof(buf),0))==-1){
					perror("send()");
				}
			}
		}
		else if(pid > 0){ 
			int ret;
			fd_set read_fds;
			struct timeval tv;

			tv.tv_sec = 1;
			tv.tv_usec = 0;

			read_fds = active_fd_set;
			ret = select(maxfd+1,&read_fds,NULL,NULL,&tv);	

			if(res == -1){
				perror("select()");
				returns();
			}
			else{
				for(i=0;i<FD_SETSIZE;i++){
					if(FD_ISSET(i,&read_fds)){
						if(i==sockfd){ 
							strcpy(buf,"test_resevered");

							res = recv(recv_fd,buf,sizeof(buf),0);
							if(res <= 0){
								perror("recv()");
								returns();
							}
							else{
								if(strcmp(buf,"testreserved")==0){
									res = send(sockfd,buf,sizeof(buf),0);
									continue;
								}
								else{
									myprintf(buf);
								}
							}
							FD_ZERO(&active_fd_set);
							FD_SET(sockfd,&active_fd_set);
							FD_SET(recv_fd,&active_fd_set);
							break;	

						}//end if
						else{
							res = recv(i,buf,sizeof(buf),0);
							if(res == -1){
								perror("recv()");
								returns();
							}
							else if(res ==0){
								printf("Chat Room shutdown.\n");
								returns();
							}
							if(strcmp(buf,"EXIT()")==0){
								close(sockfd);
								returns();
							}
							myprintf(buf);
						} // end else
						break; // deal one at once
					}//end if for if(fdisset(i))
				}// end for for transverse fdset
			}//end else for select()
		}
	}
	close(sockfd);
	return 0;
}

void  myprintf(char buf[])
{
	strcpy(str,buf);
	mvwaddstr(field,ry,rx,str);	
	
	++ry;
	wclear(score);
	wrefresh(score);
	draw_borders(score);
	wrefresh(score);
	refresh_all();
}
int main_start(void) {
	initscr();
	noecho();
	nodelay(stdscr,TRUE);
	curs_set(FALSE);
	//keypad(score,TRUE);
	// get our maximum window dimensions
	getmaxyx(stdscr, parent_y, parent_x);
	// set up initial windows
	field = newwin(parent_y - score_size, parent_x, 0, 0);
	score = newwin(score_size, parent_x, parent_y - score_size, 0);
	keypad(score,TRUE);
	// draw to our windows
	draw_borders(field);
	draw_borders(score);

	// clean up
	//refresh_all();
	//delwin(field);
	//delwin(score);
	//endwin();
}
void refresh_all()
{
	getmaxyx(stdscr, new_y, new_x);
	if (new_y != parent_y || new_x != parent_x) {
		parent_x = new_x;
		parent_y = new_y;
		wresize(field, new_y - score_size, new_x);
		wresize(score, score_size, new_x);
		mvwin(score, new_y - score_size, 0);
		wclear(stdscr);
		wclear(field);
		wclear(score);
		draw_borders(field);
		draw_borders(score);
	}
	mvwprintw(field, 0, 0, "Chat Room");
	mvwprintw(score, 0, 0, "chat");
	// refresh each window
	wrefresh(field);
	wrefresh(score);
}
void start_and_read()
{
	while(1){
		refresh_all();
		ch = mvwgetch(score,ty,tx);
		if( ch == '\n' ) {
			str[strn]='\0';
			draw_borders(score);
			strn=0;
			ry++; 

			tx=1;
			ty=1;
			return ;
		}
		else{
			str[strn++]=ch;
			mvwaddch(score,ty,tx,ch);
			if(tx < parent_x - 2 )tx++;
			else{ 
				tx = 1;
				ty++;
			}
		}
	}		
}
void draw_borders(WINDOW *screen) {
	int x, y, i;
	getmaxyx(screen, y, x);
	// 4 corners
	mvwprintw(screen, 0, 0, "+");
	mvwprintw(screen, y - 1, 0, "+");
	mvwprintw(screen, 0, x - 1, "+");
	mvwprintw(screen, y - 1, x - 1, "+");
	// sides
	for (i = 1; i < (y - 1); i++) {
		mvwprintw(screen, i, 0, "|");
		mvwprintw(screen, i, x - 1, "|");
	}
	// top and bottom
	for (i = 1; i < (x - 1); i++) {
		mvwprintw(screen, 0, i, "-");
		mvwprintw(screen, y - 1, i, "-");
	}
}
