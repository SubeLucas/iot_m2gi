/*
 * Copyright: Olivier Gruber (olivier dot gruber at acm dot org)
 *
 * This program is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <https://www.gnu.org/licenses/>.
 */
#include "main.h"
#include "uart.h"

extern uint32_t irq_stack_top;
extern uint32_t stack_top;

void check_stacks() {
  void *memsize = (void*)MEMORY;
  void *addr;
  addr = &stack_top;
  if (addr >= memsize)
    panic();
/*
  addr = &irq_stack_top;
  if (addr >= memsize)
    panic();
*/
}

void uart_receive_handler() {
  uint8_t status = VICIRQSTATUS
  if (status & 0x01)
  uart_receive(UART0, &c);
  else
  //led_off(LED_MMIO_BAR);
}

/**
 * This is the C entry point,
 * upcalled once the hardware has been setup properly
 * in assembly language, see the startup.s file.
 */
void _start(void) {
  char c;
  check_stacks();
  uarts_init();
  core_enable_irqs();   // Interruption cpu
  
  //vic_setup_irqs();
  //void vic_enable_irq(uint32_t irq,void(*callback)(uint32_t,void*),void*cookie);
  core_enable_irqs()
  uart_enable(UART0);

  uart_send(UART0, 48);
  for (;;) {
    halt();
  }
}



void panic() {
  for(;;)
    ;
}
