# aye
ZXAY metadata editor

# Usage
```
aye [args] [infile]

args = 
-q  Quiet
-o  Output
-a  New author
-m  New misc
-j  File to join
-s  Song to edit
-t  New song title (requires -s)
-d  Debug

infile = Input ZXAYEMUL file
```

If no options are specified the information and and song list from the file will be dumped to output.

If -o is specified the input file will be parsed and written to the output file.  It will be functionally but not necessarily binary identical.  Any duplicate data blocks will get squashed.  The output file will be overwritten without warning if it exists.

If -a, -m and/or -t are specified the relevant information fields will be changed to those provided on the command line.  Without an output file this does nothing.

-j Specifies another file to join to the input file.  Note: Due to how the AY format works, there is a limit on how big the file can get.  It is better to have larger tracks at the end of the file as AY uses relative offsets to the start of the data blocks, and these can overflow.  Aye does not currently check offsets are within bounds so there is a possibility of tracks not playing as expected due to the AY file pointing to the wrong data.

-d dumps the data for debugging purposes:
```
0 = none (default)
1 = basic dump of read data 
2 = basic dump of read and saved data 
3 = full dump of read data, basic dump of save data 
4 = full dump of read data, full dump of save data
```
