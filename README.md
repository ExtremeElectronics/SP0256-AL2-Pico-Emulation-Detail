### Emulating SPO256 al2 on RP2040 (Pico)
From wav samples from the origional chip available at https://www.cpcwiki.eu/index.php/SP0256_Allophones
Origional Chip data sheet in Additional folder

## GPIO
You need two pins on the same slice (currently 14/15) I invert one so you can put a small speaker directly across them.
Much better quality can be gained by using an amplifier and a simple filter.

## Sound Files
All sound files are embedded in allophones.c as byte wide bitstreams. This make uploading them easy, plus as they are code, so they don't use any ram

Adjusted lengths have been calculated as the origionals are (mostly) longer then they aught to be. 
Actual lengths of the allophones are in allophonesize[]
Adjusted play lengths are in allophonesizeCorrected[]

## Enumeration
allophoneDefs.h contains the allophone names from the data sheet as an innumeration to make making lists of allophones easier. 

## Making speach
Look through the origional data sheet. String together allophones. send them (with a length) to  PlayAllophones Simple.... ish :) 
