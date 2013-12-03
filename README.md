SECompressor
============

The squash program can tarball or compress files or directories into a single file. LZW compression is the only supported compression algorithm, and yields about a 50% size reduction on most natural language texts.

Usage
-----

**To compress:**
```
./squash [-a ALGORITHM] [-o OUTPUT_FILE] INPUT_FILE [INPUT_FILE [...]]
```
Options (can be in any order):
* `-a ALGORITHM` allows the compression algorithm to be specified. The only supported options are `lzw` and `none`.
* `-o OUTPUT_FILE` allows the user to specify a custom output file location. The default is `compressed.cmp`
* One or more input files may be specified. All of the input files will be pulled into the archive.

**To expand (decompress):**
```
./squash -e INPUT_FILE
```
Options: (can be in any order):
* `-e` specifies that the archive be expanded, not compressed.
* One input file may be specified, and it must be a valid archive file. The contents of the file will be extracted into the current working directory.