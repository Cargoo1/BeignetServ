#!/usr/bin/env python3
import os
import sys
import time

def read_body():
    """Reads the body from STDIN based on CONTENT_LENGTH."""
    content_length_str = os.environ.get("CONTENT_LENGTH")
    if not content_length_str:
        return ""
    try:
        bytes_2_read = int(content_length_str)
        if bytes_2_read > 0:
            return sys.stdin.read(bytes_2_read)
    except ValueError:
        pass
    return ""

def find_filename():
    """Extracts the 'file=' parameter from QUERY_STRING."""
    query = os.environ.get("QUERY_STRING", "")
    target = "file="
    found_pos = query.find(target)
    
    if found_pos == -1:
        return None
        
    start_idx = found_pos + len(target)
    sep_pos = query.find('&', found_pos)
    
    sys.stderr.write(f"{sep_pos} {found_pos}\n")
    
    if sep_pos != -1:
        filename = query[start_idx:sep_pos]
    else:
        filename = query[start_idx:]
        
    sys.stderr.write(f"{filename}\n")
    return filename

def handle_get():
    filename = find_filename()
    if not filename:
        return 404, ""
        
    if not os.access(filename, os.F_OK | os.R_OK):
        return 404, ""
        
    try:
        with open(filename, "r") as f:
            body = f.read()
        return 0, body
    except IOError:
        return 404, ""

def handle_post(server_body):
    filename = find_filename()
    if not filename:
        return 404, ""
        
    path_info = os.environ.get("PATH_INFO")
    if path_info is not None:
        file_path = "." + path_info
        try:
            os.chdir(file_path)
        except OSError:
            return -1, ""
            
    sys.stderr.write(f"{filename}\n")
    try:
        with open(filename, "w") as f:
            f.write(server_body)
    except IOError:
        sys.stderr.write("Not open\n")
        
    return 0, "Hi from the script!!!"

def main():
    time.sleep(5)
    
    server_body = read_body()
    method = os.environ.get("REQUEST_METHOD", "")
    
    status = 0
    body = ""
    
    if method == "GET":
        status, body = handle_get()
    elif method == "POST":
        status, body = handle_post(server_body)
    else:
        status = -1
        
    if status != 0:
        sys.stdout.write("Status: 404 Not Found\r\n")
        
    sys.stdout.write(f"Content-Length: {len(body)}\r\n\r\n")
    sys.stdout.write(body)

if __name__ == "__main__":
    main()
