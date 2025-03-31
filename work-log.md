Auteur : Lucas Sube

# Cours 1

Qemu fourni un gdb-server sur lequel on peut se connecter (target remote localhost:xxxx)

lancer gdb en multi-arch (car sous Ubuntu) : ``gdb-multiarch kernel.elf``

Mémoire flash = persistante

Trouver doc pour connaitre les zones mémoire à l'intitialisation

Qemu met déjà le programme à executer en mémoire(en 0x0), pas besoin de gérer le stage 1 du bootloader

Linker Script : met les blocs du code (code, data, BSS, stack) dans la mémoire
    -> todo : comprendre la syntaxe


MMIO Register : zone mémoire pour la communication avec les périphériques
    -> faire des fonctions pour lire écrire sur 8,12,32 dessus




UART_SR : donne le status du périphérique (pour check si vivant)
UART_FIFO : transmet + reçoit avec le périphérique


# Cours 2
kernel.bin : équivalent de ce qui est chargé en mémoire en lisant les sections du .elf

Interruption pour éviter le polling.

UART à un petit buffer -> faire attention a ne pas le laisser se remplir

halt : stop le processeur jusqu'a qu'il soit notifié

Programmable/Vectored Interrupt Controller (PIC / VIC) : intermédiaire entre les périphériques et le CPU pour les interruptions. A une mémoire.


Interrupt Service Routine (ISR) : sauvegarde contexte, registre 0 à 12,lr puis remettre le contenu du lr dans pc
    - Retire 4 à lr pour "annuler" le fait que le processeur se préparait à l'instruction suivante. 
    - Dépiler le mode actuel : " ^ " à la fin du load
    - Voir slides pour l'implem
    - aknowlege le vic ?



Interruption set PC à 0x18 -> faire branchement vers handler ici

Si crash et que mémoire bloqué entre 0x04 et 0x10 -> accès mémoire illegal

Créer une autre stack dans le linker pour le mode interruption
Autoriser les UARTs d'autoriser les interruptions
    UARTIMSC 0x038 Mask set clear interruption 
        -> voir le bitfield dans la doc (page 64)
            bits 4-5
    UARTRIS 0x03C les interruption que le device voudrait lever
    
    UARTMIS 0x040 Mask autorisation + demandes 
    
    UARTICR 0x044 Clear l'interruption levé
    
    UARTIFLS 0x034 parametrage : interruption tout les X caractères

Activer le VIC
    Doc page 29, pas de FIQ que du IRQ
    VIRCIRQSTATUS 0x000 Mask autorisation + demandes 

Autoriser les interrution dans le CPU
    ->Code assembleur


-----

### lancer + debug :
Dans un terminal :
    make debug

Dans un autre
    ./debug.sh



## Step 1

Memory map register UART ( DDI0183G-UART_PL011_r1p5_TRM.pdf ) : page 49
    UARTDR : page 52, ne prend qu'un octet sur les bits de poid faible en payload
    
Memory map carte (DUI0225D_versatile_application_baseboard_arm926ej_s_ug-1.pdf) : page 74
    UART0 00x101F0000


uart_receive : 
    read de la zone mémoire + check flag "RXFE"
    polling tant que la file est vide



## Step 2


todo général :
    interruption dans le receive
    vérifier l'encodage reçu du clavier (en particulier é et les flèches)

-----

Listes des étapes faites(dans l'ordre de réalisation)

- Ajout d'une stack "irq_stack_top" pour le mode interruption dans kernel.ld

- Definition des macros relatives aux interruptions UART dans uart.h

- Implémentation de uart_enable / uart_disable en activant le bit de de reception d'interruption

- Activation des interruption CPU dans main.c:_start

- definition des macros isr.h (timer: doc page 201 *Versatile Application Baseboard*)


// Toujours pas de test jusque là, je ne vois pas comment faire a part tout implémenté puis debugger

- Squelette handler receive main.c

- todo : lire en détail le irq.s
    Rien compris au _wfi : c'est quoi une barriere mémoire ? Pourquoi on fait ça ?

- todo : Initialiser le vic à 0xFFFFF000 (4ko) afin d'optimiser la latence
    + TODO : call ça dans le kernel.ld ? ou irq.s ?
    + check VICIRQSTATUS (offset 0x000) si les interruptions sont bien activées
        * Peut-être faire ça comme check_stack dans main.c ?
    

