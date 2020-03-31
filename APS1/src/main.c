 /* Material :
 *  - Kit: ATMEL SAME70-XPLD - ARM CORTEX M7
 */

 /************************************************************************/
 /* includes                                                             */
 /************************************************************************/

 #include "asf.h"
 #include "musicas.h"
 #include "gfx_mono_ug_2832hsweg04.h"
 #include "gfx_mono_text.h"
 #include "sysfont.h"


 /************************************************************************/
 /* defines                                                              */
 /************************************************************************/

 //LED 1
#define LED1_PIO			PIOA
#define LED1_PIO_ID		    ID_PIOA
#define LED1_PIO_IDX        0
#define LED1_PIO_IDX_MASK   (1<< LED1_PIO_IDX)

 //LED 2
 #define LED2_PIO			PIOC
 #define LED2_PIO_ID		    ID_PIOC
 #define LED2_PIO_IDX        30
 #define LED2_PIO_IDX_MASK   (1<< LED2_PIO_IDX)

 //LED 3
 #define LED3_PIO		    PIOB
 #define LED3_PIO_ID		    ID_PIOB
 #define LED3_PIO_IDX        2
 #define LED3_PIO_IDX_MASK   (1<< LED3_PIO_IDX)

 //SELEÇÃO DE MUSICA
 #define BUT1_PIO		   PIOD
 #define BUT1_PIO_ID		   ID_PIOD
 #define BUT1_PIO_IDX       28
 #define BUT1_PIO_IDX_MASK  (1u << BUT1_PIO_IDX)

 //BOTÃO PLAY/PAUSE
 #define BUT2_PIO		   PIOC
 #define BUT2_PIO_ID		   ID_PIOC
 #define BUT2_PIO_IDX       31
 #define BUT2_PIO_IDX_MASK  (1u << BUT2_PIO_IDX)

 //BUZZER
 #define BUZZER_PIO           PIOD
 #define BUZZER_PIO_ID        ID_PIOD
 #define BUZZER_PIO_IDX       30
 #define BUZZER_PIO_IDX_MASK  (1u << BUZZER_PIO_IDX)


 volatile int tocando;
 volatile Bool musica;
 volatile int active;
 volatile int nota_atual;

 /************************************************************************/
 /* constants                                                            */
 /************************************************************************/
 void init(void);


 void callback_BUT1(void){
	musica = true;
 }

 void callback_BUT2(void) {
	delay_ms(200);
	tocando = !tocando;
 }

 void selecao(void){
	if(active == 0){
		pio_clear(LED1_PIO,LED1_PIO_IDX_MASK);
		pio_set(LED2_PIO,LED2_PIO_IDX_MASK);
		pio_set(LED3_PIO,LED3_PIO_IDX_MASK);
	 }
 
	 else if(active == 1){
		pio_clear(LED2_PIO,LED2_PIO_IDX_MASK);
		pio_set(LED1_PIO,LED1_PIO_IDX_MASK);
		pio_set(LED3_PIO,LED3_PIO_IDX_MASK);
	 }
 
	 else if(active == 2){
		 pio_clear(LED3_PIO,LED3_PIO_IDX_MASK);
		 pio_set(LED2_PIO,LED2_PIO_IDX_MASK);
		 pio_set(LED1_PIO,LED1_PIO_IDX_MASK);
	 }
 }

 int buzz(double freq, double tm,double velocidade,int notaAt) {
 if(tocando == 1){
	freq = 2*freq;
	pio_set(LED1_PIO,LED1_PIO_IDX_MASK);

	double val = 1000000 / ((freq) * 2);
 
 
		if(freq == 0){
			pio_clear(BUZZER_PIO,BUZZER_PIO_IDX_MASK);
			delay_us((tm*1000)/velocidade);
		}
		else{
		 pio_clear(LED1_PIO,LED1_PIO_IDX_MASK);
		 long numCycles = freq * tm/ 1000;
		 
		 for(int i = 0;i<(numCycles/velocidade);i++){
			 pio_set(BUZZER_PIO,BUZZER_PIO_IDX_MASK);
			 delay_us(val);
			 pio_clear(BUZZER_PIO,BUZZER_PIO_IDX_MASK);
			 delay_us(val);
			}
		}
	 pio_clear(BUZZER_PIO,BUZZER_PIO_IDX_MASK);
	 return notaAt+1;
	}
 return notaAt;
 }



 void init(void){
 
 tocando = 1;
 musica = false;
 active = 2;
 sysclk_init();

 WDT->WDT_MR = WDT_MR_WDDIS;
 
 // Ativa o PIO
 pmc_enable_periph_clk(LED1_PIO_ID);
 pmc_enable_periph_clk(LED2_PIO_ID);
 pmc_enable_periph_clk(LED3_PIO_ID);
 pmc_enable_periph_clk(BUZZER_PIO_ID);
 pmc_enable_periph_clk(BUT1_PIO_ID);
 pmc_enable_periph_clk(BUT2_PIO_ID);
 
 //Inicializa saída
 pio_set_output(LED1_PIO, LED1_PIO_IDX_MASK, 0, 0, 0);
 pio_set_output(LED2_PIO, LED2_PIO_IDX_MASK, 0, 0, 0);
 pio_set_output(LED3_PIO, LED3_PIO_IDX_MASK, 0, 0, 0);
 pio_set_output(BUZZER_PIO, BUZZER_PIO_IDX_MASK, 0, 0, 0);
 
 pio_configure(BUT1_PIO,PIO_INPUT,BUT1_PIO_IDX,PIO_PULLUP);
 pio_configure(BUT2_PIO,PIO_INPUT,BUT2_PIO_IDX,PIO_PULLUP | PIO_DEBOUNCE);
 
 //CONFIGURA BOTÕES COMO 1
 pio_handler_set(BUT1_PIO, BUT1_PIO_ID,BUT1_PIO_IDX_MASK,PIO_IT_FALL_EDGE,callback_BUT1);
 pio_handler_set(BUT2_PIO,BUT2_PIO_ID,BUT2_PIO_IDX_MASK,PIO_IT_FALL_EDGE,callback_BUT2);
  
 pio_enable_interrupt(BUT1_PIO,BUT1_PIO_IDX_MASK);
 pio_enable_interrupt(BUT2_PIO,BUT2_PIO_IDX_MASK);
 
 NVIC_EnableIRQ(BUT1_PIO_ID);
 NVIC_SetPriority(BUT1_PIO_ID,4);
 
 NVIC_EnableIRQ(BUT2_PIO_ID);
 NVIC_SetPriority(BUT2_PIO_ID,4);
 }

 /************************************************************************/
 /* Main                                                                 */
 /************************************************************************/

 int main(void){
 
 init();
 gfx_mono_ssd1306_init();
 
 
 WDT->WDT_MR = WDT_MR_WDDIS;
 nota_atual = 0;
 
 
 while (1)
 {
 
 pio_clear(BUZZER_PIO,BUZZER_PIO_IDX_MASK);
 selecao();
	 if(musica == true){
		 delay_ms(500);
		 if(active == 0){
			 active = 1;
			 nota_atual = 0;
			 gfx_mono_draw_string("Jigglypuff", 0,10, &sysfont);
		 }
		 else if(active == 1){
			 active = 2;
			 nota_atual = 0;
			 gfx_mono_draw_string("Canon in D", 0,10, &sysfont);
		 }
		 else{
			 active = 0;
			 nota_atual = 0;
			 gfx_mono_draw_string("Pirates   ", 0,10, &sysfont);
		 }
		 
		 musica = false;
		 }
 
 
 
	 if(active == 0 && tocando == 1){
		while(nota_atual<(sizeof(musica1)/sizeof(musica1[0]))){
			nota_atual = buzz(musica1[nota_atual],tempos1[nota_atual],0.7,nota_atual);
		}
	 nota_atual = 0;
	 tocando = 0;
	 pio_clear(BUZZER_PIO,BUZZER_PIO_IDX_MASK);
	 }
	 
	 else if (active == 1 && tocando == 1){
		 
		while(nota_atual<(sizeof(canon_in_D)/sizeof(canon_in_D[0]))){
		nota_atual = buzz(canon_in_D[nota_atual],tempo2[nota_atual]*2,1,nota_atual);
	 }
	 nota_atual = 0;
	 tocando = 0;
	 pio_clear(BUZZER_PIO,BUZZER_PIO_IDX_MASK);
	 }
	 
	 else if(active==2 && tocando==1){
		while(nota_atual<(sizeof(melody)/sizeof(melody[0]))){
		nota_atual = buzz(melody[nota_atual],temp[nota_atual],1.1,nota_atual);
	 }
	 nota_atual = 0;
	 tocando = 0;
	 pio_clear(BUZZER_PIO,BUZZER_PIO_IDX_MASK);
	 }
	}
 return 0;
 }


