#include <stdio.h>
#include  "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "hardware/structs/bus_ctrl.h"
#include "pico/multicore.h"
#include "z80io.pio.h"
#include "hardware/pio.h"
#include "hardware/irq.h"

/* Real UART setup*/
#define UART_ID uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 0
#define UART_RX_PIN 1


//trace GPIO
#define TRACEGPIO 27
int told=0;        

//Registers

uint8_t Registers[8];

//pico pio globals
uint8_t registers[16];
PIO pio = pio0;  
uint sm_z80io; 

int trace=0;

//pico LED
const uint LEDPIN = PICO_DEFAULT_LED_PIN;

void flash_led(int t){
  //flash LED
  gpio_put(LEDPIN, 1);
  sleep_ms(t);
  gpio_put(LEDPIN, 0);
  sleep_ms(t);

}


//isr currently not used (I cant get it to work,I don't think it will be any faster either.)
void isr(){
    printf(" here ");
    pio_sm_put(pio,sm_z80io,0xaa);
//    pio_interrupt_clear(pio0, 0);
    irq_clear(PIO0_IRQ_0);
}



//second core handles z80 pio requests
void z80io_core_entry() {
//        float freq = 60000000.0; 
        float freq =   80000000.0; 
	float div = (float)clock_get_hz(clk_sys) / freq;
	uint offset_z80io = pio_add_program(pio, &z80io_program);

	sm_z80io= pio_claim_unused_sm(pio, true);	
	z80io_init(pio, sm_z80io, offset_z80io, div);
	pio_sm_set_enabled(pio, sm_z80io, true);

	uint32_t io = 0;
	uint32_t c=0;
	uint16_t addr=0;
	uint8_t data=0;

	sleep_ms(2000);  //wait for core 1 to finish setup

//	printf("\nOK\n\r");

/*	
        unused interrupt stuff.
        pio_set_irq0_source_enabled(pio, pis_interrupt0, true);
        irq_set_exclusive_handler(PIO0_IRQ_0, isr);
        irq_set_enabled(PIO0_IRQ_0, true);
*/
    	while (1) {
            
//z80read Pico SENDS data
	    if(pio_interrupt_get(pio, 5)){
              io = pio_sm_get(pio, sm_z80io);
	      addr=(io >>10) & 0x07;
	      
	      data=Registers[addr];

	      pio_sm_put(pio,sm_z80io,data);
	      pio_interrupt_clear(pio, 5);
	      if (trace  ) {printf("io:%04X Wr reg[%02X]->%02X %03i\n\r",io,addr,data,data);}
	    }
	    
//z80write Pico RECEIVES data
	    if(pio_interrupt_get(pio, 6)){
              io = pio_sm_get(pio, sm_z80io);

              pio_interrupt_clear(pio, 6);

	      addr=(io >>10) & 0x07;
	      data=(io & 0x00ff);

              Registers[addr]=data;
              
              if (trace  ){printf("io:%04X Rd reg[%02X]<-%02X \n\r",io,addr,data);}
	    }
	    		
//trace switch
/*            if (gpio_get(TRACEGPIO)==1){
              trace=0;
            }else{
              trace=1;
            }  
	    if(told!=trace){
	      printf("Trace: %X\n\r",trace);
              sleep_ms(500);
	      told=trace;
            }
*/
            tight_loop_contents();	
		
	}

}

int main(){
	stdio_init_all();
	printf("\r\nz80disk\n\r\n");
//over clock
        set_sys_clock_khz(250000, true);


//uart

    // Set up our UART with a basic baud rate.
    uart_init(UART_ID, 2400);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Actually, we want a different speed
    // The call will return the actual baud rate selected, which will be as close as
    // possible to that requested
    int __unused actual = uart_set_baudrate(UART_ID, BAUD_RATE);

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(UART_ID, false, false);

    // Set our data format
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);


//LED
        gpio_init(LEDPIN);
        gpio_set_dir(LEDPIN, GPIO_OUT);
        flash_led(200);        
//banner
    sleep_ms(2000);

        printf( "\n\n\r     ________________________________");
        printf(   "\n\r    /                                |");
        printf(   "\n\r   /           RC2014                |");
        printf(   "\n\r  /        SD SPO 256 AL2            |");
        printf(   "\n\r |  O                                |");
        printf(   "\n\r |          Derek Woodroffe          |");
        printf(   "\n\r |              2022                 |");
        //printf(   "\n\r |                                   |");
        printf(   "\n\r |___________________________________|");
        printf(   "\n\r   | | | | | | | | | | | | | | | | |  \n\n\r");
	
        flash_led(200);

//IO setup
	gpio_init(14);
	gpio_init(15);
	gpio_set_dir(14,GPIO_IN);
	gpio_set_dir(15,GPIO_IN);

        // trace switch
        gpio_init(TRACEGPIO);
        gpio_set_dir(TRACEGPIO,GPIO_IN);
        gpio_pull_up(TRACEGPIO);



        printf("\n\rTrace:%i",trace);
        printf("\n\r\n\r OK lets start the z80 stuff\n\r");
//launch core 1
        multicore_launch_core1(z80io_core_entry);
	  
	  if (trace==0){ 
	   printf("\n\r Trace zero, so no more info shown, \n\r Keep the speed up and the noise down\n\r SSShhh!!!!!....\n\r");

        }

        flash_led(200);
        

        flash_led(60);
        flash_led(60);
        flash_led(60);
//twiddle thumbs
	while (1) {

/*
            if (gpio_get(TRACEGPIO)==1){
              trace=0;
            }else{
              trace=1;
            }  
	    if(told!=trace){
	      printf("Trace: %X\n\r",trace);
              sleep_ms(500);
	      told=trace;
            }
*/


	}
	
}

