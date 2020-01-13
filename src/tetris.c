#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

#define WINDOW_WIDTH (450)
#define WINDOW_HEIGHT (550)

// speed in pixels/second
#define SPEED (25)

static int board[10][20] = {0};
struct piece *curr_piece;
int pieces[7] = {-1, -1, -1, -1, -1, -1, -1};

/*
	- I : 0
	- O : 1
	- T : 2
	- S : 3
	- Z : 4
	- J : 5
	- L : 6
*/
SDL_Rect self;
SDL_Rect b1, b2, b3, b4;
/*
b1 = self
b2-b4 = rest
*/

struct piece {
	int type;
	int rotation;
};


struct piece I = {.type = 1, .rotation = 0};
struct piece O = {.type = 2, .rotation = 0};
struct piece T = {.type = 3, .rotation = 0};
struct piece S = {.type = 4, .rotation = 0};
struct piece Z = {.type = 5, .rotation = 0};
struct piece J = {.type = 6, .rotation = 0};
struct piece L = {.type = 7, .rotation = 0};


void print_board();
void clear_board();
void fill_bag();
struct piece *select_piece();
void set_piece_orientation(struct piece *p);


int main(void) {

	srand(time(NULL));
    // attempt to initialize graphics and timer system
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* win = SDL_CreateWindow("Tetris in C!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    // create a renderer, which sets up the graphics hardware
    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);

	// Block Colors :
	/*
	- I : Blue
	- O : Yellow
	- T : Violet
	- S : Green
	- Z : Red
	- J : Indigo
	- L : Orange
	*/

	// I : BLUE:
	SDL_Surface* surface = IMG_Load("Resources/single_blocks/blue.png");
	SDL_Texture*  t_s0 = SDL_CreateTextureFromSurface(rend, surface);
	// O : YELLOW:
	surface = IMG_Load("Resources/single_blocks/yellow.png");
	SDL_Texture*  t_s1 = SDL_CreateTextureFromSurface(rend, surface);
	// T : VIOLET:
	surface = IMG_Load("Resources/single_blocks/violet.png");
	SDL_Texture*  t_s2 = SDL_CreateTextureFromSurface(rend, surface);
	// S : GREEN:
	surface = IMG_Load("Resources/single_blocks/green.png");
	SDL_Texture*  t_s3 = SDL_CreateTextureFromSurface(rend, surface);
	// Z : RED:
	surface = IMG_Load("Resources/single_blocks/red.png");
	SDL_Texture*  t_s4 = SDL_CreateTextureFromSurface(rend, surface);
	// J : INDIGO:
	surface = IMG_Load("Resources/single_blocks/indigo.png");
	SDL_Texture*  t_s5 = SDL_CreateTextureFromSurface(rend, surface);
	// L : ORANGE
	surface = IMG_Load("Resources/single_blocks/orange.png");
	SDL_Texture* t_s6 = SDL_CreateTextureFromSurface(rend, surface);

	// SINGLE BLOCKS
	SDL_Texture *single_blocks[7] = {t_s0, t_s1, t_s2, t_s3, t_s4, t_s5, t_s6};

	// CURSOR
	surface = IMG_Load("Resources/cursor.png");
	SDL_Texture* self_texture = SDL_CreateTextureFromSurface(rend, surface);

	// BACKGROUND
    surface = IMG_Load("Resources/bg.png");
    SDL_Texture* bg_texture = SDL_CreateTextureFromSurface(rend, surface);

    // FREE SURFACE
    SDL_FreeSurface(surface);
    
    // struct to hold the position and size of the sprite
    SDL_Rect bg;

    // BACKGROUND
    SDL_QueryTexture(bg_texture, NULL, NULL, &bg.w, &bg.h);

    // SELF
    SDL_QueryTexture(self_texture, NULL, NULL, &self.w, &self.h);
    
    // start self at one space down center left
    float x_pos = 25 * 5;
    float y_pos = 25 * 1;
    int curs_grid_x = 0;
    int curs_grid_y = 0;
	
    // keep track of which inputs are given
    int up = 0;
    int down = 0;
    int left = 0;
    int right = 0;
    
    // set to 1 when window close button is pressed
    int close_requested = 0;
    
    // background
    bg.y = 0;
    bg.x = 0;

    // piece handling
    bool piece_selected = false;
    bool piece_locked = false;
    //bool piece_held = false;

    // count of dropdown
    int downcount = 10;

    srand(time(NULL));
    while (!close_requested) {

    	// piece selection
    	if (!piece_selected) {
    		curr_piece = select_piece();
    		piece_selected = true;
    	}

        // process events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
	            case SDL_QUIT:
	                close_requested = 1;
	                break;

	            case SDL_KEYDOWN:
	                switch (event.key.keysym.scancode) {
		                /*case SDL_SCANCODE_W:
		                case SDL_SCANCODE_UP:
		                    up = 1;
		                    break;
						*/
		                case SDL_SCANCODE_A:
		                case SDL_SCANCODE_LEFT:
		                    left = 1;
		                    break;
		                
		                case SDL_SCANCODE_S:
		                case SDL_SCANCODE_DOWN:
		                    down = 1;
		                    break;

		                case SDL_SCANCODE_D:
		                case SDL_SCANCODE_RIGHT:
		                    right = 1;
		                    break;

		                case SDL_SCANCODE_P:
		                	print_board();
		                	break;

		                case SDL_SCANCODE_C:
		                	clear_board();
		                	break;
		                case SDL_SCANCODE_SPACE:
		                	if (curr_piece->rotation == 3) {
		                		curr_piece->rotation = 0;
		                	} else {
		                		curr_piece->rotation++;
		                	}
		            }
	            	break;

	            case SDL_KEYUP:
	                switch (event.key.keysym.scancode) {
	                case SDL_SCANCODE_A:
	                case SDL_SCANCODE_LEFT:
	                    left = 0;
	                    break;

	                case SDL_SCANCODE_S:
	                case SDL_SCANCODE_DOWN:
	                    down = 0;
	                    break;

	                case SDL_SCANCODE_D:
	                case SDL_SCANCODE_RIGHT:
	                    right = 0;
	                    break;
	                }
	            	break;
	        }
        }


        set_piece_orientation(curr_piece);


        // directions
        if (left && !right) x_pos -= 25;
        if (right && !left) x_pos += 25;
        if (downcount == 0) {
        	y_pos += 25;
        	downcount = 10;
        } else {
        	downcount--;
        }
        
        // collision detection with bounds
        if (x_pos <= 25) x_pos = 25;
        if (y_pos <= 25) y_pos = 25;
        if (x_pos >= WINDOW_WIDTH - self.w - 175) x_pos = WINDOW_WIDTH - self.w - 175;
        if (y_pos >= WINDOW_HEIGHT - self.h - 25)  y_pos = WINDOW_HEIGHT - self.h - 25;
        
        // grid positions
        curs_grid_y = (y_pos / 25) - 1;
        curs_grid_x = (x_pos / 25) - 1;

        // DEBUG
        printf("Curs: {x = %d}, {y = %d}\n, rotation = %d", curs_grid_x, curs_grid_y, curr_piece->rotation);

        // clear the window
        SDL_RenderClear(rend);

        // render background
        SDL_RenderCopy(rend, bg_texture, NULL, &bg);
        
        // redraw board
        for (int i = 0; i < 10; i++) {
        	for (int j = 0; j < 20; j++) {
        		self.x = ((i + 1) * 25);
        		self.y = ((j + 1) * 25);
            	switch (board[i][j]) {
					case 1:
					SDL_QueryTexture(single_blocks[0], NULL, NULL, &self.w, &self.h);
					SDL_RenderCopy(rend, t_s0, NULL, &self);
					break;
					case 2:
					SDL_QueryTexture(single_blocks[1], NULL, NULL, &self.w, &self.h);
					SDL_RenderCopy(rend, t_s1, NULL, &self);
					break;
					case 3:
					SDL_QueryTexture(single_blocks[2], NULL, NULL, &self.w, &self.h);
					SDL_RenderCopy(rend, t_s2, NULL, &self);
					break;
					case 4:
					SDL_QueryTexture(single_blocks[3], NULL, NULL, &self.w, &self.h);
					SDL_RenderCopy(rend, t_s3, NULL, &self);
					break;
					case 5:
					SDL_QueryTexture(single_blocks[4], NULL, NULL, &self.w, &self.h);
					SDL_RenderCopy(rend, t_s4, NULL, &self);
					break;
					case 6:
					SDL_QueryTexture(single_blocks[5], NULL, NULL, &self.w, &self.h);
					SDL_RenderCopy(rend, t_s5, NULL, &self);
					break;
					case 7:
					SDL_QueryTexture(single_blocks[6], NULL, NULL, &self.w, &self.h);
					SDL_RenderCopy(rend, t_s6, NULL, &self);
					break;
        		}
        	}
    	}

    	// handle cursor position and render
    	self.y = (int) y_pos;
        self.x = (int) x_pos;
        SDL_QueryTexture(self_texture, NULL, NULL, &self.w, &self.h);
        SDL_RenderCopy(rend, self_texture, NULL, &self);


        // display current frame
        SDL_RenderPresent(rend);

        // wait 1/60th of a second
        SDL_Delay(1000/20);
    }
    
    // clean up resources before exiting
    SDL_DestroyTexture(bg_texture);
   // SDL_DestroyTexture(self_texture);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
}




void print_board() {
	for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 10; j++) {
            printf("%d", board[j][i]);
        }
        printf("\n");
    }
}

void clear_board() {
	for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 10; j++) {
            board[j][i] = 0;
        }
    }
}


void fill_bag() {
    bool filled = false;
    bool found = false;
    while (!filled) {
        int num = (rand() % (7 - 1 + 1)) + 1;
        for (int i = 0; i < 7; i++) {
            if (pieces[i] == num) {
                found = true;
                break;
            }
        }
        if (!found || pieces[6] != -1) {
            for (int i = 0; i < 7; i++) {
                if (pieces[i] == -1) {
                    pieces[i] = num;
                    found = true;
                    break;
                } else {
                    found = false;
                }
            }
            if (!found) {
                filled = true;
            }
        }
        found = false;
    }
}


struct piece *select_piece() {
	bool found = false;
	int i;
	for (i = 0; i < 7; i++) {
		if (pieces[i] == -1) {
			continue;
		} else {
			found = true;
			break;
		}
	}
	if (found) {
		switch (pieces[i]) {
			case 1:
				pieces[i] = -1;
				return &I;
			break;
			case 2:
				pieces[i] = -1;
				return &O;
			break;
			case 3:
				pieces[i] = -1;
				return &T;
			break;
			case 4:
				pieces[i] = -1;
				return &S;
			break;
			case 5:
				pieces[i] = -1;
				return &Z;
			break;
			case 6:
				pieces[i] = -1;
				return &J;
			break;
			case 7:
				pieces[i] = -1;
				return &L;
			break;
		}
	} else {
		fill_bag();
	}
	return &I;

}



void set_piece_orientation(struct piece *p) {
	switch(p->type) {

		// I
		case 1:
			switch(p->rotation) {
				case 0:
					b1.x = self.x;
					b1.y = self.y;

					b2.x = self.x;
					b2.y = self.y;

					b3.x = self.x;
					b3.y = self.y;

					b4.x = self.x;
					b4.y = self.y;
				break;
				case 1:
				break;
				case 2:
				break;
				case 3:
				break;
			}
		break;

		// O
		case 2:
			switch(p->rotation) {
				case 0:
				break;
				case 1:
				break;
				case 2:
				break;
				case 3:
				break;
			}
		break;

		// T
		case 3:
			switch(p->rotation) {
				case 0:
				break;
				case 1:
				break;
				case 2:
				break;
				case 3:
				break;
			}
		break;

		// S
		case 4:
			switch(p->rotation) {
				case 0:
				break;
				case 1:
				break;
				case 2:
				break;
				case 3:
				break;
			}
		break;

		// Z
		case 5:
			switch(p->rotation) {
				case 0:
				break;
				case 1:
				break;
				case 2:
				break;
				case 3:
				break;
			}
		break;

		// J
		case 6:
			switch(p->rotation) {
				case 0:
				break;
				case 1:
				break;
				case 2:
				break;
				case 3:
				break;
			}
		break;

		// L
		case 7:
			switch(p->rotation) {
				case 0:
				break;
				case 1:
				break;
				case 2:
				break;
				case 3:
				break;
			}
		break;
	}
}



