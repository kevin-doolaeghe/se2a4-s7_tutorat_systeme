1) LIBUSB

La partie a été testée et est fonctionnelle.
a- brancher la manette (vérifier avec lsusb)
b- cd libusb
c- make rebuild && sudo ./build/output

Le programme peut envoyer différentes requètes comme allumer la led 4 par exemple.

2) LUFA

cd lufa-LUFA-170418/PolytechLille/Manette
make && ./upload

Le programme crée trois interfaces :
- interface 1 pour le joystick (1 endpoint interrupt in 0x81)
- interface 2 pour les leds (1 endpoint interrupt out 0x02)
- interface 3 pour les vibreurs (1 endpoint interrupt out 0x03)

les leds et vibreurs sont contrôlables via le programme libusb