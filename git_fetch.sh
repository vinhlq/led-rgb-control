#!/bin/sh

chmod 400 .github/vinhlq.github.led-driver-lp50xx.ro.pem
# ssh-agent sh -c 'ssh-add .github/vinhlq.github.ro.pem; git fetch --all'
eval $(ssh-agent -s)
ssh-add .github/vinhlq.github.led-driver-lp50xx.ro.pem
git fetch --all
ssh-agent -k
git reset --hard origin/master
