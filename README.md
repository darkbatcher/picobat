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

* The [HELP](doc/help) system is different from the original cmd help system. 
  On the first time you run the command, you need to genérate the whole 
  documentation using:

        help /b

  Once all the data has been generated you can use it as the **help** command, 
  but beware, it's fairly improved.

* There is a few [FOR](doc/for) modifications:

  * Empty line are processed by [FOR /F](doc/for). This behaviour can be 
    disabled using:

            SETLOCAL EnableCmdlyCorrect

  * Multiples lines can be used as input for **FOR /F**.

  * Tokens can be specified in reverse order and can also overlap without bug.

* Some undocumented but useful variables from cmd are still lacking \(Though 
  **%=EXITCODEASCII%** is now supported\).

* Picobat has no inconsistencies with escaped characters with **^**, it 
  requires only one escape.

* [DIR /b](doc/dir) does not automatically return absolute paths.

* Support of [START](doc/start) is a bit tricky under some platforms and some 
  of the options or the whole command might not be provided depending on your 
  system configuration if you use \*nix.

On the other hand, Picobat also provides you with with a bunch of extensions:

* A full set of extensions to support floating points arithmetics through 
  [SET](doc/set) and [IF](doc/if):

  * New [SET /a](doc/seta) commands extensions to perform operations on 
    floating point numbers.

  * Extensions for **IF** command to automatically detect floating-point and 
    compare floating-point numbers.

* The ground-breaking possibility to specify logical expressions using 
  **AND** and **OR** and the new [IF](doc/if) extensions, like in the 
  following example:

        IF [ [ !ok! EQU 1 ] and [ defined file ] ] (
         :: some code 
        )

* A module system allowing extension loading at run-time. This offer numbers 
  of possibilities from graphical user interfaces to networking extensions. 
  Currently, the only module provided is the [BATBOX](doc/batbox) module.

* To speed up parsing, Picobat loads files entirely in memory at startup. If 
  the file gets modified during its execution, it is reloaded and Picobat 
  restarts on the next line \(counting lines from the begining\). This can 
  also be disabled using:

        SETLOCAL EnableCmdlyCorrect

* As can be seen in the previous code, **::-style** comments can be used 
  inside [blocks](doc/spec/cmdline).

* All the [FOR](doc/for) modifications described above.

* Enhanced [GOTO](doc/goto) and [CALL](doc/call) that can use a file and a 
  label at the same time and ignore errors:

        GOTO :mylabel myfile.bat /Q
        CALL /e :mylabel myfile

* Extended [HELP](doc/help) providing search capabilities and help in various 
  formats including **HTML**.

* The new command [ALIAS](doc/alias) allowing to set up simple and fast 
  aliases for the commands:

        ALIAS dbs=dir /b /s

* An extension to the [FIND](doc/find) command to use simple regular 
  expressions:

        echo match my regular expression | FIND /e "match * regular expression"

* The [FIND](doc/find) and [MORE](doc/more) commands are provided as internal 
  commands.

* New internal commands [XARGS](doc/xargs) and [WC](doc/wc) inspired from 
  their \*nix counterparts:

  * [XARGS](doc/xargs) runs a command taking command parameters on the 
    standard input.

  * [WC](doc/wc) counts line or words or bytes in a file.

            :: Count lines in subdirs 
            dir /s /b /a:-D . | xargs wc /l

* New external command [DUMP](doc/dump) to dump hexadecimal code.

* New [SHIFT](doc/shift) extensions and new **%+** variable containing the 
  remaining arguments.

## Troubleshooting ##

If you have some questions about or need some help with Picobat, please feel 
free to join **Picobat**'s official discord at: 
[https://discord.gg/w4KtdCv](https://discord.gg/w4KtdCv)

If you want to dig deepeer, do not hesitate to browse the documentation. 
[Command index](doc/commands) and [Specification index](doc/spec/index) are 
good places to start. Have fun !

