#include<bits/stdc++.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include<thread>
using namespace std;

#pragma comment(lib, "ws2_32.lib")


bool Initialize(){
    WSAData data;
    return WSAStartup(MAKEWORD(2,2), &data) == 0;
}

void sendMessage(SOCKET s){
    cout<<"Enter Your Name: ";
    string name, msg;
    getline(cin, name);
    while (1){
        getline(cin, msg);
        string message = name + ": " + msg;
        int bystesSent = send(s, message.c_str(), message.length(), 0);
        if(bystesSent == SOCKET_ERROR){
            cout<<"Failed to send message\n";
            break;
        }
        if(msg == "exit") break;
    }closesocket(s);
    WSACleanup();    
}

void receiveMessage(SOCKET s){
    char buffer[4096];
    int rcvlngth;
    while (1){
        rcvlngth = recv(s, buffer, sizeof(buffer), 0);
        if(rcvlngth <= 0){
            cout<<"Server disconnected\n";
            break;
        }else{
            cout<<string(buffer, rcvlngth)<<"\n";
        }
    }
    closesocket(s);
    WSACleanup(); 
}

int main(){
    if(!Initialize()){
        cout<<"Failed to initialize Winsock\n";
        return 1;
    }

    SOCKET s;
    s=socket(AF_INET, SOCK_STREAM, 0);
    if(s == INVALID_SOCKET){
        cout<<"Failed to create socket\n";
        return 1;
    }

    string serverAddress = "127.0.0.1";
    int port = 12345;
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    
    inet_pton(AF_INET, serverAddress.c_str(), &serverAddr.sin_addr);
    if(connect(s, (sockaddr*)&serverAddr, sizeof(serverAddr))== SOCKET_ERROR){
        cout<<"Failed to connect to server\n";
        closesocket(s);
        WSACleanup();
        return 1;
    }

    cout<<"Connected to server\n";

    thread senderThread(sendMessage, s);
    thread receiverThread(receiveMessage, s);

    senderThread.join();
    receiverThread.join();

    return 0;
}