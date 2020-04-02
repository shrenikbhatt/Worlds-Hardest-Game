#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define N 7
#define SIZE 4
#define player_colour 0xF800 // red
#define obstacle_colour 0x001F // blue

void clear_screen();
void draw_line(int x0, int y0, int x1, int y1, short int color);
void plot_pixel(int x, int y, short int line_color);
void wait();
void plot_obstacles();
void plot_player();
void clear_obstacles();
bool check_in_bounds();


volatile int pixel_buffer_start; // global variable
volatile int pixel_backBuffer_start;

// define the number of obstacles + player
int x[N], y[N], incx[N], incy[N];

int main(){
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;

    // Set the initial positions of the obstacles
    for (int i = 0; i < N-1; ++i){
        if (i%2 == 0){
            y[i] = 1;
            incy[i] = 1;
        }
        else {
            y[i] = 235;
            incy[i] = -1;
        }
        x[i] = 20 + 55*i;
    }

    // Set position for player
    x[N-1] = 8; 
    y[N-1] = 118;


     *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen();


    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clear_screen();

    
    while (true){
        // Clear obstacles
        clear_obstacles();
        // Plot obstacles
        plot_obstacles();

        // plot player
        plot_player();

        wait(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
        
    }

    return 0;
}


// Screen draw functions
void wait() {
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020; // Buffer register
    register int status;

    *pixel_ctrl_ptr = 1; // start the synchronization process

    status = *(pixel_ctrl_ptr + 3);
    while ((status & 0x01) != 0) {
        status = *(pixel_ctrl_ptr + 3);
    }
}

void clear_screen() {
    for (int x = 0; x < 320; x++){
        for (int y = 0; y < 240; y++){
            if ((x >= 0 && x < 20) || (x >= 300 && x < 320)){
                if (y >= 110 && y < 130){
                    plot_pixel(x, y, 0x07E0); 
                }
                else{
                    plot_pixel(x, y, 0xFFFF);   
                }
            }
            else{
                plot_pixel(x, y, 0xFFFF);   
            }
        }
    }
}

void plot_pixel(int x, int y, short int line_color){
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}


void clear_obstacles(){
    for (int i = 0; i < N-1; ++i){
        if (y[i] >= 1 && y[i] <= 235){
            if (incy[i] == 1){
                for (int j = 0; j < SIZE; ++j){
                    for (int k = 0; k < SIZE; ++k){
                        if (y[i] + k - SIZE < 0){
                            plot_pixel(x[i] + j, 0 , 0xFFFF);
                        }
                        else
                            plot_pixel(x[i] + j, y[i] + k - SIZE , 0xFFFF);
                    }
                }
            }
            else{
                for (int j = 0; j < SIZE; ++j){
                    for (int k = 0; k < SIZE; ++k){
                        plot_pixel(x[i] + j, y[i] + k + SIZE , 0xFFFF);
                    }
                }
            }
            
        }            
    }
}

void plot_obstacles(){
    for (int i = 0; i < N-1; ++i){
        for (int j = 0; j < SIZE; ++j){
            for (int k = 0; k < SIZE; ++k){
                plot_pixel(x[i] + j, y[i] + k, obstacle_colour);
            }
        }
        if (y[i] == 1) incy[i] = 1;
        else if (y[i] == 235) incy[i] = -1;

        y[i] += incy[i];
    }
}

void plot_player(){
    for (int i = 0; i < SIZE; ++i){
        for (int j = 0; j < SIZE; ++j){
            plot_pixel(x[N-1] + i, y[N-1] + j, player_colour);
        }
    }
}

/* GAME LOGIC */

bool check_in_bounds(){
    if (x[N-1] < 0 || x[N-1] >= 320 || y[N-1] < 0 || y[N-1] >= 240){
        return false;
    }
    return true;
}