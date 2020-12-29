#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

WINDOW *die(int height, int width, int origin_y, int origin_x);
void die_face(int face, int height, int width, int origin_y, int origin_x);

int main(int argc, char *argv[])
{	
	int origin_x, origin_y, width, height;
	int face1, face2;
	int ch;

	initscr();			// Initiate ncurses
	cbreak();			

	keypad(stdscr, TRUE);		// F1 Terminates

	height = 5;
	width = 9;
	origin_y = (LINES - height) / 2;	// Calculate center
	origin_x = (COLS - width) / 2;	

	curs_set(0);
	
	printw("Press a key to roll.\nPress F1 to exit.");

	refresh();

	while((ch = getch()) != KEY_F(1))
	{
		for ( int count = 1 ; count < 50 ; ++count )
		{
			// Die 1
			die_face(face1, height, width, origin_y, origin_x);
			die(height, width, origin_y, origin_x - width);
			refresh();

			// Die 2
			die_face(face2, height, width, origin_y, origin_x + 3 * width / 2);
			die(height, width, origin_y, origin_x + width / 2);
			refresh();
			
			// Roll
			face1 = rand()%6;
			face2 = rand()%6;
			
			usleep(20000);
		}

	}
		
	endwin();			// End ncurses
	return 0;
}


WINDOW *die(int height, int width, int origin_y, int origin_x)
{	
	WINDOW *local_win;

	local_win = newwin(height, width, origin_y, origin_x);
	box(local_win, 0 , 0);		/* 0, 0 gives default characters 
					 * for the vertical and horizontal
					 * lines			*/
	wrefresh(local_win);		

	return local_win;
}

void die_face(int face, int height, int width, int origin_y, int origin_x)
{
        switch(face)
        {
        case 0:
                mvprintw(origin_y + 1, origin_x - width + 2, "     ");
                mvprintw(origin_y + 2, origin_x - width + 2, "  x  ");    
                mvprintw(origin_y + 3, origin_x - width + 2, "     ");
                break;
        case 1:
                mvprintw(origin_y + 1, origin_x - width + 2, "x    ");
                mvprintw(origin_y + 2, origin_x - width + 2, "     ");    
                mvprintw(origin_y + 3, origin_x - width + 2, "    x");
                break;
        case 2:
                mvprintw(origin_y + 1, origin_x - width + 2, "    x");
                mvprintw(origin_y + 2, origin_x - width + 2, "  x  ");    
                mvprintw(origin_y + 3, origin_x - width + 2, "x    ");
                break;
        case 3:
                mvprintw(origin_y + 1, origin_x - width + 2, "x   x");
                mvprintw(origin_y + 2, origin_x - width + 2, "     ");    
                mvprintw(origin_y + 3, origin_x - width + 2, "x   x");
                break;
        case 4:
                mvprintw(origin_y + 1, origin_x - width + 2, "x   x");
                mvprintw(origin_y + 2, origin_x - width + 2, "  x  ");    
                mvprintw(origin_y + 3, origin_x - width + 2, "x   x");
                break;
        case 5:
                mvprintw(origin_y + 1, origin_x - width + 2, "x   x");
                mvprintw(origin_y + 2, origin_x - width + 2, "x   x");    
                mvprintw(origin_y + 3, origin_x - width + 2, "x   x");
                break;
        }
}

