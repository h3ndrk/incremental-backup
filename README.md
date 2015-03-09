# Incremental Backup in C

Simple incremental backup implementation in C.

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

## Used libraries

* POSIX for recursive directory walking
* libarchive for storing the files in an archive