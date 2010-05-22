#!/bin/sh
git push
ssh labs.anyclip.com "\
(git clone /home/git/AnyLoader .al &&\
cd .al/daemon &&\
qmake && make && sudo make install && sudo /etc/init.d/anyloader restart);\
rm -rf .al"
../GitTools/UploadToWebserver.sh web/