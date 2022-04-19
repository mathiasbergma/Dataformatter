#
#!/bin/bash
sudo chmod u+x internetOverUSB
sudo cp internetOverUSB /root
sudo cp internetOverUSB.sh /etc/init.d
sudo chmod 755 /etc/init.d/internetOverUSB.sh
sudo ln -s /etc/init.d/internetOverUSB.sh /etc/rc5.d/S99internetOverUSB.sh
