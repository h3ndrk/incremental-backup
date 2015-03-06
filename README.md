# Incremental Backup in C

Simple incremental backup implementation in C.

## Used libraries

* POSIX for recursive directory walking
* SQLite for storing file informations
* libtar (in the future)

## Database structure

The database structure is stored in the file `database.sqlite` in the SQLite-format. All files included in the backup are stored in a table called "files". Create a new database-file with the following SQLite-command:

`CREATE TABLE "files" (path TEXT NULL, last_modified TIMESTAMP NULL, flag UNSIGNED TINYINT DEFAULT 0);`