# Quick starting guide #

**Dos9** is a open source command prompt meant to implement a superset of 
**cmd.exe** script language \(known as **batch** by most devellopers\). It is 
designed to be as simple to use as possible, reliable, portable and 
lightweight.

## Getting started with dos9 ##

Firstly, if you have never used cmd or an equivalent interpreter before, you 
should definitely consider reading a tutorial about batch programming. On the 
other hand if you are already familiar with batch, just open **Dos9** and 
start typing some commands ! Dos9 batch dialect is almost compatible with cmd.

If you use Dos9 on windows, the only file extension provided is **.dos9** by 
default \(to avoid trashing your OS with potential conflicts\). However, 
calling a **.bat** or **.cmd** script from inside Dos9, leads them to get 
executed by Dos9.

## Dos9 changes ##

Once installed, it is quite straightforward to play with Dos9, especially if 
you are quite familiar with cmd. However, there is a couple of thing out there 
you have to know about.

On one hand, there is a couple of differences between Dos9 and cmd :

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

* Dos9 has no inconsistencies with escaped characters with **^**, it requires 
  only one escape.

* **DIR /b** does not automatically return absolute paths.

* Support of **START** is a bit tricky under some platforms and some of the 
  options or the whole command might not be provided depending on your system 
  configuration if you use \*nix.

On the other hand, Dos9 also provides you with with a bunch of extensions:

* A full set of extensions to support floating points arithmetics through 
  **SET** and **IF**:

  * A new **SET /a:f** command to perform operations on floating point 
    numbers.

  * New comparisons like **FEQ** or **FNEQ** for the **IF** command to compare 
    floating-point numbers.

* The ground-breaking possibility to specify logical expressions using 
  **AND** and **OR** and the new **IF** extensions, like in the following 
  example:

        IF [ [ !ok! EQU 1 ] and [ defined file ] ] (
         :: some code 
        )

* To speed up parsing, Dos9 loads files entirely in memory at startup. If the 
  file gets modified during its execution, it is reloaded and Dos9 restarts on 
  the next line \(counting lines from the begining\). This can also be 
  disabled using:

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

Dos9 supports only two languages yet \(**french** and **english**\). If you 
cannot see the messages translated, please check the value of the **LANG** and 
**LANGUAGE** environment variables and set them appropriately.

If you have some questions about or need some help with Dos9, please feel free 
to join **Dos9**'s official discord at: 
[https://discord.gg/w4KtdCv](https://discord.gg/w4KtdCv)

There is a few interesting things in this directory that you might need:

* [BUILD.readme](BUILD.readme) : A text file describing how to build **dos9**

* [WHATSNEW](WHATSNEW.md) : A log of the changes made to Dos9

* [THANKS](THANKS.md) : A list of all the contributors to the project.

* [Guidelines](GUIDELINES.md) : A text files giving guidelines on how to 
  modify Dos9.

* **COPYING.\*** : The license of the project and of projects used by Dos9.

