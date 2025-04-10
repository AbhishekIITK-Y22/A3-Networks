#include <iostream>
#include <cstring>
#include <cstdlib>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

#define SERVER_PORT 12345     // Taken from server.cpp
#define SERVER_IP "127.0.0.1"   // The server IP address (localhost)
#define CLIENT_PORT 54321     // Client port

// Function to print the TCP flags for debugging purposes
void print_output(struct tcphdr *tcp) {
    cout << "[+] TCP Flags: "
         << " SYN: " << (int)tcp->syn
         << " ACK: " << (int)tcp->ack
         << " FIN: " << (int)tcp->fin
         << " RST: " << (int)tcp->rst
         << " PSH: " << (int)tcp->psh
         << " SEQ: " << ntohl(tcp->seq) << endl;
}

// Function to send a packet (either SYN or final ACK) using a raw socket.
// This builds the IP and TCP headers for the packet.
void send_packet(int sock, const char *src_ip, const char *dest_ip, 
                 uint16_t src_port, uint16_t dst_port, 
                 uint32_t seq, uint32_t ack_seq, 
                 bool syn, bool ack) {
    
    // The packet is composed of an IP header followed by a TCP header.
    char packet[sizeof(struct iphdr) + sizeof(struct tcphdr)];
    memset(packet, 0, sizeof(packet));

    // Pointers to the IP and TCP headers within the packet.
    struct iphdr *ip = (struct iphdr *) packet;
    struct tcphdr *tcp = (struct tcphdr *)(packet + sizeof(struct iphdr));

    // --- Filling the IP header ---
    ip->ihl = 5;                               // IP header length (5 * 32-bit words)
    ip->version = 4;                           // IPv4
    ip->tos = 0;                               // Type of service
    ip->tot_len = htons(sizeof(packet));       // Total length of the packet
    ip->id = htons(54321);                     // Identification field
    ip->frag_off = 0;                          // No fragmentation
    ip->ttl = 64;                              // Time to live
    ip->protocol = IPPROTO_TCP;                // TCP protocol
    ip->saddr = inet_addr(src_ip);             // Source IP address
    ip->daddr = inet_addr(dest_ip);            // Destination IP address

    // --- Filling the TCP header ---
    tcp->source = htons(src_port);             // Source port
    tcp->dest = htons(dst_port);               // Destination port
    tcp->seq = htonl(seq);                     // Sequence number
    tcp->ack_seq = htonl(ack_seq);             // Acknowledgment number
    tcp->doff = 5;                             // Data offset (header length = 20 bytes)
    tcp->syn = syn ? 1 : 0;                      // Set SYN flag if 'syn' is true
    tcp->ack = ack ? 1 : 0;                      // Set ACK flag if 'ack' is true
    tcp->window = htons(8192);                 // Window size
    tcp->check = 0;                            // Checksum (kernel will compute)

    // --- Sending the packet ---
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(dst_port);
    dest_addr.sin_addr.s_addr = inet_addr(dest_ip);
    
    if(sendto(sock, packet, sizeof(packet), 0, 
              (struct sockaddr*)&dest_addr, sizeof(dest_addr)) < 0) {
        cout << "sendto() failed" << endl;
        exit(EXIT_FAILURE);
    } else {
        if(syn && !ack) {
            cout << "[+] Sent SYN (seq=200)" << endl;
        } else if(!syn && ack) {
            cout << "[+] Sent final ACK (seq=600)" << endl;
        }
    }
}

int main() {
    // Create a raw socket for TCP communication
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if(sock < 0) {
        cout << "Socket creation failed" << endl;
        exit(EXIT_FAILURE);
    }

    // Tell the kernel that the packet includes its own IP header
    int one = 1;
    if(setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        cout << "setsockopt failed" << endl;
        exit(EXIT_FAILURE);
    }

    // --------------------- Build and send the SYN packet ---------------------
    // Our SYN packet uses sequence number 200.
    send_packet(sock, "127.0.0.1", SERVER_IP, CLIENT_PORT, SERVER_PORT, 
                200, 0, true, false);

    // ------------------ Wait for the server's SYN-ACK response -----------------
    char buffer[65536];
    struct sockaddr_in source_addr;
    socklen_t addr_len = sizeof(source_addr);

    while(true) {
        int data_size = recvfrom(sock, buffer, sizeof(buffer), 0, 
                                 (struct sockaddr*) &source_addr, &addr_len);
        if(data_size < 0) {
            cout << "recvfrom failed" << endl;
            continue;
        }

        // Extract IP and TCP headers from the received packet.
        struct iphdr *ip = (struct iphdr *) buffer;
        struct tcphdr *tcp = (struct tcphdr *)(buffer + (ip->ihl * 4));

        // Check if the packet is from the server (source IP should match SERVER_IP)
        if(strcmp(inet_ntoa(*(struct in_addr *)&ip->saddr), SERVER_IP) != 0)
            continue;
        
        // Check if the packet is destined to our chosen client port.
        if(ntohs(tcp->dest) != CLIENT_PORT)
            continue;

        // Print the TCP flags for debugging
        print_output(tcp);

        // The server should send a SYN-ACK:
        // - Server's sequence number should be 400.
        // - Acknowledgment number should be 201 (client seq 200 + 1)
        if(tcp->syn == 1 && tcp->ack == 1 && ntohl(tcp->seq) == 400) {
            cout << "[+] Received SYN-ACK from " << inet_ntoa(source_addr.sin_addr) << endl;

            // ----------------- Send the final ACK ------------------
            // The final ACK uses sequence number 600.
            // The acknowledgment number should be 401 (server's seq 400 + 1)
            send_packet(sock, "127.0.0.1", SERVER_IP, CLIENT_PORT, SERVER_PORT, 
                        600, 401, false, true);

            cout << "[+] Handshake complete" << endl;
            break;
        }
    }

    close(sock);
    return 0;
}
