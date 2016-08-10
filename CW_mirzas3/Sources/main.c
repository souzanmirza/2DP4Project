/*********************************************************************                  
/*                       McMaster University                         *
/*                      2DP4 Microcontrollers                        *
/*                          Lab Section 02                           *
;*                  Souzan Mirza mirzas3 1211547                     *
;*********************************************************************
;*********************************************************************
;*                       2DP4 Final Project                          *
;*                 This code implements a serial transmission of     *
the ADC Channel 10 to a PC.                                          *
Log:                                                                 *
March 31                                                             *
Clock is not set: needs to be 4MHz, I think it is running at 6.25MHz.* 
Once it is set need to set the SCI rate properly. At this time, this *
configuration can transmit at 10Hz signal through the conditioning   *
cct without aliasing.                                                *
April 3                                                              *
Clock is set, time delay replaced. Interrupt driven on fallen edge.  *
;*                                                                   *
;*                                                                   *
;*********************************************************************
;*********************************************************************
;*                            References                             *
;* 2DP4 Lab Code                                                     *
;*                                                                   *
;*                                                                   *
;*********************************************************************/

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"  /* derivative information */
#include "SCI.h"

char string[20];
unsigned short val;  
char on=0;               //transmission state variable
char counter=0;          //track number of falling edges
void delayby1ms(int);
void setClk(void);
void transmit (void);

//---------------------OutCRLF---------------------
// Output a CR,LF to SCI to move cursor to a new line
// Input: none
// Output: none

void OutCRLF(void){
  SCI_OutChar(CR);
  SCI_OutChar(LF);
  PTJ^=0x01;
}

void main(void) {
  
  setClk();

  TSCR1 = 0x90;    //Timer System Control Register 1
                    // TSCR1[7] = TEN:  Timer Enable (0-disable, 1-enable)
                    // TSCR1[6] = TSWAI:  Timer runs during WAI (0-enable, 1-disable)
                    // TSCR1[5] = TSFRZ:  Timer runs during WAI (0-enable, 1-disable)
                    // TSCR1[4] = TFFCA:  Timer Fast Flag Clear All (0-normal 1-read/write clears interrupt flags)
                    // TSCR1[3] = PRT:  Precision Timer (0-legacy, 1-precision)
                    // TSCR1[2:0] not used

  TSCR2 = 0x01;    //Timer System Control Register 2
                    // TSCR2[7] = TOI: Timer Overflow Interrupt Enable (0-inhibited, 1-hardware irq when TOF=1)
                    // TSCR2[6:3] not used
                    // TSCR2[2:0] = Timer Prescaler Select:
                    
  TIOS = 0xFE;     //Timer Input Capture or Output capture
                   //set TIC[0] and input (similar to DDR)
  PERT = 0x01;     //Enable Pull-Up resistor on TIC[0]

  TCTL3 = 0x00;    //TCTL3 & TCTL4 configure which edge(s) to capture
  TCTL4 = 0x02;    //Configured for capture on falling edges on TIC[0]      
   
  TIE = 0x01;      //Timer Interrupt Enable

	EnableInterrupts; //CodeWarrior's method of enabling interrupts


// Setup and enable ADC channel 10
// Refer to Chapter 14 in S12G Reference Manual for ADC subsystem details
		
	ATDCTL1 = 0x2F;		// set for 10-bit resolution
	ATDCTL3 = 0x88;		// right justified, one sample per sequence
	ATDCTL5 = 0x2A;		// continuous conversion on channel 10
	
	
// Setup LED and SCI
  DDRJ |= 0x01;    
  PTJ=0x01;         //turn on-board LED on
  SCI_Init(9600);   //set Baudrate 
  
  //infinte loop
  for(;;) {
  if (on==1) {
    transmit();
  }
  }
}

//interrupt on falling edge to toggle the state variable "on"
static interrupt  VectorNumber_Vtimch0 void ISR_Vtimch0(void) {
  unsigned int temp;      //for setting interrupt flag
  counter++;              //increment counter to keep track of falling edges
  if (counter%2==1) {     //if odd number of falling edges, toggle "on"
    on=1;
  } 
  temp = TC0;              //reset to be able to read more interrupts
}


//functions
void transmit (void) {
  while(counter%2==1) {
  val=ATDDR0;             //ADC value saved in register 0
  SCI_OutUDec(val);       //transmit as decimal out, this means transmitting 0-4 ascii character
  OutCRLF();              //send terminator
  delayby1ms(3);          //delay for Matlab ~300Hz sampling
  }
}

void setClk(void){          //sets bus-clk to 4Mz since bus=pll/2

  CPMUCLKS_PLLSEL = 1;
  CPMUOSC = 0x00;
  CPMUSYNR = 0x0F;         //fref=2*1*(15+1)=32
  CPMUFLG=0x00;            //pllclk=32/4=8MHz then busclk is 8/2=4MHz
}

void delayby1ms(int time)     //delay for 4MHz
{
	unsigned int j,k;
	for(j=0;j<time;j++)
		for(k=0;k<645;k++);
		  PTJ = PTJ;
      PTJ = PTJ;
      PTJ = PTJ;
      PTJ = PTJ;
      PTJ = PTJ;

}  