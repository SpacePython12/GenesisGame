    .section .data
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