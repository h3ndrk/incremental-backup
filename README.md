# Incremental Backup in C

This program is a implementation of an efficient incremental backup solution. It stores file informations in a human-readable textfile (called 'index file'). The backups are stored in tar-archives.

For a full list of options try:

    backup --help

## Features

* full backup per command line option
* user questions which are skippable per command line option
* several compression algorithms supported:
    * b64encode
    * **bzip2**
    * **compress**
    * grzip
    * **gzip**
    * lrzip
    * lzip
    * lzop
    * uuencode
    * **xz**
* automatic use of absolute paths
* Stable with tested features
* incremental backup
* backup archive as (compressed) tar file
* exclude patterns per command line option
* skip hidden files per command line option

## Why another backup program?

This program tries to be very efficient. That's why the program is fast and uses a tiny amount of memory. In addition it supports incremental backups as a core feature. On the other hand the efficiency does not mean that it has less features. The user is able to use a bunch of features like several mainly used compression algorithms. These characteristics make this backup program an individual program with features that other programs do not have.

## Installation

1. Install all libraries for the program (POSIX, libarchive)
2. Go into the directory of the program (where `Makefile` is stored): `cd path/to/the/program`
3. Compile the program: `make`
4. The program is located in `bin/backup`

For a full list of options try:

    bin/backup --help

## Used libraries

* POSIX for recursive directory walking
* libarchive for storing the files in an archive

## Tests

### Machine

* Intel Core i5 M480 @ 2.76 GHz x 4
* 3.7 GiB RAM
* Arch Linux 64-Bit

### Memory usage test

Memory usage with 856002 files stored from filesystem and 856002 files stored from index: 123.25 MB

## License

lastest GPL-license (see LICENSE-file)

NIPE-SYSTEMS
[http://www.nipe-systems.de](http://www.nipe-systems.de)
