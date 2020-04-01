# Resumo :

A APS1 - Musical consiste em utilizar os seguintes periéricoas combinados para tocar uma música monofônica:

Link para o vídeo de demonstração: https://youtu.be/6VxK5Qw3l84

Periféricos uC:

    - Power Managment Controller (PMC)
    - Serial Protocol Interface (SPI)
 
APIs :

    - Driver SSD1306
 
Módulos : 
    
    - SAM-E70-XPLD
    - OLED 1 XPLAINED PRO
    - Buzzer


## Ligação

![](final.jpeg)

## Funcionamento

Com o Oled conectado a placa na saída EXT1 (como mostrado na figura acima) e o buzzer ligado ao pino PD30 e ao GND, embarque o esse projeto e verifique seu funcionamento:
- O Botão 1 serve para selecionar a música a ser tocada. No visor LCD é possível ver o nome da musica e o LED acende de acordo com o numero da música (Musica 1 -> LED 1, Musica 2 -> LED 2, Musica 3 -> LED 3);
- O Botão 2 inicia a musica selecionada e o LED 1 pisca de acordo com a música.
- Ao pressionar a o Botão 2 novamente, a musica é pausada e ao pressinonar novamnte, a musica volta a tocar.

## Limitações

O porjeto possui algumas limitações:
- Só é possivel tocar outra música após a primeira terminar
- O LCD não comporta muitas letras, por isso alguns títulos são cortados. 


