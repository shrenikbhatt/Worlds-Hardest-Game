#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define M 10
#define NUM_COINS 3
#define SIZE 6
#define PLAYER_COLOUR 0xF800 // red
#define OBSTACLE_COLOUR 0x001F // blue
#define COINS_COLOR 0xFF00

#define PLATFORM_SIZE 20

void clear_screen();
void draw_line(int x0, int y0, int x1, int y1, short int color);
void plot_pixel(int x, int y, short int line_color);
void wait();
void plot_obstacles(int level);
void plot_coins();
void plot_player();
void clear_obstacles();
bool check_in_bounds();
bool player_hit();
void init_player();
void clear_player();
int collected_coin();
bool check_win(int coin_count);
void write_string(int a, int b, char c);
void write_opening_display();
void write_next_level_display();
void write_clear_display();
void write_finished_display();
void colour_screen(short int colour);


void disable_A9_interrupts(void);
void set_A9_IRQ_stack(void);
void config_GIC(void);
void config_KEYs(void);
void enable_A9_interrupts(void);

volatile int pixel_buffer_start; // global variable
volatile int pixel_backBuffer_start;

// define the number of obstacles + player
int x[M], y[M], incx[M], incy[M];
int coin_x[NUM_COINS], coin_y[NUM_COINS];
bool coin_exists[NUM_COINS];

int old_pos[2];

int main(){
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    volatile int * sw_ptr = (int *) 0xFF200040;

    disable_A9_interrupts();// disable interrupts in the A9 processor
    set_A9_IRQ_stack();// initialize the stack pointer for IRQ mode
    config_GIC();// configure the general interrupt controller
    config_KEYs();// configure pushbutton KEYs to generate interrupts 
    
    enable_A9_interrupts();// enable interrupts in the A9 processor


    // Level 1

    // Set the initial positions of the obstacles
    for (int i = 0; i < M-1; ++i){
        if (i%2 == 0){
            y[i] = 1;
            incy[i] = 1;
        }
        else {
            y[i] = 235;
            incy[i] = -1;
        }
        incx[i] = 0;
        x[i] = PLATFORM_SIZE + ((320-PLATFORM_SIZE)/(M-1))*i;
    }

    // Set position for player
    init_player();
    old_pos[0] = 8; 
    old_pos[1] = 118;

    // Set position for coins
    for (int i = 0; i < NUM_COINS; i++){
        coin_exists[i] = true;
        // coin_x[i] = 25+ i * 67;
        // coin_y[i] = 10 + i*55;
        coin_x[i] = rand() % 300 + 20;
        coin_y[i] = rand() % 210 + 5;

    }


     *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;

    // Introduction
    colour_screen(0x0000);
    int prev = *sw_ptr;
     while (*sw_ptr == prev){
         write_opening_display();
     }

     write_clear_display();

    clear_screen();
    

    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clear_screen();

    // Code to write SW to HEX display
    int count = 0;
    volatile int* HEX3_0_ptr = (int *) 0xFF200020;
    char seg7[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67}; // 0 - 9

    int coin_count = 0;


  /*  while (!check_win(coin_count)){
        // coins
        plot_coins();

        // player
        clear_player();
        plot_player();


        // obstacles
        clear_obstacles();
        plot_obstacles(1);
        
        if (player_hit()){
            count++;
            init_player();
            coin_count = 0;
            for (int i = 0; i < NUM_COINS; i++){
                coin_exists[i] = true;
            }
        }
        // check if player collides into a coin
        int coin = collected_coin();
        if(coin != -1){
            coin_count++;
        }

        // Reset death count after 100 deaths.
        if (count == 100){
            count = 0;
        }

        // Death Count (up until 99)
        if (count >= 10){
            *HEX3_0_ptr = (seg7[(int) (count / 10)] << 8) | seg7[count % 10] ;
        }
        else{
            *HEX3_0_ptr = (seg7[0] << 8) | seg7[count];
        }

        wait(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
        
    } */

    // Level 2
    // Set the initial positions of the obstacles
    for (int i = 0; i < M-1; ++i){
        if (i%2 == 0){
            y[i] = 1;
            incy[i] = 2;
        }
        else {
            y[i] = 235;
            incy[i] = -2;
        }
        incx[i] = 0;
        x[i] = PLATFORM_SIZE + ((320-PLATFORM_SIZE)/(M-1))*i;
    }

    // Set position for player
    init_player();
    old_pos[0] = 8; 
    old_pos[1] = 118;

    // Set position for coins
    for (int i = 0; i < NUM_COINS; i++){
        coin_exists[i] = true;
        // coin_x[i] = 25+ i * 67;
        // coin_y[i] = 10 + i*55;
        coin_x[i] = rand() % 300 + 20;
        coin_y[i] = rand() % 210 + 5;

    }


     *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;

    
    colour_screen(0x0000);
    

    prev = *sw_ptr;

     while (*sw_ptr == prev){
         write_next_level_display();
     }

    write_clear_display();
    clear_screen();
    

    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clear_screen();

    count = 0;

    coin_count = 0;


  /*  while (!check_win(coin_count)){
        // coins
        plot_coins();

        // player
        clear_player();
        plot_player();


        // obstacles
        clear_obstacles();
        plot_obstacles(2);
        
        if (player_hit()){
            count++;
            init_player();
            coin_count = 0;
            for (int i = 0; i < NUM_COINS; i++){
                coin_exists[i] = true;
            }
        }
        // check if player collides into a coin
        int coin = collected_coin();
        if(coin != -1){
            coin_count++;
        }

        // Reset death count after 100 deaths.
        if (count == 100){
            count = 0;
        }

        // Death Count (up until 99)
        if (count >= 10){
            *HEX3_0_ptr = (seg7[(int) (count / 10)] << 8) | seg7[count % 10] ;
        }
        else{
            *HEX3_0_ptr = (seg7[0] << 8) | seg7[count];
        }

        wait(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
        
    } */


    // Level 3
     for (int i = 0; i < M-1; ++i){
        if (i%2 == 0){
            y[i] = 1;
            incy[i] = 3;
        }
        else {
            y[i] = 235;
            incy[i] = -3;
        }
        incx[i] = 0;
        x[i] = PLATFORM_SIZE + ((320-PLATFORM_SIZE)/(M-1))*i;
    }

    // Set position for player
    init_player();
    old_pos[0] = 8; 
    old_pos[1] = 118;

    // Set position for coins
    for (int i = 0; i < NUM_COINS; i++){
        coin_exists[i] = true;
        // coin_x[i] = 25+ i * 67;
        // coin_y[i] = 10 + i*55;
        coin_x[i] = rand() % 300 + 20;
        coin_y[i] = rand() % 210 + 5;

    }


     *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;

    colour_screen(0x0000);
    

    prev = *sw_ptr;

     while (*sw_ptr == prev){
         write_next_level_display();
     }

    write_clear_display();
    clear_screen();
    

    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clear_screen();

    count = 0;

    coin_count = 0;


    while (!check_win(coin_count)){
        // coins
        plot_coins();

        // player
        clear_player();
        plot_player();


        // obstacles
        clear_obstacles();
        plot_obstacles(3);
        
        if (player_hit()){
            count++;
            init_player();
            coin_count = 0;
            for (int i = 0; i < NUM_COINS; i++){
                coin_exists[i] = true;
            }
        }
        // check if player collides into a coin
        int coin = collected_coin();
        if(coin != -1){
            coin_count++;
        }

        // Reset death count after 100 deaths.
        if (count == 100){
            count = 0;
        }

        // Death Count (up until 99)
        if (count >= 10){
            *HEX3_0_ptr = (seg7[(int) (count / 10)] << 8) | seg7[count % 10] ;
        }
        else{
            *HEX3_0_ptr = (seg7[0] << 8) | seg7[count];
        }

        wait(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
        
    }

    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;

    colour_screen(0x0000);

    bool finished = true;

     while (finished){
         write_finished_display();
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
            if ((x >= 0 && x < PLATFORM_SIZE) || (x >= 320 - PLATFORM_SIZE && x < 320)){
                if (y >= (240/2)-PLATFORM_SIZE/2 && y < (240/2)+PLATFORM_SIZE/2){
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

void colour_screen(short int colour) {
    for (int i = 0; i < 320; i++){
        for (int j = 0; j < 240; j++){
                plot_pixel(i, j, 0x0000);
        }
    }
}

void plot_pixel(int x, int y, short int line_color){
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}


void clear_obstacles(){
    for (int i = 0; i < M-1; ++i){
        if (y[i] >= 1 && y[i] <= 235 && incy[i] != 0){
            if (incy[i] >= 1){
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
        else if (x[i] >= PLATFORM_SIZE && x[i] <= 318-PLATFORM_SIZE && incx[i] != 0){
            if (incx[i] == 2){
                for (int j = 0; j < SIZE; ++j){
                    for (int k = 0; k < SIZE; ++k){
                        if (x[i] + k - SIZE < PLATFORM_SIZE){
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

void clear_player(){
    for (int i = 0; i < SIZE; ++i){
        for (int j = 0; j < SIZE; ++j){
            if ((old_pos[0] + i >= 0 && old_pos[0] + i < PLATFORM_SIZE) || (old_pos[0] + i >= 320 - PLATFORM_SIZE && old_pos[0] + i < 320)){
                if (old_pos[1] + j >= (240/2)-PLATFORM_SIZE/2 && old_pos[1] + j < (240/2)+PLATFORM_SIZE/2){
                    plot_pixel(old_pos[0]+i, old_pos[1]+j, 0x07E0); 
                }
                else{
                    plot_pixel(old_pos[0]+i, old_pos[1]+j, 0xFFFF);   
                }
            }
            else{
                plot_pixel(old_pos[0]+i, old_pos[1]+j, 0xFFFF);   
            }
        }
    }
}

void plot_obstacles(int level){
    for (int i = 0; i < M-1; ++i){
        for (int j = 0; j < SIZE; ++j){
            for (int k = 0; k < SIZE; ++k){
                plot_pixel(x[i] + j, y[i] + k, OBSTACLE_COLOUR);
            }
        }
        if (incx[i] == 0){
            if (y[i] <= 1) incy[i] = level;
            else if (y[i] >= 235) incy[i] = -1*level;
            y[i] += incy[i];
        }
        // else if (incy[i] == 0){
        //     if (x[i] <= PLATFORM_SIZE+2) incx[i] = 2;
        //     else if (x[i] >= 318-PLATFORM_SIZE) incx[i] = -2;
        //     x[i] += incx[i];
        // }
        
        
    }
}

void plot_player(){
    for (int i = 0; i < SIZE; ++i){
        for (int j = 0; j < SIZE; ++j){
            plot_pixel(x[M-1] + i, y[M-1] + j, PLAYER_COLOUR);
        }
    }
}

void plot_coins(){
    for (int a = 0; a < NUM_COINS; ++a){
        for (int i = 0; i < SIZE; ++i){
            for (int j = 0; j < SIZE; ++j){
                int x = coin_x[a];
                int y = coin_y[a]; 
                if (coin_exists[a]){
                    plot_pixel(x+i, y+j, COINS_COLOR);
                }
                else {
                    plot_pixel(x+i, y+j, 0xFFFF);
                }
            }
        }
    }

}

void init_player(){
    x[M-1] = 8; 
    y[M-1] = 118;
}

void write_string(int a, int b, char c) {
  // VGA character buffer
  volatile char * string_output_buffer = (char *) (0xc9000000 + (b<<7) + a);
  *string_output_buffer = c;

}

void write_opening_display() {
	
   	char* game_name = "Worlds Hardest Game!";
   	int starting_x_position = 155;
   	while (*game_name) {
     		write_string(starting_x_position, 14, *game_name);
	 	starting_x_position++;
	 	game_name++;
   	}

	char* names = "Made by: Shrenik and Mohammad";
   	starting_x_position = 155;
   	while (*names) {
     		write_string(starting_x_position, 18, *names);
	 	starting_x_position++;
	 	names++;
   	}

	char* game_rules1 = "Key 0, GO DOWN";
   	starting_x_position = 155;
   	while (*game_rules1) {
     		write_string(starting_x_position, 22, *game_rules1);
	 	starting_x_position++;
	 	game_rules1++;
   	}

	char* game_rules2 = "Key 1, GO RIGHT";
   	starting_x_position = 155;
   	while (*game_rules2) {
     		write_string(starting_x_position, 26, *game_rules2);
	 	starting_x_position++;
	 	game_rules2++;
   	}

	char* game_rules3 = "Key 2, GO LEFT";
   	starting_x_position = 155;
   	while (*game_rules3) {
     		write_string(starting_x_position, 30, *game_rules3);
	 	starting_x_position++;
	 	game_rules3++;
   	}

	char* game_rules4 = "Key 3, GO UP";
   	starting_x_position = 155;
   	while (*game_rules4) {
     		write_string(starting_x_position, 34, *game_rules4);
	 	starting_x_position++;
	 	game_rules4++;
   	}
}

void write_clear_display() {
	
   	char* game_name = "                         ";
   	int starting_x_position = 155;
   	while (*game_name) {
     		write_string(starting_x_position, 14, *game_name);
	 	starting_x_position++;
	 	game_name++;
   	}

	char* names = "                                 ";
   	starting_x_position = 155;
   	while (*names) {
     		write_string(starting_x_position, 18, *names);
	 	starting_x_position++;
	 	names++;
   	}

	char* game_rules1 = "                              ";
   	starting_x_position = 155;
   	while (*game_rules1) {
     		write_string(starting_x_position, 22, *game_rules1);
	 	starting_x_position++;
	 	game_rules1++;
   	}

	char* game_rules2 = "                                 ";
   	starting_x_position = 155;
   	while (*game_rules2) {
     		write_string(starting_x_position, 26, *game_rules2);
	 	starting_x_position++;
	 	game_rules2++;
   	}

	char* game_rules3 = "                                  ";
   	starting_x_position = 155;
   	while (*game_rules3) {
     		write_string(starting_x_position, 30, *game_rules3);
	 	starting_x_position++;
	 	game_rules3++;
   	}

	char* game_rules4 = "                                    ";
   	starting_x_position = 155;
   	while (*game_rules4) {
     		write_string(starting_x_position, 34, *game_rules4);
	 	starting_x_position++;
	 	game_rules4++;
   	}

		
}

void write_next_level_display() {
	
   	char* game_name = "Nice!! Lets go faster!";
   	int starting_x_position = 155;
   	while (*game_name) {
     		write_string(starting_x_position, 14, *game_name);
	 	starting_x_position++;
	 	game_name++;
   	}	
}

void write_finished_display() {
	
   	char* game_name = "Congrats! You Win!";
   	int starting_x_position = 155;
   	while (*game_name) {
     		write_string(starting_x_position, 14, *game_name);
	 	starting_x_position++;
	 	game_name++;
   	}	
}


/* GAME LOGIC FUNCTIONS */

bool check_in_bounds(){
    if (x[M-1] < 0 || x[M-1] >= (320 - SIZE) || y[M-1] < 0 || y[M-1] >= (240-SIZE)){
        return false;
    }
    return true;
}

bool player_hit(){
    for (int i = 0; i < M-1; ++i){
        if ((x[M-1] + SIZE  >= x[i]) && (x[M-1] <= x[i]+ SIZE)){
            if ((y[M-1] + SIZE  >= y[i]) && (y[M-1] <= y[i] + SIZE)){
                old_pos[0] = x[M-1]; 
                old_pos[1] = y[M-1];
    			return true;  
            }
        }
    }
    return false;
}


int collected_coin(){
    for (int i = 0; i < NUM_COINS; ++i){
        if ((x[M-1] + SIZE  >= coin_x[i]) && (x[M-1] <= coin_x[i]+ SIZE)){
            if ((y[M-1] + SIZE  >= coin_y[i]) && (y[M-1] <= coin_y[i] + SIZE)){
                if (coin_exists[i]){
                    coin_exists[i] = false;
                    return i;
                }     
            }
        }
    }
    return -1;
}

bool check_win(int coin_count){
    if (coin_count == NUM_COINS){
        if ((x[M-1] >= 0 && x[M-1] < PLATFORM_SIZE) || (x[M-1] >= 320 - PLATFORM_SIZE && x[M-1] < 320)){
            if (y[M-1] >= (240/2)-PLATFORM_SIZE/2 && y[M-1] < (240/2)+PLATFORM_SIZE/2){
                return true; 
            }
        }
    }
    return false;
}



/* !!!! The below code was referenced and modified from the Intel DE1-SOC manual. Credit to them for providing a base template we would be
    able to use for our own game.  !!!! */

/*setup the KEY interrupts in the FPGA*/
void config_KEYs() {
    volatile int *KEY_ptr = (int*) 0xFF200050;// pushbutton KEY base address
    *(KEY_ptr + 2) = 0xF;// enable interrupts for the two KEYs
}
    
/*This file:
*1. defines exception vectors for the A9 processor
*2. provides code that sets the IRQ mode stack, and that dis/enables*interrupts
*3. provides code that initializes the generic interrupt controller*/

void pushbutton_ISR(void);
void config_interrupt(int,int);

// Define the IRQ exception handler

void __attribute__((interrupt)) __cs3_isr_irq(void) {

    // Read the ICCIAR from the CPU Interface in the GIC
    int interrupt_ID =*((int*)0xFFFEC10C);
    if(interrupt_ID == 73)  // check if interrupt is from the KEYs
        pushbutton_ISR();
    else
        while(1); // if unexpected, then stay here
        
    // Write to the End of Interrupt Register (ICCEOIR)
    *((int*)0xFFFEC110) = interrupt_ID;
}

/**Turn off interrupts in the ARM processor*/

void disable_A9_interrupts(void) {
    int status = 0b11010011;
    asm("msr cpsr, %[ps]" : : [ps] "r"(status));
}

/**Initialize the banked stack pointer register for IRQ mode*/

void set_A9_IRQ_stack(void) {
    
    int stack, mode;
    stack = 0xFFFFFFFF - 7;// top of A9 onchip memory, aligned to 8 bytes/*change processor to IRQ mode with interrupts disabled*/
    mode = 0b11010010;
    asm("msr cpsr, %[ps]" : : [ps] "r"(mode));
    /*set banked stack pointer*/
    asm("mov sp, %[ps]" : : [ps] "r"(stack));
    
    /*go back to SVC mode before executing subroutine return!*/
    mode = 0b11010011;
    asm("msr cpsr, %[ps]" : : [ps] "r"(mode));
}

/**Turn on interrupts in the ARM processor*/
void enable_A9_interrupts(void) {
    int status = 0b01010011;
    asm("msr cpsr, %[ps]" : : [ps] "r"(status));
}

/**Configure the Generic Interrupt Controller (GIC)*/
void config_GIC(void) {
    config_interrupt (73, 1);
    // configure the FPGA KEYs interrupt (73)
    
    // Set Interrupt Priority Mask Register (ICCPMR). Enable interrupts of all priorities
    *((int*) 0xFFFEC104) = 0xFFFF;
    
    // Set CPU Interface Control Register (ICCICR). Enable signaling of interrupts
    *((int*) 0xFFFEC100) = 1;
    
    // Configure the Distributor Control Register (ICDDCR) to send pending interrupts to CPUs
    *((int*) 0xFFFED000) = 1;
}

/**Configure Set Enable Registers (ICDISERn) and Interrupt Processor Target*Registers (ICDIPTRn). The default (reset) values are used for other registers*in the GIC.*/
void config_interrupt(int N,int CPU_target) {
    int reg_offset, index, value, address;
    /*Configure the Interrupt Set-Enable Registers (ICDISERn).*reg_offset = (integer_div(N / 32)*4*value = 1 << (N mod 32)*/
    reg_offset = (N >> 3) & 0xFFFFFFFC;
    index      = N & 0x1F;
    value      = 0x1 << index;
    address    = 0xFFFED100 + reg_offset;
    /*Now that we know the register address and value, set the appropriate bit*/
    *(int*)address |= value;
    
    /*Configure the Interrupt Processor Targets Register (ICDIPTRn)*reg_offset = integer_div(N / 4)*4*index = N mod 4*/
    reg_offset = (N & 0xFFFFFFFC);
    index      = N & 0x3;
    address    = 0xFFFED800 + reg_offset + index;
    /*Now that we know the register address and value, write to (only) the*appropriate byte*/
    *(char*)address = (char)CPU_target;
}

/*********************************************************************
Pushbutton - Interrupt Service Routine
*
*This routine checks which KEY has been pressed.
*******************************************************************/
void pushbutton_ISR(void) {
    /*KEY base address*/
    volatile int*KEY_ptr = (int*) 0xFF200050;
    int press;
    press          =*(KEY_ptr + 3);// read the pushbutton interrupt register
    *(KEY_ptr + 3) = press;// Clear the interrupt

    old_pos[0] = x[M-1];
    old_pos[1] = y[M-1];

    if(press & 0x1){ // KEY0
        y[M-1] = y[M-1] + SIZE+1; 
        if(!check_in_bounds()){
            y[M-1] = y[M-1] - SIZE+1; 
        }

    }
    else if(press & 0x2){// KEY1
        x[M-1] = x[M-1] + SIZE+1;
        if(!check_in_bounds()){
            x[M-1] = x[M-1] - SIZE+1;
        }
    }
    else if(press & 0x4){ // KEY2
        x[M-1] = x[M-1] - (SIZE+1);
        if(!check_in_bounds()){
            x[M-1] = x[M-1] + (SIZE+1);
        }
    }
    else{   // press & 0x8, which is KEY3
        y[M-1] = y[M-1] - (SIZE+1);
        if(!check_in_bounds()){
            y[M-1] = y[M-1] + (SIZE+1);
        }
    }
        

    return;
}




