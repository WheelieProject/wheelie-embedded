#!/bin/sh
echo "Compiling and deploying to particle electron"

rm *.bin
particle compile electron .

fileName="$(ls *.bin)"

echo "Deploying $fileName"

particle flash --usb $fileName
