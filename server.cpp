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

void Interaction(SOCKET clientSocket, vector<SOCKET>& clients){
    cout<<"Client connected\n";
    char buffer[4096];
    while (1){
        int bystesRecvd = recv(clientSocket, buffer, sizeof(buffer), 0);
        if(bystesRecvd <= 0){
            cout<<"Client disconnected\n";
            break;
        }
        string message(buffer, bystesRecvd);
        cout<<"Message received: "<<message<<"\n";
        for(auto client : clients){
            if(client != clientSocket)
            send(client, message.c_str(), message.length(), 0);
        }
    }
    auto it= find(clients.begin(), clients.end(), clientSocket);
    if(it != clients.end()) clients.erase(it);
    closesocket(clientSocket);
}

int main(){
    if(!Initialize()){
        cout<<"Failed to initialize Winsock\n";
        return 1;
    }

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(listenSocket == INVALID_SOCKET){
        cout<<"Failed to create socket\n";
        return 1;
    }
    sockaddr_in serverAddr; int port=12345;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if(inet_pton(AF_INET, "0.0.0.0", &serverAddr.sin_addr) !=1){
        cout<<"Failed to convert IP address\n";
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    if(bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR ){
        cout<<"Failed to bind socket\n";
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    
    if(listen(listenSocket, SOMAXCONN)  == SOCKET_ERROR){
        cout<<"Failed to listen\n";
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    cout<<"Listening... at port "<<port<<"\n";
    vector<SOCKET> clients;
    while(1){
        SOCKET clientSocket = accept(listenSocket, NULL, NULL);
        if(clientSocket == INVALID_SOCKET){
            cout<<"Failed to accept client\n";
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }
        clients.push_back(clientSocket);
        thread t1(Interaction, clientSocket, std::ref(clients));
        t1.detach();
    }
    closesocket(listenSocket);


    WSACleanup();
    return 0;
}