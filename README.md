# Incremental Backup in C

Simple incremental backup implementation in C.

## Features

* full backup per command line option
* user questions which are skippable per command line option
* several compression algorithms supported:
    * b64encode
    * bzip2
    * compress
    * grzip
    * gzip
    * lrzip
    * lzip
    * lzop
    * uuencode
    * xz
* automatic use of absolute paths
* Stable with tested features
* incremental backup
* backup archive as (compressed) tar file
* exclude patterns
* skip hidden files

## Used libraries

* POSIX for recursive directory walking
* libarchive for storing the files in an archive