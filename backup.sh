#!/bin/sh

bin/backup \ #
	--archive "/home/user/backup/initial_backup.tar.gz" \ # the archive to store
	--compress gzip \ # compression algorithm: GZIP
	--index "/home/user/backup/index" \ # existing index directory (should be empty on first use)
	--source "/home/user/Programme/can" \ # backup directory which should be 'backupped'
	--exclude "/boot/*" \ # exclude shell wildcard pattern for all files in /boot
	--exclude "/dev/*" \ # exclude shell wildcard pattern for all files in /dev
	--exclude "/mnt/*" \ # exclude shell wildcard pattern for all files in /mnt
	--exclude "/proc/*" \ # exclude shell wildcard pattern for all files in /proc
	--exclude "/run/*" \ # exclude shell wildcard pattern for all files in /run
	--exclude "/sys/*" \ # exclude shell wildcard pattern for all files in /sys
	--exclude "/tmp/*" \ # exclude shell wildcard pattern for all files in /tmp
	--exclude "/home/user/backup/*" \ # exclude shell wildcard pattern for all files in the backup directory (don't backup the backup ;) )
	--verbose \ # verbose output
	--full # full initial backup