import requests
from bs4 import BeautifulSoup
from urllib.parse import urlparse
from urllib.parse import parse_qs
import json

url = f"https://www.gladir.com/CODER/CWINDOWS/apiwinprototypes.htm"
r = requests.get(url)

if r.status_code != 200 and r.status_code != 202 :
    print(f"Code != 200: {r.status_code}")
    exit(0)

html = r.text

soup = BeautifulSoup(html, 'html.parser')
table = soup.find('table', {'class': 'ListCoder'})

functions_list = []
for row in table.find_all('tr')[1:]:
    cells = row.find_all('td')
    
    # Function name
    function_name = cells[0].text.strip()

    # Prototype
    prototype = cells[1].text.strip()
    
    # Extract arguments types
    arguments = []
    if '(' in prototype:
        args_string = prototype.split('(')[1].split(')')[0].strip()
        if args_string:
            args = args_string.split(',')
            for arg in args:
                arg_type = ' '.join(arg.strip().split()[:-1])
                arguments.append(arg_type)

    # DLL name
    dll_name = cells[2].text.strip()

    # Add the function to the list
    functions_list.append({
        'function_name': function_name,
        'arguments': arguments,
        'dll_name': dll_name,
        'full_prototype': prototype.replace("\n", "").replace("\r", "").replace("\t", "")
    })

json_output = json.dumps(functions_list, ensure_ascii=False, indent=4)
with open("win_apis.json", "w") as f :
    f.write(json_output)
print(json_output)