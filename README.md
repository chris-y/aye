# aye
ZXAY metadata editor

# Usage
aye [args] [infile]

args = 
-q  Quiet
-o  Output
-a  New author
-m  New misc

infile = Input ZXAYEMUL file

If no options are specified the information and and song list from the file will be dumped to output.

If -o is specified the input file will be parsed and written to the output file.  It will be functionally but not necessarily binary identical.  The output file will be overwritten without warning if it exists.
NB: At the moment ZXAYEMUL files containing multiple songs do not parse correctly and haven't been tested properly.

If -a and/or -m are specified the relevant information fields will be changed to those provided on the command line.  Without an output file this does nothing.
