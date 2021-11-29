#!/bin/bash

if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

mknod /dev/simple_driver c 60 0
chmod 666 /dev/simple_driver
