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
-s  Song to edit
-t  New song title (requires -s)
-d  Debug

infile = Input ZXAYEMUL file
```

If no options are specified the information and and song list from the file will be dumped to output.

If -o is specified the input file will be parsed and written to the output file.  It will be functionally but not necessarily binary identical.  Any duplicate data blocks will get squashed.  The output file will be overwritten without warning if it exists.

If -a, -m and/or -t are specified the relevant information fields will be changed to those provided on the command line.  Without an output file this does nothing.

-d dumps the data for debugging purposes. 0 = none,
1 = basic dump of read data, 
2 = basic dump of read and saved data, 
3 = full dump of read data, basic dump of save data
