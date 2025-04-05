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
#include "isr.h"
#include "isr-mmio.h"

/*
 * Assembly functions:
 */
extern void _irqs_setup(void);
extern void _irqs_enable(void);
extern void _irqs_disable(void);
extern void _wfi(void);

/*
 * Data structure for handlers and cookies
 */
struct handler {
  void (*callback)(uint32_t, void*);
  void *cookie;
};

struct handler handlers[NIRQS];

/*
 * Interrupt Service Routine, up-called from assembly.
 * Needs to interogate the VIC about the masked interrupt
 * status and call the corresponding handlers.
 */
void isr() {
  core_disable_irqs();
  uint32_t irqs = mmio_read32((void *)VIC_BASE_ADDR, VICIRQSTATUS);
  for (uint32_t i = 0; i < NIRQS; i++) {

    // Vérifie si un flag d'interruption est levé
    if (irqs & (1 << i)){
      handlers[i].callback(i, handlers[i].cookie);
    }
  }
  core_enable_irqs();
}

void core_enable_irqs() {
  _irqs_enable();
}

void core_disable_irqs() {
  _irqs_disable();
}

void core_halt() {
  _wfi();
}

/*
 * Initial setup our interrupt support,
 * need to initialize both the hardware and software
 * sides.
 */
void vic_setup_irqs() {
  //_irqs_setup();
  // setup les handlers ici ?
}

/*
 * Enables the given interrupt at the VIC level.
 */
void vic_enable_irq(uint32_t irq, void (*callback)(uint32_t, void*), void *cookie) {
  //mmio_write32((void*)VIC_BASE_ADDR,VICINTENABLE,(VIC_BASE_ADDR+VICINTENABLE | (1<<(11+irq))));
  mmio_set((void*)VIC_BASE_ADDR,VICINTENABLE,irq);
  handlers[irq].callback = callback;
  handlers[irq].cookie = cookie;
  
}

/*
 * Disables the given interrupt at the VIC level.
 */
void vic_disable_irq(uint32_t irq) {
  mmio_clear((void*)VIC_BASE_ADDR,VICINTENABLE,irq);

  handlers[irq].callback = NULL;
  handlers[irq].cookie = NULL;
}
