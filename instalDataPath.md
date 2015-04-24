# Installation Guide #

## System Requirements ##

  * A 64-bit processor. Multiple multi-core processors recommended.
  * A large amount of RAM is recommended.
  * A relatively recent Linux distribution.

### Software Requirements ###

To compile DataPath, the following software is required:

  * GCC with C++ support (preferably 4.7+, although earlier versions will work.)
  * An M4 macro processor (preferably GNU M4)
  * A bash-compatible shell.
  * pkg-config
    * Used to determine whether needed libraries are installed and properly link them.
    * Available in the repositories of most distributions.

### Third-Party Libraries ###

The following third party libraries are required to compile and run DataPath:

  * [SQLite3](http://www.sqlite.org/)
    * Most modern Linux distributions make this available in their repositories.
  * Antlr 3.4 and its C runtime.
    * For a detailed installation guide, please read [Installing Antlr](installAntlr.md).
  * [Lemon](http://lemon.cs.elte.hu/trac/lemon) v1.2.3

Additionally, the following libraries are required to use parts of DataPath's standard library:

  * [Oniguruma](http://www.geocities.jp/kosako3/oniguruma/).
    * Used for regular expression support in pattern matching.
    * Available in several major repositories.
      * Fedora: `oniguruma`
      * Ubuntu: `libonig2`
  * [Miscellaneous Container Templates](https://launchpad.net/libmct)
    * Used for an efficient hash map and hash set implementation in some GLAs (including the Group By).

## Installation ##

First, either download a release of DataPath from the downloads section or check out the current trunk using subversion.

Navigate to the `src` directory, and run the following command:

```
./compile.datapath.sh
```

This script will give you an overview of the settings that will be used by DataPath as well as an estimate of how much RAM DataPath will use for its global hash. If these settings look ok, accept them and the script will automatically compile DataPath.

To install a front-end to DataPath that makes running queries easier, run the follwing:

```
sudo make install
```

Note that this frontend will not work properly unless you open a new terminal.

## Running Queries ##

Once DataPath has been installed correctly and the front-end is working properly, you can run queries by doing the following:

```
datapath run QueryFile.pgy
```