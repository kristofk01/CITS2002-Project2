# CITS2002-Project2

The goal of this project was to design and develop a command-line utility program, named duplicates, to locate and report duplicate files in, and below, a named directory.

More info can be found here: https://teaching.csse.uwa.edu.au/units/CITS2002/past-projects/p2021-2/summary.php

## Compilation

```bash
make clean && make
```

## Usage

```bash
./duplicates [options] directory1 [directory2]...
```

The available options are as follows:

```
-a          include directory entries whose names begin with a dot (.)
-A          report if the program attemps the advanced version
-f=FILENAME list all files whose SHA2 hash matches that of the indicated file
-h=HASH     list all files with the indicated SHA2 hash
-l          list all duplicate files
-q          quietly test if any duplicate files exist
```

## Examples

Traverse through `mydir`, and consider all files (including hidden ones):
```bash
$ ./duplicates -a mydir
4395
92568704
4323
92317429
```
Here, we found 4395 files with total size 92.57 MB, of which, 4323 files are unique with total size 92.32 MB.

Print the relative paths of all files that match the SHA2 hash of `myfile.txt` under the directory `Desktop`:
```bash
$ ./duplicates -f myfile.txt Desktop
Desktop/anotherfile.txt
Desktop/onemore.txt
```
Or similarly, using the `-h` flag:
```bash
$ ./duplicates -h c15019ab522a5b2afdca268fd6b60faf05c54d7c5d902bafc9b23875 Desktop
Desktop/anotherfile.txt
Desktop/onemore.txt
```
Where `myfile.txt` has a SHA2 hash of `c15019ab522a5b2afdca268fd6b60faf05c54d7c5d902bafc9b23875`.

## Caveats
Due to our choice of using a hashtable to keep track of all files (unique and duplicate), a core limitation in our design is that the number of files that can be checked is restricted. Currently, the accuracy of reporting degrades as the number of files reaches this limit (`2^28`, i.e. our hashtable size).
