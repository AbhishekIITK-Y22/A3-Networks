# TCP Handshake Implementation

## Overview

This project demonstrates the implementation of the TCP three-way handshake using raw sockets in C++. It includes both client and server programs that establish a TCP connection by manually crafting and interpreting TCP packets. The handshake process involves:

1. **SYN Packet:** The client initiates the connection with a sequence number.
2. **SYN-ACK Packet:** The server responds, acknowledging the client's sequence number and providing its own.
3. **ACK Packet:** The client acknowledges the server's sequence number, completing the handshake.

This implementation provides insights into low-level network programming and the mechanics of TCP connections.

## Contents

- `client.cpp`: Source code for the TCP client.
- `server.cpp`: Source code for the TCP server.
- `Makefile`: Facilitates the compilation of both programs.
- `README.md`: This documentation file.

## Requirements

- **Operating System:** Linux-based system (e.g., Ubuntu, Debian).
- **Privileges:** Root access is required to create and send raw packets.
- **Compiler:** `g++` supporting C++17 standard.
- **Libraries:** Standard networking libraries such as `<netinet/ip.h>`, `<netinet/tcp.h>`, and `<arpa/inet.h>`.

## Setup Instructions

1. **Clone the Repository**
   ```bash
   git clone https://github.com/your-repository/tcp-handshake-implementation.git
   cd tcp-handshake-implementation
   ```

2. **Compile the Code**
   Use the provided Makefile:
   ```bash
   make
   ```
   Alternatively, compile manually:
   ```bash
   g++ -std=c++17 client.cpp -o client
   g++ -std=c++17 server.cpp -o server
   ```

3. **Run the Server**
   Start the server first:
   ```bash
   sudo ./server
   ```
   The server will listen for incoming connections and handle the handshake process.

4. **Run the Client**
   In a separate terminal, run the client:
   ```bash
   sudo ./client
   ```
   The client will initiate the handshake with the server.

## Expected Output

### Server Output:
```
[+] TCP Flags: SYN: 1 ACK: 0 FIN: 0 RST: 0 PSH: 0 SEQ: 200
[+] Received SYN from 127.0.0.1
[+] Sent SYN-ACK
[+] TCP Flags: SYN: 0 ACK: 1 FIN: 0 RST: 0 PSH: 0 SEQ: 600
[+] Received ACK, handshake complete.
```

### Client Output:
```
[+] Sent SYN (seq=200)
[+] TCP Flags: SYN: 1 ACK: 1 FIN: 0 RST: 0 PSH: 0 SEQ: 400
[+] Received SYN-ACK from 127.0.0.1
[+] Sent final ACK (seq=600)
[+] Handshake complete.
```

## Key Concepts

- **Raw Sockets:** Enable direct sending and receiving of IP packets without the usual transport layer processing.
- **TCP Three-Way Handshake:**
  - **SYN:** Client sends a synchronization packet to initiate a connection.
  - **SYN-ACK:** Server responds, acknowledging the client's request and synchronizing its own sequence number.
  - **ACK:** Client acknowledges the server's response, establishing the connection.
