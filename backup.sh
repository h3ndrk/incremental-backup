#!/bin/sh

bin/backup \ #
	--archive "/home/hendrik/backup/initial_backup.tar.gz" \ # the archive to store
	--compress gzip \ # compression algorithm: GZIP
	--index "/home/hendrik/backup/index" \ # existing index directory (should be empty on first use)
	--s "/home/hendrik/Programme/can" \ # backup directory which should be 'backupped'
	--e "/boot/*" \ # exclude shell wildcard pattern for all files in /boot
	--e "/dev/*" \ # exclude shell wildcard pattern for all files in /dev
	--e "/mnt/*" \ # exclude shell wildcard pattern for all files in /mnt
	--e "/proc/*" \ # exclude shell wildcard pattern for all files in /proc
	--e "/run/*" \ # exclude shell wildcard pattern for all files in /run
	--e "/sys/*" \ # exclude shell wildcard pattern for all files in /sys
	--e "/tmp/*" \ # exclude shell wildcard pattern for all files in /tmp
	--e "/home/hendrik/backup/*" \ # exclude shell wildcard pattern for all files in /
	--v # verbose output
	--f # full initial backup