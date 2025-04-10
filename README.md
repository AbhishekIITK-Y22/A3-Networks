# TCP Handshake Implementation

## Overview

This project implements the client-side of the TCP three-way handshake using raw sockets in C++. The server-side code is provided and handles the server's part of the handshake. This assignment’s main objective is to manually construct and exchange TCP packets (SYN, SYN-ACK, ACK) to demonstrate the low-level mechanics of TCP connections.

## Contents

- `client.cpp`: Source code for the TCP client. This code constructs and sends a SYN packet, receives a SYN-ACK from the server, and sends the final ACK to complete the handshake.
- `server.cpp`: Provided server-side code that listens for the SYN, responds with a SYN-ACK, and waits for the final ACK.
- `Makefile`: A file to compile both client and server programs.
- `README.md`: This documentation file.

## Requirements

- **Operating System**: Linux-based (e.g., Ubuntu, Debian)
- **Privileges**: Root access (required for sending raw packets via raw sockets)
- **Compiler**: A C++17-compliant compiler (e.g., `g++`)
- **Libraries**: Standard networking libraries (`<netinet/ip.h>`, `<netinet/tcp.h>`, `<arpa/inet.h>`)

## Setup Instructions

### Step 1: Clone the Repository

```bash
git clone https://github.com/privacy-iitk/cs425-2025.git
cd cs425-2025/Homeworks/A3
```

### Step 2: Compile the Code

To compile the client and server programs:

```bash
g++ -std=c++17 client.cpp -o client
g++ -std=c++17 server.cpp -o server
```

Alternatively, use the provided Makefile:

```bash
make
```

### Step 3: Run the Server

Start the server first (it listens on port `12345`):

```bash
sudo ./server
```

### Step 4: Run the Client

In a separate terminal, start the client:

```bash
sudo ./client
```

### Step 5: Verify the Handshake

#### Server Output:

```
[+] TCP Flags:  SYN: 1 ACK: 0 FIN: 0 RST: 0 PSH: 0 SEQ: 200
[+] Received SYN from 127.0.0.1
[+] Sent SYN-ACK
[+] TCP Flags:  SYN: 0 ACK: 1 FIN: 0 RST: 0 PSH: 0 SEQ: 600
[+] Received ACK, handshake complete.
```

#### Client Output:

```
[+] Sent SYN (seq=200)
[+] TCP Flags:  SYN: 1 ACK: 1 FIN: 0 RST: 0 PSH: 0 SEQ: 400
[+] Received SYN-ACK from 127.0.0.1
[+] Sent final ACK (seq=600)
[+] Handshake complete
```

### Step 6: Server Termination

After the handshake completes successfully, the server code is designed to automatically exit. If for any reason the server does not exit or you wish to terminate it early, you may press `Ctrl+C` to manually stop it.

---

## Key Concepts

### Raw Sockets

Raw sockets allow direct manipulation of IP and TCP headers. This project manually sets these headers, bypassing the operating system’s standard TCP/IP stack.

### TCP Three-Way Handshake

- **SYN**: The client sends a SYN packet with an initial sequence number (`200`).
- **SYN-ACK**: The server responds with a SYN-ACK, using a sequence number (`400`) and acknowledging the client’s sequence (expecting `201`).
- **ACK**: The client sends an ACK with a sequence number (`600`) and acknowledges the server’s sequence (`401`).

---

## Explanation of the Code

### Client-side Code (`client.cpp`)

- **Constructing the SYN Packet**  
  The client builds a packet with an IP header and a TCP header. The SYN packet is sent with sequence number `200`.

- **Processing SYN-ACK**  
  The client listens for packets from the server. It filters to ensure the packet is from the expected source and port, verifies that the SYN-ACK contains the correct sequence number (`400`), and confirms the flags.

- **Final ACK**  
  After validating the server’s response, the client sends the final ACK with sequence number `600` and the acknowledgment number set appropriately (server sequence + 1).

### Server-side Code (`server.cpp`)

- **Listening and Filtering**  
  The server uses raw sockets to monitor incoming packets on port `12345`. It only processes packets meant for this port.

- **SYN Handling and Response**  
  Upon receiving a valid SYN (with sequence number `200`), the server sends a SYN-ACK packet with sequence number `400` and acknowledges the client’s packet (expecting an ACK of `201`).

- **Completing the Handshake**  
  Once the final ACK is received (with sequence number `600`), the server prints a message indicating that the handshake is complete and then automatically terminates.

---

## Troubleshooting

- **Root Privileges**  
  Ensure you run the programs with `sudo` since raw sockets require elevated permissions.

- **Dependencies**  
  Verify that `g++` and all required networking libraries are installed.

- **Firewall Settings**  
  Firewalls may block raw socket traffic. Adjust settings as necessary or disable temporarily for testing.

- **Packet Verification**  
  Use the debug output from the programs to verify that packets have correct sequence numbers and flags.

---

## Conclusion

This project provides a hands-on understanding of how the TCP three-way handshake works by manually crafting and exchanging TCP packets using raw sockets. The detailed instructions and explanations in this README, along with clearly commented source code, should help you learn and verify the process effectively.
