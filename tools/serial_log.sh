ls /dev/ttyUSB* 

if [ -e "/dev/ttyUSB0"  ]; then
date >> /dev/shm/ttyUSB0.txt 
sudo gnome-terminal --title="ttyUSB0" -x minicom -D /dev/ttyUSB0 -b 115200 -C /dev/shm/ttyUSB0.txt
fi

if [ -e /dev/ttyUSB1  ]; then 
date >> /dev/shm/ttyUSB1.txt 
sudo gnome-terminal --title="ttyUSB1" -x minicom -D /dev/ttyUSB1 -b 115200 -C /dev/shm/ttyUSB1.txt
fi

if [ -e /dev/ttyUSB2  ]; then
date >> /dev/shm/ttyUSB2.txt 
sudo gnome-terminal --title="ttyUSB2" -x minicom -D /dev/ttyUSB2 -b 115200 -C /dev/shm/ttyUSB2.txt
fi

if [ -e /dev/ttyUSB3  ]; then
date >> /dev/shm/ttyUSB3.txt
sudo gnome-terminal --title="ttyUSB3" -x minicom -D /dev/ttyUSB3 -b 115200 -C /dev/shm/ttyUSB3.txt
fi

if [ -e /dev/ttyUSB4  ]; then
date >> /dev/shm/ttyUSB4.txt 
sudo gnome-terminal --title="ttyUSB4" -x minicom -D /dev/ttyUSB4 -b 115200 -C /dev/shm/ttyUSB4.txt
fi

if [ -e /dev/ttyUSB5  ]; then
date >> /dev/shm/ttyUSB5.txt 
sudo gnome-terminal --title="ttyUSB5" -x minicom -D /dev/ttyUSB5 -b 115200 -C /dev/shm/ttyUSB5.txt
fi

if [ -e /dev/ttyUSB6  ]; then
date >> /dev/shm/ttyUSB6.txt 
sudo gnome-terminal --title="ttyUSB6" -x minicom -D /dev/ttyUSB6 -b 115200 -C /dev/shm/ttyUSB6.txt
fi

if [ -e /dev/ttyUSB7  ]; then
date >> /dev/shm/ttyUSB7.txt 
sudo gnome-terminal --title="ttyUSB7" -x minicom -D /dev/ttyUSB7 -b 115200 -C /dev/shm/ttyUSB7.txt
fi

if [ -e /dev/ttyUSB8  ]; then
date >> /dev/shm/ttyUSB8.txt
sudo gnome-terminal --title="ttyUSB8" -x minicom -D /dev/ttyUSB8 -b 115200 -C /dev/shm/ttyUSB8.txt 
fi

if [ -e /dev/ttyUSB9  ]; then
date >> /dev/shm/ttyUSB9.txt 
sudo gnome-terminal --title="ttyUSB9" -x minicom -D /dev/ttyUSB9 -b 115200 -C /dev/shm/ttyUSB9.txt 
fi

if [ -e /dev/ttyUSB10  ]; then
date >> /dev/shm/ttyUSB10.txt 
sudo gnome-terminal --title="ttyUSB10" -x minicom -D /dev/ttyUSB10 -b 115200 -C /dev/shm/ttyUSB10.txt
fi

if [ -e /dev/ttyUSB11  ]; then
date >> /dev/shm/ttyUSB11.txt 
sudo gnome-terminal --title="ttyUSB11" -x minicom -D /dev/ttyUSB11 -b 115200 -C /dev/shm/ttyUSB11.txt 
fi

if [ -e /dev/ttyUSB12  ]; then
date >> /dev/shm/ttyUSB12.txt 
sudo gnome-terminal --title="ttyUSB12" -x minicom -D /dev/ttyUSB12 -b 115200 -C /dev/shm/ttyUSB12.txt 
fi

if [ -e /dev/ttyUSB13  ]; then
date >> /dev/shm/ttyUSB13.txt 
sudo gnome-terminal --title="ttyUSB13" -x minicom -D /dev/ttyUSB13 -b 115200 -C /dev/shm/ttyUSB13.txt 
fi
