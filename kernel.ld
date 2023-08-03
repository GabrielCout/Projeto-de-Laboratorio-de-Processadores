SECTIONS {                  /* arquivo kernel.ld para placa Evaluator7T */
   /*
    * Vetor de reset
    */
   . = 0;
   .reset : { *(.reset) }

   /*
    * Segmentos text e data
    */
   . = 0x8000; 
  .text : { *(.text) }
  .data : { *(.data) }

  /*
   * Segmento bss
   */
  inicio_bss = .;
  .bss : { *(.bss) }
  . = ALIGN(4); 
  fim_bss = .;

  /*
   * Reserva espaÃ§o para a pilha
   */
  inicio_heap = .;
  . = . + 4096;
  . = ALIGN(8);
  inicio_stack = .;
}
