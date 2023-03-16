# Laika

J'ai commencé un début de Shellcode injection mais j'ai un peu la flemme de continuer. Pour bien faire il faudrait créer un process "zombie" qui va recevoir les shellcodes à injecter (agents/features), et un watcher qui le relance s'il meurt.<br>
L'agent N'EST PAS plug and play, il manque certaines choses, de plus c'est un projet perso et c'est pas vraiment fait pour être utilisé.<br>

# Agent

## Features :
* Reverse shell
* File explorer (download/upload/remove)

## Caractéristiques
* Petit (14ko)
* Fait en C, sans CRT
* x32 bit
* Modulaire (peut se déployer via shellcode/dll/pe injection/.exe)
* Trafic chiffré
* API resolve de manière dynamique
* Reconnect en cas de plantage serv/autre
* Liste de serveurs de fallback
* FUD

<details open>
<summary>TODO</summary>

* x64bit support
* Custom sleep
* Shellcode injection pour x64/x32 bits process
</details>

# Server

* Fait en Python
* Permet de build le client/faire la config
* File explorer (upload/download/remove)

<details open>
<summary>TODO</summary>

* Fix l'input qui bloque l'arrivée de données (reproductible avec une session powershell)
</details>


![Alt text](Screenshots/Server.PNG "Screenshot du server")
