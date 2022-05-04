### Emulating SPO256 al2 on RP2040 (Pico)
From wav samples from the origional chip available at https://www.cpcwiki.eu/index.php/SP0256_Allophones
Origional Chip data sheet in Additional folder

##Sound Files
All sound files are embedded in allophones.c as byte wide bitstreams. This make uploading them easy, plus as they are code, so they don't use any ram

Adjusted lengths have been calculated as the origionals are (mostly) longer then they aught to be. 
Actual lengths of the allophones are in allophonesize[]
Adjusted play lengths are in allophonesizeCorrected[]

## Enumeration
allophoneDefs.h contains the allophone names from the data sheet as an innumeration to make making lists of allophones easier. 

