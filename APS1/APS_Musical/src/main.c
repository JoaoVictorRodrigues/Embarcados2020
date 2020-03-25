/**
 * Material :
 *  - Kit: ATMEL SAME70-XPLD - ARM CORTEX M7
 */

/************************************************************************/
/* includes                                                             */
/************************************************************************/

#include "asf.h"
#include "musicas.h"

/************************************************************************/
/* defines                                                              */
/************************************************************************/

//LED 1
#define LED1_PIO			PIOC
#define LED1_PIO_ID		    ID_PIOC
#define LED1_PIO_IDX        8
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

//BOTÃO MUSICA 1
#define BUT1_PIO		   PIOD
#define BUT1_PIO_ID		   ID_PIOD
#define BUT1_PIO_IDX       28
#define BUT1_PIO_IDX_MASK  (1u << BUT1_PIO_IDX)

//BOTÃO PLAY/PAUSE
#define BUT2_PIO		   PIOC
#define BUT2_PIO_ID		   ID_PIOC
#define BUT2_PIO_IDX       31
#define BUT2_PIO_IDX_MASK  (1u << BUT2_PIO_IDX)

//BOTÃO MUSICA 2
#define BUT3_PIO		   PIOA
#define BUT3_PIO_ID		   ID_PIOA
#define BUT3_PIO_IDX       19
#define BUT3_PIO_IDX_MASK  (1u << BUT3_PIO_IDX)

//BUZZER
#define BUZZER_PIO           PIOD
#define BUZZER_PIO_ID        ID_PIOD
#define BUZZER_PIO_IDX       30
#define BUZZER_PIO_IDX_MASK  (1u << BUZZER_PIO_IDX)




/************************************************************************/
/* constants                                                            */
/************************************************************************/

int notes = sizeof(canon_in_D) / sizeof(canon_in_D[0]);

void buzz(long frequency, long length) {
	frequency = frequency*2;
	pio_set(LED1_PIO, LED1_PIO_IDX_MASK);
	pio_set(BUZZER_PIO, BUZZER_PIO_IDX_MASK);
	long delayValue = 1000000 / ((frequency) *2); // calculate the delay value between transitions
	long numCycles = frequency * length / 600;
	if(frequency == 0){
		pio_clear(BUZZER_PIO, BUZZER_PIO_IDX_MASK);
		delay_us(length);
	}else{
		for (long i = 0; i < numCycles; i++) { // for the calculated length of time...
			pio_clear(LED1_PIO, LED1_PIO_IDX_MASK);
			pio_set(BUZZER_PIO, BUZZER_PIO_IDX_MASK);
			delay_us(delayValue); // wait for the calculated delay value
			pio_set(LED1_PIO, LED1_PIO_IDX_MASK);
			pio_clear(BUZZER_PIO, BUZZER_PIO_IDX_MASK);
			delay_us(delayValue);
		}
	}
	pio_set(LED1_PIO, LED1_PIO_IDX_MASK);
	pio_clear(BUZZER_PIO, BUZZER_PIO_IDX_MASK);
}

void init(void);

void init(void)
{
  sysclk_init();

  WDT->WDT_MR = WDT_MR_WDDIS;
  
  //INICIALIZA O BUZZER
  pmc_enable_periph_clk(BUZZER_PIO_ID);
  
  //INICIALIZA SAIDA BUZZER
  pio_set_output(BUZZER_PIO, BUZZER_PIO_IDX_MASK, 0, 0, 0);
  
  // Ativa o PIO na qual o LED foi conectado
  // para que possamos controlar o LED.
  pmc_enable_periph_clk(LED1_PIO_ID);
  pmc_enable_periph_clk(LED2_PIO_ID);
  pmc_enable_periph_clk(LED3_PIO_ID);
  
  //Inicializa saída
  pio_set_output(LED1_PIO, LED1_PIO_IDX_MASK, 0, 0, 0);
  pio_set_output(LED2_PIO, LED2_PIO_IDX_MASK, 0, 0, 0);
  pio_set_output(LED3_PIO, LED3_PIO_IDX_MASK, 0, 0, 0);
   
  // Inicializa PIO do botao
  pmc_enable_periph_clk(BUT1_PIO_ID);
  pmc_enable_periph_clk(BUT2_PIO_ID);
  pmc_enable_periph_clk(BUT3_PIO_ID);
  
  // configura pino ligado ao botão como entrada com um pull-up.
  pio_set_input(BUT1_PIO, BUT1_PIO_IDX_MASK, PIO_DEFAULT);
  pio_set_input(BUT2_PIO, BUT2_PIO_IDX_MASK, PIO_DEFAULT);
  pio_set_input(BUT3_PIO, BUT3_PIO_IDX_MASK, PIO_DEFAULT);
  
  //CONFIGURA BOTÕES COMO 1
  pio_pull_up(BUT1_PIO, BUT1_PIO_IDX_MASK,1);
  pio_pull_up(BUT2_PIO, BUT2_PIO_IDX_MASK,1);
  pio_pull_up(BUT3_PIO, BUT3_PIO_IDX_MASK,1);
  
}

/************************************************************************/
/* Main                                                                 */
/************************************************************************/

int main(void)
{
  init();

  while (1)
  {
	if(!pio_get(BUT1_PIO, PIO_INPUT, BUT1_PIO_IDX_MASK)){
		while(!pio_get(BUT1_PIO, PIO_INPUT, BUT1_PIO_IDX_MASK)){
			pio_clear(LED1_PIO, LED1_PIO_IDX_MASK);      // Coloca 1 no pino LED
			delay_ms(100);
		}
		for (int i = 0; i < notes ; i = i + 2) {
			if (!pio_get(BUT3_PIO, PIO_INPUT, BUT3_PIO_IDX_MASK)){
				while(!pio_get(BUT3_PIO, PIO_INPUT, BUT3_PIO_IDX_MASK)){
					delay_ms(100);
				}
				break;
			}
			else{
				int noteDuration = tempo2[i] * 1.2;
				int pauseBetweenNotes = noteDuration;
				buzz(canon_in_D[i], noteDuration);
				delay_us(noteDuration);
				buzz(canon_in_D[i], noteDuration);
				delay_us(pauseBetweenNotes);
				
			}
			
		}
		pio_clear(BUZZER_PIO, BUZZER_PIO_IDX_MASK);
		pio_set(LED1_PIO, LED1_PIO_IDX_MASK);
	}
	if (!pio_get(BUT2_PIO, PIO_INPUT, BUT2_PIO_IDX_MASK)){
		while(!pio_get(BUT2_PIO, PIO_INPUT, BUT2_PIO_IDX_MASK)){
			delay_ms(100);
		}
		for (int thisNote = 0; thisNote < 208; thisNote++) {
			if (!pio_get(BUT3_PIO, PIO_INPUT, BUT3_PIO_IDX_MASK)){
				while(!pio_get(BUT3_PIO, PIO_INPUT, BUT3_PIO_IDX_MASK)){
					delay_ms(100);
				}
				break;
			}
			else{
				int noteDuration = tempos1[thisNote] * 0.5;
				int pauseBetweenNotes = noteDuration;
				buzz(musica1[thisNote], noteDuration);
				delay_us(noteDuration);
				buzz(musica1[thisNote], noteDuration);
				delay_us(pauseBetweenNotes);
			}
		}
		pio_clear(BUZZER_PIO, BUZZER_PIO_IDX_MASK);
		pio_clear(LED1_PIO, LED1_PIO_IDX_MASK);
	}
	
  }
  return 0;
}
