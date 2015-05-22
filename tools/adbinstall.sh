
#!bin/sh
for file in $1
do
    if test -f $file
    then
        echo $file
        adb install $file
    else
        echo $file 是目录
    fi
done
