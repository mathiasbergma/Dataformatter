Implementering
Usb-setup
Når USB GPS er tilsluttet en USB port blevet den automatisk oprettet i dev folderen som ”ttyACM0”.  Til at fasthold de navngivning er der implementeret en rule der reservere det navn til den.  

I Github folder Gokart_CAN_API/GPS_USB_NMEA_PARCER er der en fil der hedder 99-usb-serial.rules.   Den fil in holder følgende rule.

 

Dette rule sørger for at vores device med vendor ID ”1546” og produkt ID ”01a7” altid vil blive linket til ”ttyACM0” når forbundet.  Hvis man er tvivl om hvad de 2 ID er for sit GPS kan man benytte kommando ”lsusb” til at få en list over de forbundet USB devices, outputtet fra den ses her:
 

99-usb-serial.rules er inkluderet i github projectet og kan implementeres ved at køre en script der ligger i sammen mappe som hedder copy_usb_rules.sh.  Scriptet kan ses her og den kopier filen til den relevant rules mappe på den valgt beaglebone debian image.

 

GPS_USB_NMEA_PARCER
Den GPS_USB_NMEA_PARCER eksekverbar fil findes i undermappen /Debug.  I dette tilfald at den kompileret fil ikke er kompatibel men din embedded linux system, kan der køres ”make clean” og ”make” fra denne mappe efter man har hentet projektet ned til sit board. Programmet skal køres med sudo kommando da den skal have adgang til /dev mappen.

GPS_Deamon
Der ønskes at programmet kører automatisk ved opstart og sørger for at genstart hvis den mister forbindelse til GPS eller crasher under for et eller andet grund.  Derfor er der implementere en deamon til at overvåge vores process.  
 

I det tilfælde af en crash vil programmet start igen efter 5 sekunder og sørger for at den starter for alle bruger og kører med root access.  

Til at nemmere implementere GPSdeamon findes der en script i GPS_USB_NMEA_PARCER mappen som kopier filerne til systemd og giver instruktion om hvordan man enable og starter deamon.  Den ses her:

 

Gps service skulle nu være klar til at start selv når systemet booter og genstart i tilfeld af fejl.

