# Incremental Backup in C

Simple incremental backup implementation in C.

## Used libraries

* POSIX for recursive directory walking
* SQLite for storing file informations
* libarchive for storing the files

## Database structure

The database structure is stored in the file `database.sqlite` in the SQLite-format. All files included in the backup are stored in a table called "files". Create a new database-file with the following SQLite-command:

    CREATE TABLE "files" (path TEXT NULL, last_modified TIMESTAMP NULL, flag UNSIGNED TINYINT DEFAULT 0);

The column `path` is used to store the absolute path of the file, the `last_modified` column represents the timestamp of the last file modification and the column `flag` is used for the internal algorithm.