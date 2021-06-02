# Quick starting guide #

**Picobat** is an open source command prompt meant to implement a superset of 
**cmd.exe** script language \(known as **batch** by most developers\). It is 
designed to be as simple to use as possible, reliable, portable and 
lightweight.

## Getting started with Picobat ##

Firstly, if you have never used cmd or an equivalent interpreter before, you 
should definitely consider reading a tutorial about batch programming. On the 
other hand if you are already familiar with batch, just open **pbat** and 
start typing some commands ! Picobat batch dialect is almost compatible with 
cmd.

If you use Picobat on windows, the only file extension provided is **.pbat** 
by default \(to avoid trashing your OS with potential conflicts\). However, 
calling a **.bat** or **.cmd** script from inside Picobat, leads them to get 
executed by Picobat.

## Downloading Picobat ##

The latest release of picobat is available 
[here](https://github.com/darkbatcher/picobat/releases). Extract the archive 
and start playing with **pbat** in the extracted folder.

## Picobat changes ##

Once installed, it is quite straightforward to play with Picobat, especially 
if you are quite familiar with cmd. However, there is a couple of thing out 
there you have to know about.

On one hand, there is a couple of differences between Picobat and cmd :

* The **HELP** system is different from the original cmd help system. On the 
  first time you run the command, you need to genérate the whole documentation 
  using:

        help /b

  Once all the data has been generated you can use it as the **help** command, 
  but beware, it's fairly improved.

* There is a few **FOR** modifications:

  * Empty line are processed by **FOR /F**. This behaviour can be disabled 
    using:

            SETLOCAL EnableCmdlyCorrect

  * Multiples lines can be used as input for **FOR /F**.

  * Tokens can be specified in reverse order and can also overlap without bug.

* Some undocumented but useful variables from cmd are still lacking \(Though 
  **%=EXITCODEASCII%** is now supported\).

* Picobat has no inconsistencies with escaped characters with **^**, it 
  requires only one escape.

* **DIR /b** does not automatically return absolute paths.

* Support of **START** is a bit tricky under some platforms and some of the 
  options or the whole command might not be provided depending on your system 
  configuration if you use \*nix.

On the other hand, Picobat also provides you with with a bunch of extensions:

* A full set of extensions to support floating points arithmetics through 
  **SET** and **IF**:

  * New **SET /a** commands extensions to perform operations on floating point 
    numbers.

  * Extensions for **IF** command to automatically detect floating-point and 
    compare floating-point numbers.

* The ground-breaking possibility to specify logical expressions using 
  **AND** and **OR** and the new **IF** extensions, like in the following 
  example:

        IF [ [ !ok! EQU 1 ] and [ defined file ] ] (
         :: some code 
        )

* A module system allowing extension loading at run-time. This offer numbers 
  of possibilities from graphical user interfaces to networking extensions. 
  Currently, the only module provided is the **BATBOX** module.

* To speed up parsing, Picobat loads files entirely in memory at startup. If 
  the file gets modified during its execution, it is reloaded and Picobat 
  restarts on the next line \(counting lines from the begining\). This can 
  also be disabled using:

        SETLOCAL EnableCmdlyCorrect

* As can be seen in the previous code, **::-style** comments can be used 
  inside **blocks**.

* All the **FOR** modifications described above.

* Enhanced **GOTO** and **CALL** that can use a file and a label at the same 
  time and ignore errors:

        GOTO :mylabel myfile.bat /Q
        CALL /e :mylabel myfile

* Extended **HELP** providing search capabilities and help in various formats 
  including **HTML**.

* The new command **ALIAS** allowing to set up simple and fast aliases for the 
  commands:

        ALIAS dbs=dir /b /s

* An extension to the **FIND** command to use simple regular expressions:

        echo match my regular expression | FIND /e "match * regular expression"

* The **FIND** and **MORE** commands are provided as internal commands.

* New internal commands **XARGS** and **WC** inspired from their \*nix 
  counterparts:

  * **XARGS** runs a command taking command parameters on the standard input.

  * **WC** counts line or words or bytes in a file.

            :: Count lines in subdirs 
            dir /s /b /a:-D . | xargs wc /l

* New external command **DUMP** to dump hexadecimal code.

* New **SHIFT** extensions and new **%+** variable containing the remaining 
  arguments.

## Troubleshooting ##

Picobat supports only two languages yet \(**french** and **english**\). If you 
cannot see the messages translated, please check the value of the **LANG** and 
**LANGUAGE** environment variables and set them appropriately.

If you have some questions about or need some help with Picobat, please feel 
free to join **Picobat**'s official discord at: 
[https://discord.gg/w4KtdCv](https://discord.gg/w4KtdCv)

## What's in this folder ? ##

There is a few interesting things in this directory that you might need:

* [BUILD.readme](BUILD.readme) : A text file describing how to build 
  **Picobat**

* [WHATSNEW](WHATSNEW.md) : A log of the changes made to Picobat

* [THANKS](THANKS.md) : A list of all the contributors to the project.

* [GUIDELINES](GUIDELINES.md) : A text files giving guidelines on how to 
  modify Picobat.

* **COPYING.\*** : The license of the project and of projects used by Picobat.

