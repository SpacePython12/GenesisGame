    .balign 2
    .global Z80Driver
    .global Z80Driver_Size
Z80Driver: 
    .incbin "main/src/z80Driver.bin"
1:
    .balign 2
Z80Driver_Size: .long 1b-Z80Driver

    .global VDPRegValues
    .balign 2
VDPRegValues:
    .byte 0x14 ; // 00
    .byte 0x74 ; // 01
    .byte 0x30 ; // 02
    .byte 0x2C ; // 03
    .byte 0x07 ; // 04
    .byte 0x5C ; // 05
    .byte 0x00 ; // 06
    .byte 0x20 ; // 07
    .byte 0x00 ; // 08
    .byte 0x00 ; // 09
    .byte 0x00 ; // 0A
    .byte 0x03 ; // 0B
    .byte 0x81 ; // 0C
    .byte 0x2F ; // 0D
    .byte 0x00 ; // 0E
    .byte 0x02 ; // 0F
    .byte 0x01 ; // 10
    .byte 0x00 ; // 11
    .byte 0x00 ; // 12

    .global VDPAddrDests
    .balign 2
VDPAddrDests:
    .byte 0x00
    .byte 0x01
    .byte 0x08
    .byte 0x03
    .byte 0x04
    .byte 0x05
    .byte 0x06
    .byte 0x07

    .global TestTileData
    .global TestTileDataSize
    .balign 2
TestTileData:
    .incbin "main/data/tileset/player.bin"
TestTileDataEnd:
    .balign 2
TestTileDataSize: .long TestTileDataEnd - TestTileData

    .global TestColorData
    .balign 2

    .global TestColorData
    .global TestColorDataSize
    .balign 2
TestColorData:
    .incbin "main/data/palette/player.bin"
TestColorDataEnd:
    .balign 2
TestColorDataSize: .long TestColorDataEnd - TestColorData