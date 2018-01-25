# MuonRCNP

Time stamp sorting, event building, and analysis for MuonRCNP experiment Jan 2018

First a bit of terminology:  
a fragment is a single hit, one board, one channel, either wave form data, or pulse height data  
an event contains several fragments, within the event building window

## Setup:

lines starting with "$" refer to command line commands, starting with "[]" to commands in root

go to the directory
```
$cd ~/work/seamine2018_daq/kathrin
```
set the environment
```
$source setup
```
this set the PATH and LD_LIBRARY_PATH, the correct root version 6.12,  
and also my emacs environment, make your own setup file, if you don’t want this!

There are two main programs, 'EventBuild' and 'Histos', and two scripts

## EventBuild:
this program takes the raw data and produced a time ordered tree with built and analyzed events
```
$EventBuild
```
gives you some help on the options:  
"-r N" is the run number N, it will automatically read from the proper disk and write the output to ./root/runXXXX.root  
"-s setfile.dat" is to choose the settings file, which contains the settings for the analysis  
"-m M" is the memory depth, the number of fragments M stored in memory for timestamp sorting, if you get warnings that events have to be discarded, increase this number  
"-e E" is the event building window, fragments which are less than E ns away from the last fragment in the current event are added, otherwise a new event is created.  
"-w W" chooses if the waveform is written to the tree, 0 for never, 1 only if the threshold was crossed, 2 always, 3 if threshold not crossed do not write anything 

two other options are more for debugging:  
"-l F" is the last fragment to be read, only F fragments are process then  
"-v V" is the verbose level, the higher V the more stuff is dumped to the screen  

A typical command would be
```
$EventBuild -r 12 -m 20000 -s defaultset.dat
```
you can also use ctrl+c to quit savely, i.e. the fragments will be flushed, events closed, and the file written.

At the end the program gives you a bit of statistics:
```
Status of SortHits:
Event memory depth: 20000
Fragments processed:   2642280
Events built:   198099
writing tree to root file
------------------------------------
Total of 2642280 data fragments (3719 MB) read.
198099 entries written to tree (59 MB)
First time stamp: 19129144, last time stamp: 742731846712, data taking time: 5941.7 seconds.
Program Run time: 130.648 s.
Unpacked 20224.4 fragments/s.
```

## Histos:
The second program produces histograms from the tree
```
$Histos
```
gives you help  
"-i infile" defines the inputfile(s), it can also combine several files into one output  
"-o outfile" the outputfile  
again debugging options are  
"-l L" to read up to event L  
"-v V" verbose level

so far there are spectra for the germanium energies and the BaF pulse height defined.  
More spectra need to be defined.
