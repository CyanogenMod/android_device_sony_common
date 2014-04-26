#!/bin/bash

VENDOR=sony

BASE=../../../vendor/$VENDOR/$DEVICE/proprietary

while getopts ":nh" options
do
  case $options in
    n ) NC=1 ;;
    h ) echo "Usage: `basename $0` [OPTIONS] "
        echo "  -n  No clenup"
        echo "  -h  Show this help"
        exit ;;
    * ) ;;
  esac
done

if [ "x$NC" != "x1" ]; then
  rm -rf $BASE/*
fi

for FILE in `grep -v ^# ../$DEVICE/proprietary-files.txt | grep -v ^$ | sort`
do
  # Split the file from the destination (format is "file[:destination]")
  OLDIFS=$IFS IFS=":" PARSING_ARRAY=($FILE) IFS=$OLDIFS
  FILE=${PARSING_ARRAY[0]}
  DEST=${PARSING_ARRAY[1]}
  if [[ "$FILE" =~ ^-.* ]]; then
    FILE=`echo $FILE | sed s/^-//`
  fi
  if [ -z "$DEST" ]; then
    DEST=$FILE
  fi
  DIR=`dirname $FILE`
  if [ ! -d $BASE/$DIR ]; then
    mkdir -p $BASE/$DIR
  fi
  adb pull /system/$FILE $BASE/$DEST
  # if file dot not exist try destination
  if [ "$?" != "0" ]; then
    adb pull /system/$DEST $BASE/$DEST
  fi
done

exit 0
