# Polytech Lille - IMA2A4 S7 - Tutorat Système

## Sujet du tutorat

[https://rex.plil.fr/Enseignement/Systeme/Tutorat.Systeme.IMA4/index.html](https://rex.plil.fr/Enseignement/Systeme/Tutorat.Systeme.IMA4/index.html)

## Description de la plaquette de la manette

[https://wiki-ima.plil.fr/mediawiki/index.php/IMA4_2018/2019_SEC4](https://wiki-ima.plil.fr/mediawiki/index.php/IMA4_2018/2019_SEC4)

## Préambule

Avant de compiler et d'utiliser les programmes, il est nécessaire que certains paquets soient installés.

La commande permettant d'installer ces paquets est la suivante :

```bash
apt install dfu-programmer avrdude avr-libc gcc-avr make build-essential gcc
```

## Fonctionnement

Le projet contient deux programmes :
- Un programme permettant de compiler et charger le firmware du microcontrôleur ATMega32u4 de la manette.
- Un programme C utilisant la bibliothèque LIBUSB pour contrôler les LEDs et vibreurs de la plaquette via la communication USB.

### 1. LUFA

Il est au préalable nécessaire de brancher et réinitialiser la manette.

Compilation et chargement du programme pour l'ATMega32u4 de la plaquette :

```bash
cd lufa-LUFA-170418/PolytechLille/Manette
make && ./upload
```

Le programme crée trois interfaces :
- Interface n°1 pour le joystick : 1 endpoint de type interruption IN (adresse 0x81) reconnu par le shell
- Interface n°2 pour les LEDs : 1 endpoint interrupt OUT (adresse 0x02)
- Interface n°3 pour les vibreurs : 1 endpoint interrupt OUT (adresse 0x03)

Les LEDs et vibreurs sont contrôlables via le programme libusb décrit ci-après.

### 2. LIBUSB

Différentes commandes sont disponibles pour le contrôle de la plaquette :

- h : Affiche les différentes commandes du programme
- q : Quitte le programme
- a,b,c,d : Commandes sur la plaquette (LEDs et vibreurs)

Compilation et lancement du programme :

- Brancher la manette (vérifier avec lsusb).
- Charger le firmware à l'aide des commandes ci-dessous :

```bash
cd libusb
make rebuild && sudo ./build/output
```

Le programme peut envoyer différentes requêtes comme allumer un vibreur par exemple.

## Mise en pratique de la manette

Pour tester la manette, il est possible d'utiliser les paquets suivants :
- wordwarvi : Jeu utilisant la manette
- jstest-gtk : Programme permettant de voir et de tester les propriétés de la manette

Pour installer ces paquets, il faut utiliser la commande ci-dessous.

```bash
apt install wordwarvi jstest-gtk
```
