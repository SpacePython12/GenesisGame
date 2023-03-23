# GenesisGame
This is a mess right now, as I'm trying to understand how I want the architecture to work.

## Contributing
If you're contributing assets, there's a couple things you need to know:
* The directory for assets is `/main/data`. There will be subdirectories for different types of assets.
* For artists:
    * The Genesis/Megadrive has color limitations you need to keep in mind. There are 4 palettes of 16 colors each. There are 512 possible colors that the Genesis can possibly display, but without tricks it can only display 61 of those colors. There is also no support for partial transparency, so stick with dithering or ask a developer about what to do. The first color in 3 of the 4 palettes is completely transparent. Each sprite can use one palette at a time, while each 8x8 level tile can use one palette. You can put art in the data directory. The art will be compressed by a developer.
* For musicians:
    * The Genesis/Megadrive has two sound chips: the YM2612 FM synthesizer, and the SN76489 PSG. The YM2612 has 6 FM channels, one of which can be swapped out for a sample channel. The PSG has 3 square wave channels and a noise channel. One of the PSG's channels will be dedicated to SFX. Try not to have more than 7 instruments playing at once in your composition. If you know how to use a tracker like Furnace, you can export a VGM. If not, a MIDI or sheet music is fine as well, one of the developers can transcribe your music.
* For developers:
    * I'll make sure to document most of my code (eventually) to show what it does.