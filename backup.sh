#!/bin/sh

bin/incremental-backup -a "/home/hendrik/backup/`date "+%y%m%d_%H%M%S"`.tar.gz" -c gzip -i "/home/hendrik/backup/index" -s "/" -e "/boot/*" -e "/dev/*" -e "/mnt/*" -e "/proc/*" -e "/run/*" -e "/sys/*" -e "/tmp/*" -e "/home/hendrik/backup/*" -v -t
