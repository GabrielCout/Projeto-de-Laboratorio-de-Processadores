SECTIONS {                  /* arquivo kernel.ld para placa Evaluator7T */
   /*
    * Vetor de reset
    */
   . = 0;
   .reset : { *(.reset) }

   /*
    * Segmentos text e data
    */
   . = 0x40000000; 
  .text : { *(.text) }
  .data : { *(.data) }

  /*
   * Segmento bss
   */
  start_bss = .;
  .bss : { *(.bss) }
  . = . + 0x20000;
  . = ALIGN(4); 
  end_bss = .;

  /*
   * Reserva espaÃ§o para a pilha
   */
  start_heap = .;
  . = . + 0x4000000;
  . = ALIGN(8);
  end_heap = .;
  . = . + 0x200000; 
  stack_svr = .;
}
