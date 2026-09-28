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
#include "hardware/pwm.h"

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


//trace GPIO needs to be compiled, commented out for speed
#define TRACEGPIO 27
int told=0;        
//debug trace setting from gpio
int trace=0;

// spo256al2 data
#include "allophones.c"
#include "allophoneDefs.h"

//CTS 
char Sentance[1024]={' ',0};
uint16_t SentPos=0;
#include "CTS256_AL2.c"



//sound settings must be pins on the same slice
#define soundIO1 19
#define soundIO2 20
#define PWMrate 90
#define MAXALLOPHONE 64

uint PWMslice;
volatile static uint8_t SPO256DataReady=0;

//Beep
#include "midiNotes.h"
volatile static uint8_t BeepDataReady=0;

//CTS256
volatile static uint8_t CTSDataReady=0;

//SPOFREq
volatile static uint8_t SPO256FreqDataReady=0;

//Registers
volatile static uint8_t Registers[8];


//pico pio globals
PIO pio = pio0;  
uint sm_z80io; 


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

	//default speach frequency
	Registers[2]=90;


	sleep_ms(1000);  //wait for core 1 to finish setup

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

              if(addr==0) SPO256DataReady=1;
              if(addr==1) BeepDataReady=1;              
              if(addr==2) SPO256FreqDataReady=1;
              if(addr==3) CTSDataReady=1;              

//              printf("address %X SPO:%i Beep:%i Data:%i\n\r",addr,SPO256DataReady,BeepDataReady,data);
              if (trace  ){printf("io:%04X Rd reg[%02X]<-%02X \n\r",io,addr,data);}
	    }
	    		
//trace switch

/*            if (gpio_get(TRACEGPIO)==1){
              trace=1;
            }else{
              trace=0;
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


//############################################################################################################
//################################################# Sound ####################################################
//############################################################################################################

void PlayAllophone(int al){
    int b,s;
    uint8_t v;
    int pwmr=PWMrate;
    pwmr=MidiNoteWrap[Registers[2] & 0x7f]/4;
//    if(trace)printf("R:2 %i,PWMR:%i\n",Registers[2],pwmr);
    //reset pwm settings (play notes may change them)
    pwm_set_clkdiv(PWMslice,16);
    pwm_set_wrap (PWMslice, 256);

    if(al>MAXALLOPHONE) al=0;
    
    //get length of allophone sound bite
    s=allophonesizeCorrected[al];
    //and play
    for(b=0;b<s;b++){
        v=allophoneindex[al][b]; //get delta value
        sleep_us(pwmr);
        pwm_set_both_levels(PWMslice,v,v);
    }

}

void PlayAllophones(uint8_t *alist,int listlength){
    int a;
    for(a=0;a<listlength;a++){
       if(alist[a]==64){alist[a]=0;}
       PlayAllophone(alist[a]);
    }
}

void SetPWM(void){
    gpio_init(soundIO1);
    gpio_set_dir(soundIO1,GPIO_OUT);
    gpio_set_function(soundIO1, GPIO_FUNC_PWM);

    gpio_init(soundIO2);
    gpio_set_dir(soundIO2,GPIO_OUT);
    gpio_set_function(soundIO2, GPIO_FUNC_PWM);

    PWMslice=pwm_gpio_to_slice_num (soundIO1);
    pwm_set_clkdiv(PWMslice,16);
    pwm_set_both_levels(PWMslice,0x80,0x80);

    pwm_set_output_polarity(PWMslice,true,false);

    pwm_set_wrap (PWMslice, 256);
    pwm_set_enabled(PWMslice,true);

}

void Beep(uint8_t note){
    int w;
    //set frequency
    pwm_set_clkdiv(PWMslice,256);
    if (note>0 && note<128){
        //get divisor from Midi note table.
        w=MidiNoteWrap[note];
        pwm_set_both_levels(PWMslice,w>>1,w>>1);
        //set frequency from midi note table.
        pwm_set_wrap(PWMslice,w);
    }else{
        pwm_set_both_levels(PWMslice,0x0,0x0);
    }
}



//############################################################################################################
//################################################# CTS ######################################################
//############################################################################################################

void AddToSentance(char c){
    if(c=='\n'){
      printf("\nSay '%s'\n",Sentance);
        uint16_t s=slen(Sentance);
        Sentance[s++]=' ';
        Sentance[s++]=' ';
        Sentance[s++]=' ';
        Sentance[s]=0;
        sayWBW(Sentance);
//        PrintOutput();
//        printf(" - after say , starting speak\n");
        SentPos=1;
        Sentance[0]=' ';
        Sentance[1]=0;
        PlayAllophones(AlloBuffer,slen(AlloBuffer));
        AlloBuffer[0]=0;
    }else{
        Sentance[SentPos]=c;
        SentPos++;
        Sentance[SentPos]=0;

    }

}



//############################################################################################################
//################################################# Main #####################################################
//############################################################################################################




int main(){
    stdio_init_all();
    printf("\r\nz80disk\n\r\n");

//over clock
    set_sys_clock_khz(250000, true);


//uart if needed

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

//Default LED pin setup
    gpio_init(LEDPIN);
    gpio_set_dir(LEDPIN, GPIO_OUT);
    flash_led(200);    
        
//banner
    sleep_ms(1000);

    printf( "\n\n\r     ________________________________");
    printf(   "\n\r    /                                |");
    printf(   "\n\r   /           RC2014                |");
    printf(   "\n\r  /     SP0256-AL2 & CTS256-AL2      |");
    printf(   "\n\r |  O                                |");
    printf(   "\n\r |          Derek Woodroffe          |");
    printf(   "\n\r |              2026                 |");
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
        gpio_pull_down(TRACEGPIO);


        printf("\n\r\n\r OK lets start the SPO stuff\n\r");
//launch core 1
        multicore_launch_core1(z80io_core_entry);
        
        if (gpio_get(TRACEGPIO)==1){
               trace=1;
           }else{
               trace=0;
        }
	  
	if (trace==0){ 
	   printf("\n\r Trace zero, so no more info shown, \n\r SSShhh!!, I'm Talking\n\r");   
        }else{
           printf("\n\r Trace SET,so info shown, \n\r I'm Talking\n\r");
        }
       

        flash_led(200);
        

        flash_led(60);
        flash_led(60);
        flash_led(60);

// init sound
    SetPWM();



//say RC2040
    uint8_t alist[] ={ 24,14,2,55,19,2,17,0,46,7,11,17,19,2,0,40,23,14,0,17,19,11,0};
    PlayAllophones(alist,sizeof(alist));

        
    while (1) {


        if(SPO256DataReady>0){
           if(trace)printf("play allophone %i\n\r",Registers[0]);
           PlayAllophone(Registers[0]);
           SPO256DataReady=0;
           Registers[0]=0;
        }

        if(BeepDataReady>0){
           if(trace)printf("play beep %i\n\r",Registers[1]);
           Beep(Registers[1]);
           BeepDataReady=0;
           Registers[1]=0;
        }

        if(SPO256FreqDataReady>0){
           if(trace)printf("Frequency for SPO %i\n\r",Registers[2]);
//           SPOFreq(Registers[2]); //note taken direct from register
           SPO256FreqDataReady=0;
        }
        
        if(CTSDataReady>0){
           if(trace)printf("play CTS %i\n\r",Registers[3]);
           AddToSentance(Registers[3]);
           CTSDataReady=0;
           Registers[3]=0;
        }
        
//        sleep_ms(5);
        tight_loop_contents();
  }



}

