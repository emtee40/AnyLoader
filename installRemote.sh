#!/bin/sh
rsync -vza --exclude=anyloader --exclude=*.o --exclude=moc_*.cpp --exclude Makefile --delete daemon labs.anyclip.com: &&
ssh labs.anyclip.com "cd daemon && qmake "CONFIG+=debug" && make && sudo make install && sudo /etc/init.d/anyloader restart"
../GitTools/UploadToWebserver.sh web/
