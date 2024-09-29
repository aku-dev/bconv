# bconv

Binary Image Converter

Usage: 
```
bconv.exe *.png [/b] [/d] [/i] [/p] [/v] [/h]

/b binary format.
/d decimal format.
/i no info format.
/p preview image mode.
/v vertical mode.
/h horisont mode.
```

Example output:
```
[ PNG INFORMATION ]
Width: 8
Height: 8
Filesize: 965 (0K)
Total pixels: 64
Data Size: 256

[ ARRAY ]

const uint8_t array_png[8] PROGMEM = {

0xFF, 0x81, 0xA5, 0x99, 0x99, 0xA5, 0x81, 0xFF

};
```
