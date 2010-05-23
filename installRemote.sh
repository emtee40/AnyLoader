#!/bin/sh
rsync -vza --exclude=anyloader --exclude=*.o --exclude=moc_*.cpp --exclude Makefile daemon labs.anyclip.com: &&
ssh labs.anyclip.com "cd daemon && qmake && make && sudo make install && sudo /etc/init.d/anyloader restart"
../GitTools/UploadToWebserver.sh web/
