#include <ncurses.h>
#include <unistd.h>
#include <string.h>

void draw_borders(WINDOW *screen);
FILE *fp;

int main(int argc, char *argv[]) {
		int parent_x, parent_y;
		int new_x, new_y;
		int score_size = 5;
		int ch;
		int ty=1,tx=1,strn=0,ry=1,rx=1;
		char str[100];

		initscr();
		noecho();
		nodelay(stdscr,TRUE);
		curs_set(FALSE);
		//keypad(score,TRUE);
		// get our maximum window dimensions
		getmaxyx(stdscr, parent_y, parent_x);
		// set up initial windows
		WINDOW *field = newwin(parent_y - score_size, parent_x, 0, 0);
		WINDOW *score = newwin(score_size, parent_x, parent_y - score_size, 0);
		keypad(score,TRUE);
		// draw to our windows
		draw_borders(field);
		draw_borders(score);
		while(1){
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
				ch = mvwgetch(score,ty,tx);
				if( ch == '\n' ) {
						str[strn]='\0';
						draw_borders(score);

						/****************************/
						fp = fopen("log.txt","w");
						fprintf(fp,"%s",str);
						fclose(fp);
						/***************************/
						mvwaddstr(field,ry,rx,str);
						strn=0;
						ry++; 

						wclear(score);
						wrefresh(score);
						draw_borders(score);
						wrefresh(score);
						tx=1;
						ty=1;
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
		// clean up
		delwin(field);
		delwin(score);
		endwin();
		//return 0;
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
