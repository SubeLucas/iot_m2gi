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



Interruption set PC à 0x18 -> faire branchement vers handler ici pour atteindre 0x20 (_reset) car eu le temps de fetch 2 instructions

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


# Cours 3

Ne jamais faire de truc long ou blocant, ni de mutex dans les handlers.
    -> Risque de perdre des données

Buffer circulaire(implémentation dépend du processeur) pour être thread-safe entre le handler et le code
    -> plus besoin de désactiver les interruptions en zone critique


Cas interruption juste avant un halt -> solution flush / faire le test + halt atomiquement (voir slide 13)

Slide 13 : activer les interruption dans le for, c'est mieux sinon doit vider la liste sans interruption à chaque fois.

Si ring pleins -> pas être en bizantin, tout stoper (rediriger en panic)

Vider la fifo jusqu'a en dessous du seuil, sinon pas d'interruption

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
        Flèches -> 3 octets donc 3 interruptions

-----

Listes des étapes faites(dans l'ordre de réalisation)

- Ajout d'une stack "irq_stack_top" pour le mode interruption dans kernel.ld

- Definition des macros relatives aux interruptions UART dans uart.h

- Implémentation de uart_enable / uart_disable en activant le bit de de reception d'interruption

- Activation des interruption CPU dans main.c:_start

- definition des macros isr.h (timer: doc page 201 *Versatile Application Baseboard*)

- Implémentation de vic_enable_irq



// Toujours pas de test jusque là, je ne vois pas comment faire a part tout implémenté puis debugger
    -> Possible de test le UART avant l'implem du VIC

- Squelette handler receive main.c
    -> todo : rajouter (void* cookie) dans la signature
    -> pointeur de handler

- Initialiser le vic à 0xFFFFF000 (4ko) afin d'optimiser la latence -> mettre selon l'autre documentation de la board avec le 1040 car après dans la chaine de la documentation.
    + TODO : call ça dans le kernel.ld ? ou irq.s ?
    + check VICIRQSTATUS (offset 0x000) si les interruptions sont bien activées
        * Peut-être faire ça comme check_stack dans main.c ?
    + 
        
- branch link ``_isr_handler`` avec sauvegarde et restauration de contexte.
    + Juste les 12 premiers + LR pour revenir, registres 13,14 sont "banké"

- todo : une fois jump à _isr 
    + Savoir quelle identifier quelle interruption
    + Que faire avec le matériel qui a interrompu
    + Que faire au logiciel qui s'est interrompu

debug gdb : layout next




## Step 3

interruption tx quand il reste suffisament d'espace dans la file
    Remplir le ring au delà du seuil à chaque interruption


Mettre des listener sur la lecture / écriture



// brouillon //

uartinit(no,rl,wl,cookie){
    handler fifo rx appel rl
    handler fifo tx appel wl
}

uint8_t bit;
while (uart_read(no,&bit)){
    uart_write(no,&bit);
}

-> voir cour 3 slide 22
Read :
Pas de notification tant que return pas false, responsabilité de l'app

Write :
Pas défaut disponible, jusqu'a pleins.
Une fois pleins, notifie une fois re-disponible