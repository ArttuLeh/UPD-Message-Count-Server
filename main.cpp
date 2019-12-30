/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: Arttu Lehtovaara
 *
 * Created on May 17, 2019, 11:43 AM
 */

#include <iostream>

#include <string.h> // memset, strlen
#include <stdio.h> // sprintf

// headers for socket (UDP) communication
#include <unistd.h> // standard unix calls
#include <sys/types.h> // standard types for system calls
// for socket connections
#include <sys/socket.h> // basic socket calls
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h> // inet_ntoa

#include <signal.h> // sigaction, SIGINT

using namespace std;

// global variables for signal handling
int s; //= socket(AF_INET, SOCK_DGRAM, 0); // ipv4-socket, UDP // socket
bool end; // for main loop ending
/*
 * 
 */
int main(int argc, char** argv) 
{
    void sigint_handler(int sig); // handler prototype
    
    struct sigaction act; // for signal handling
    
    char bf[100], ebf[120]; // max 100 character message buffer, sbf=sendbuffer
    socklen_t l = sizeof(sockaddr_in); // the length of the sockaddr structure
    int n; // count of the received characters
    
    // create a socket object
    s = socket(AF_INET, SOCK_DGRAM, 0); // ipv4-socket, UDP
            
    // define listening port for socket (4330 for Arttu Lehtovaara)
    sockaddr_in server, client; // structure for socket address
    server.sin_port = htons(4330); // own unic port number
    server.sin_family = AF_INET; // ipv4 address
    server.sin_addr.s_addr = htonl(INADDR_ANY); // connection from anywhere
    bind(s, (sockaddr*)&server, sizeof(server)); // bind the address to the socket
    
    // initialize the signal handling for ctrl/c interrupt
    memset (&act, '\0', sizeof(act)); // clear the act structure
    act.sa_handler = sigint_handler;
    sigaction(SIGINT, &act, NULL); // set new handler
    
    // main loop for listening messages
    cout << "Message counting service\n";
    
    while (!end)
    {
        cout << "Listening port " << ntohs(server.sin_port) << endl;
        // call is blocking until someone sends a message
        n = recvfrom(s, bf, sizeof(bf)-1, 0, (sockaddr*)&client, &l);
        if (!end)//n > 0)
        {
            // this is executed only if we have got message
            bf[n] = 0; // string end character
            cout << "Addr: " << inet_ntoa(client.sin_addr) <<
                    " and message: " <<  bf << " and port: " <<
                    ntohs(client.sin_port) << " and message count: " << strlen(bf) << endl;
            
            // build the echo message
            sprintf(ebf, "%d", strlen(bf));
            
            // send the echo back to the received address
            sendto(s, ebf, strlen(ebf), 0, (sockaddr*)&client, l);
            if (bf[0] == 0)
            {
                end = true;
            }
        }
    }
    
    cout << "Server program is stopped..\n";
    close(s);
    return 0;
}
// signal handler for ctrl/c
void sigint_handler(int sig)
{
    cout << "Ctrl/c pushed\n";
    // close the socket and ens the main loop
    close(s);
    end = true;
}

