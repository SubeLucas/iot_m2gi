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



-----

### lancer + debug :
Dans un terminal :
    make debug

Dans un autre
    gdb build/kernel.elf



### docs

Memory map register UART ( DDI0183G-UART_PL011_r1p5_TRM.pdf ) : page 49
    UARTDR : page 52, ne prend qu'un octet sur les bits de poid faible en payload
    
Memory map carte (DUI0225D_versatile_application_baseboard_arm926ej_s_ug-1.pdf) : page 74
    UART0 00x101F0000







