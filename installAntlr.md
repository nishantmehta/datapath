# Installing Antlr #

This guide will help you install the Antlr parser generator and it's C runtime.

DataPath currently requires version 3.4 of Antlr and it's C runtime.

## Automatic Installation ##

Antlr is available as a package in the repositories of many popular Linux distributions. However, some distribution have an out of date version of Antlr that will not work with DataPath. In particular, **Ubuntu** does not have a recent enough version of Antlr in their repositories. Users of Ubuntu will need to proceed with the steps for manual installation.

Fedora 17 is known to contain a compatible version of Antlr in their repositories. If you use Fedora, you may install Antlr by running the following:

```
sudo yum install antlr3-tool antlr3-C
```

## Manual Installation ##

### Requirements ###

To install Antlr and its runtime, you will require:

  * Java
  * A C compiler (GCC works)

### Installing the Antlr parser generator ###

First, download the pre-compiled Antlr jar from [here](http://www.antlr.org/download/antlr-3.4-complete.jar). Place this file in a location accessible by those running DataPath. A good place to put it is /usr/local/share/.

Next, we will need to create a simple script to act as a front-end to Antlr and the JVM. Place the following script at /usr/local/bin/antlr3

```
#!/bin/sh

# Replace this with the location of the antlr jar on your system.
ANTLR_JAR=/usr/local/share/antlr-3.4-complete.jar

java -jar $ANTLR_JAR "$@"
```

Next, make this file executable by running the following command (note: you may have to run this command as the superuser depending on your permissions.):

```
chmod +x /usr/local/bin/antlr3
```

### Installing the C Runtime ###

Download version 3.4 of the C runtime from [here](http://www.antlr.org/download/C/libantlr3c-3.4.tar.gz). Extract the archive (`tar -xf libantlr3c-3.4.tar.gz` will do this).

Inside the extracted folder, run the following commands:

```
./configure --enable-64bit
make
make install
```

Note that is is _very_ important to have the `--enable-64bit` option set, as by default the runtime will be built for 32-bit systems if it is omitted, and will not link properly to DataPath.