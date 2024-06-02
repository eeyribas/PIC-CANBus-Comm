#include <18F458.h>
#include <can-18xxx8.c>

#use delay(clock=20000000)
#FUSES NOWDT                    // No Watch Dog Timer
#FUSES HS                       // High speed Osc (> 4mhz)
#FUSES NOPROTECT                // Code not protected from reading
#FUSES NOBROWNOUT               // No brownout reset
#FUSES NOPUT                    // No Power Up Timer 
#FUSES NOLVP                    // Low Voltage Programming on B3(PIC16) or B5(PIC18)
#FUSES NOWRT

#define CAN_USE_EXTENDED_ID FALSE

char dat[] = {0x45,0x41,0x42};
struct rx_stat t_rx_stat;
int32 rx_id;
int in_data[8];
int rx_len;
int1 k = 0;

#int_timer1
void isr()
{
   set_timer1(65500);
   output_toggle(pin_d0);
}

#int_canrx0
void canrx0_int() 
{
   if (can_kbhit()) {
      can_getd(rx_id, in_data, rx_len, t_rx_stat);
   }
   
   switch(in_data[0]) {
      case 0xbb: { 
         output_high(pin_a2);
      }
      break;
      case 0xb0: {
         output_high(pin_a1);
      }
      break;
      default: {}
      break;
   }
}

void main()
{
   can_init();
   can_set_mode(CAN_OP_CONFIG);
   
   BRGCON1.brp=4; 
   BRGCON1.sjw=0; 
   BRGCON2.prseg=2; 
   BRGCON2.seg1ph=5; 
   BRGCON2.sam=FALSE; 
   BRGCON2.seg2phts=FALSE;  
   BRGCON3.seg2ph=5; 
   BRGCON3.wakfil=TRUE; 

   can_set_mode(CAN_OP_NORMAL);
   
   setup_timer_3(T3_DISABLED | T3_DIV_BY_1);
   setup_low_volt_detect(FALSE);
   setup_adc_ports(NO_ANALOGS);
   setup_adc(ADC_OFF);
   setup_psp(PSP_DISABLED);
   setup_spi(SPI_SS_DISABLED);
   setup_wdt(WDT_OFF);
   setup_timer_0(RTCC_INTERNAL);
   setup_timer_1(T1_INTERNAL|T1_DIV_BY_8);
   setup_timer_2(T2_DISABLED,0,1);
   setup_comparator(NC_NC_NC_NC);
   setup_vref(FALSE);
   
   enable_interrupts(int_timer1);  
   enable_interrupts(global);
   
   while (true) {
      if (can_tbe()) {
         can_putd(6, dat, 3, 0, 0, 0);
      }
      
      output_low(pin_a2);output_low(pin_a1);
      output_toggle(pin_a0); output_toggle(pin_d1);
      delay_ms(1000);
   }
}
