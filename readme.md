# Laika

A very simple RAT I created for fun.<br>
You need mingw32 and Visual Studio 2022's clang to compile the agent btw.<br>

# Agent

## Features :
* Reverse shell
* File explorer (download/upload/remove/execute)
* Shellcode execution
* Server builder supports [patate-crypter](https://github.com/ALittlePatate/patate-crypter) inetgration
* [pasm](https://github.com/ALittlePatate/patate-crypter) integration in the agent

## Characteristics
* Small (26ko)
* Made in C, no CRT
* x32 bit
* Dynamic API resolving
* Multiple fallback servers
* FUD

<details open>
<summary>TODO</summary>

* x64bit support
* Custom sleep
* Shellcode injection cross architecture
</details>

# Server

* Made with Windows Forms (C#)
* Can build the client
* File explorer GUI (upload/download/remove/execute)
* Remote shell GUI
* PASM code editor

<details open>
<summary>TODO</summary>

* Dark theme
* colors in IDE
* lines number in IDE
* close warning in IDE
* Plugins support
</details>

# Compile
You need `make` and the `LLVM` toolchain on Windows to be able to compile.<br>
Then compile the server using VS2022 and build the payload from Laika's integrated build menu.


# Screenshots
![Alt text](Screenshots/main_menu.png "main menu")
![Alt text](Screenshots/builder.png "builder")
![Alt text](Screenshots/shell.png "shell")
![Alt text](Screenshots/fex.png "file explorer")
![Alt text](Screenshots/pasm_editor.png "pasm editor")