    .section .text.keepboot
    .global _start
    .global _int
    .global _irq
    .global _vblank
    .global _hblank
    .org 0x00000000
Start_Of_Rom:
Vectors_68k:
    .long 0xFFFFFE  // SP after reset
    .long _start    // PC after reset
    .long _buserr   // Bus Error
    .long _addrerr  // Address Error
    .long _int      // Illegal Instruction
    .long _int      // Division by zero
    .long _int      // CHK exception
    .long _int      // TRAPV exception
    .long _int      // Privilege violation
    .long _int      // TRACE exception
    .long _int      // Line-A emulator
    .long _int      // Line-F emulator

    // Unused
    .long _int, _int, _int, _int
    .long _int, _int, _int, _int
    .long _int, _int, _int, _int
    .long _int      // Spurious exception

    .long _irq      // IRQ lv. 1
    .long _irq      // IRQ lv. 2
    .long _irq      // IRQ lv. 3
    .long _hblank   // IRQ lv. 4 (HBlank)
    .long _irq      // IRQ lv. 5
    .long _vblank   // IRQ lv. 6 (VBlank)
    .long _irq      // IRQ lv. 7

    // TRAP 00-15
    .long _int, _int, _int, _int
    .long _int, _int, _int, _int
    .long _int, _int, _int, _int
    .long _int, _int, _int, _int
    
    // Unused
    .long _int, _int, _int, _int
    .long _int, _int, _int, _int
    .long _int, _int, _int, _int
    .long _int, _int, _int, _int

    .org 0x00000100
ROM_Header: //            . .               .       .       .
    .ascii "SEGA MEGA DRIVE (C)SPACPYTH 2023"
    .ascii "MEGALINK OS                                     "
    .ascii "MEGALINK OS                                     "
    .ascii "OS 00000000-00"
    .word  0x0000
    .ascii "J6MKPCFD        "
    .long  0x000000
    .long  0x3FFFFF
    .ascii "RA"
    .word  0xA020
    .long  0xA13000
    .long  0xA130FF
    .ascii "            "
    .ascii "http://github.com/SpacePython12/MegaLink        "
    .ascii "JUE             " 

    .org 0x00000200

_buserr:
    move.l 0x80000000,(0xFF0004)
    jsr _int

_addrerr:
    move.l 0x40000000,(0xFF0004)
    jsr _int