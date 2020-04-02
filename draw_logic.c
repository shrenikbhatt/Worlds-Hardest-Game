#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define N 7
#define SIZE 4

void clear_screen();
void draw_line(int x0, int y0, int x1, int y1, short int color);
void plot_pixel(int x, int y, short int line_color);
void wait();

volatile int pixel_buffer_start; // global variable
volatile int pixel_backBuffer_start;

void swap(int * x, int * y){
	int temp = *x;
    *x = *y;
    *y = temp;   
}

int main(){
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;

    // define the number of obstacles + player
    int x[N], y[N], incx[N], incy[N];


    int player_colour = 0xF800; // player will be red
    int obstacle_colour = 0x001F; // obstacles will be blue

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
        x[i] = 18 + 56*i;
    }

    // Set position for player
    x[N-1] = 0; 
    y[N-1] = 120;


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
        for (int i = 0; i < N-1; ++i){
            if (y[i] >= SIZE && y[i] <= 235){
                if (incy[i] == 1){
                    for (int j = 0; j < SIZE; ++j){
                        for (int k = 0; k < SIZE; ++k){
                            plot_pixel(x[i] + j, y[i] + k - SIZE , 0x0);
                        }
                    }
                }
                else{
                    for (int j = 0; j < SIZE; ++j){
                        for (int k = 0; k < SIZE; ++k){
                            plot_pixel(x[i] + j, y[i] + k + SIZE , 0x0);
                        }
                    }
                }
                
            }
            else{
                clear_screen();
            }

            for (int j = 0; j < SIZE; ++j){
                for (int k = 0; k < SIZE; ++k){
                    plot_pixel(x[i] + j, y[i] + k, obstacle_colour);
                }
            }
            if (y[i] == 1) incy[i] = 1;
            else if (y[i] == 235) incy[i] = -1;

            y[i] += incy[i];
            
        }
        
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
    for (int x = 0; x < 320; x++)
        for (int y = 0; y < 240; y++)
            plot_pixel(x, y, 0x0);   
}

void plot_pixel(int x, int y, short int line_color){
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

void draw_line (int x0, int y0, int x1, int y1, short int color) {
    bool is_steep = abs(y1 - y0) > abs (x1 -x0);
    if (is_steep) {
        swap(&x0, &y0);
        swap(&x1, &y1);
    }
    if (x0 > x1) {
        swap(&x0, &x1);
        swap(&y0, &y1);
    }

    int deltax = x1 - x0;
    int deltay = abs(y1 - y0);
    int error = -(deltax / 2);
    int y = y0;
    int y_step;
    if (y0 < y1) {
        y_step = 1;
    } else {
        y_step = -1;
    }

    for (int x = x0; x <= x1; x++) {
        if (is_steep) {
            plot_pixel(y, x, color);
        } else {
            plot_pixel(x, y, color);
        }
        error += deltay;
        if (error >= 0) {
            y += y_step;
            error -= deltax;
        }
    }
}