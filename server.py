#!/usr/bin/env python3

import socket

HOST = "127.0.0.1"  # Standard loopback interface address (localhost)
PORT = 65432        # Port to listen on (non-privileged ports are > 1023)

def send_time(conn):
    conn.sendall("Ok!".encode())


def receive(conn):
    while True:
        data = conn.recv(1024)
        if not data:
            break
        print("Message received: ", data.decode())
        send_time(conn)


with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    while True:
        print(f"Waiting for connection in {HOST}:{PORT}")
        s.listen()
        conn, addr = s.accept()
        with conn:
            print("Connected by", addr)
            receive(conn)
            print("Connection Closed!")
