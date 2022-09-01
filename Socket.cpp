#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <strings.h>
#include <stdlib.h>
#include <string>
#include <pthread.h>
#include <sstream>
#include "Orderbook_base.cpp"

using namespace std;

void *task1(void *);

static int connFd;

int main(int argc, char* argv[])
{
    int pId, portNo, listenFd;
    socklen_t len; //store size of the address
    bool loop = false;
    struct sockaddr_in svrAdd, clntAdd;
    
    pthread_t threadA[3];
    
    if (argc < 2)
    {
        cerr << "Syntax : ./server <port>" << endl;
        return 0;
    }
    
    portNo = atoi(argv[1]);
    
    if((portNo > 65535) || (portNo < 2000))
    {
        cerr << "Please enter a port number between 2000 - 65535" << endl;
        return 0;
    }
    
    //create socket
    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    
    if(listenFd < 0)
    {
        cerr << "Cannot open socket" << endl;
        return 0;
    }
    
    bzero((char*) &svrAdd, sizeof(svrAdd));
    
    svrAdd.sin_family = AF_INET;
    svrAdd.sin_addr.s_addr = INADDR_ANY;
    svrAdd.sin_port = htons(portNo);
    
    //bind socket
    if(bind(listenFd, (struct sockaddr *)&svrAdd, sizeof(svrAdd)) < 0)
    {
        cerr << "Cannot bind" << endl;
        return 0;
    }
    
    listen(listenFd, 5);
        
    int noThread = 0;

    while (noThread < 3)
    {
        cout << "Listening" << endl;
        len = sizeof(clntAdd);
        //this is where client connects. svr will hang in this mode until client conn
        connFd = accept(listenFd, (struct sockaddr *)&clntAdd, &len);

        if (connFd < 0)
        {
            cerr << "Cannot accept connection" << endl;
            return 0;
        }
        else
        {
            cout << "Connection successful" << endl;
        }
        
        pthread_create(&threadA[noThread], NULL, task1, NULL); 
        
        noThread++;
    }
    
    for(int i = 0; i < 3; i++)
    {
        pthread_join(threadA[i], NULL);
    }
    
    
}

void *task1 (void *dummyPt)
{
    cout << "Thread No: " << pthread_self() << endl;
    char test[300];
    bzero(test, 301);
    bool loop = false;
    stringstream ss;

    while(!loop){    
        bzero(test, 301);
        read(connFd, test, 300);
        string tester (test);
        ss = stringstream();
        ss << tester;
        int number;
        float price;
        bool bid;
        int id;
        // 1 101 1
        ss >> number >> price >> bid;
        // cout << tester << endl;
        if (tester!=""){
            // cout << "ORDER PLACED" << endl;
            // cout << number << price << bid << endl;
            id = book->add_order(current_i++, number, price, bid);
            if (bid){
                cout << "ORDER ID " << id << " BUYING " << number << " SHARES at " << price << endl;
            } else {
                cout << "ORDER ID " << id << " SELLING " << number << " SHARES at " << price << endl;
            }
        }
        
        if(tester == "exit") {
            break;
        } 
    }
    
    cout << "\nClosing thread and conn" << endl;
    close(connFd);
    return nullptr;
}