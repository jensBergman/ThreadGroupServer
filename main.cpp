/* 
 * File:   main.cpp
 * Author: jesse
 *
 * Created on den 31 december 2015, 13:01
 */

#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// necessary networking headers
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

// boost, threadpool
//#include <boost/lambda/lambda.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "Logic.h"

using namespace std;

/*
 // threads can call a function when they exit see sleep_print to see how to call it.
void exitThread(){
    cout << "Exit thread!" << endl; return;
}

// not used for server, but an example how to use functions
int sleep_print(int ms, boost::thread_group & thg, boost::shared_ptr<boost::thread> * thisTh) {
    boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
    std::cout << "goint to sleep (" << ms << ")" << std::endl;
    boost::this_thread::sleep(boost::posix_time::milliseconds(ms));
    std::cout << "wake up (" << ms << ")" << std::endl;
    boost::this_thread::at_thread_exit(boost::bind(&exitThread)); 
    
    thg.remove_thread(thisTh->get());
    delete thisTh;
    return 0;
}*/

/*
 * Server that uses a threadpool
 */
int main(int argc, char** argv) {

    // create a threadpool (thread safe). With thread_group we have full control of threads life time
    boost::thread_group threadpool;

    // file descriptors
    int sockfd, newsockfd;
    int port = 9999;
    int pid; // process id
    // stores the address size of the client
    socklen_t client_length;
    
    // internet addresses
    struct sockaddr_in server_addr, client_addr;
    
    /*
     * AF_INET = Internet domain
     * SOCK_STREAM = TCP socket
     * 0 = let the operating choose the most appropiate protocol (i.e. 3rd arg = protocol)
     */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        cout << "error opening socket!" << endl;
        return 0;
    }cout << "Socket opened!" << endl;
    
    /*
     * bzero() sets all values in a buffer to zeros
     * 1st arg: buffer
     * 2nd arg: buffer length
     */
    bzero((char *) &server_addr, sizeof(server_addr));
    
    server_addr.sin_family = AF_INET; // network domain
    server_addr.sin_addr.s_addr = INADDR_ANY; // IP addr of machine
    server_addr.sin_port = htons(port); // define port number in network byte order
    
    if (bind(sockfd, (struct sockaddr *) &server_addr,sizeof(server_addr)) < 0){
        cout << "Failed to bind socket" << endl;
        close(sockfd);
        return 0;
    }cout << "Socket bound!" << endl;
    
    /*
     * The listen system call allows the process to listen on the socket for connections. 
     * The first argument is the socket file descriptor, and the second is the size of the backlog queue,
     *  i.e., the number of connections that can be waiting while the process is handling a particular connection. 
     * This should be set to 5, the maximum size permitted by most systems. 
     * If the first argument is a valid socket, this call cannot fail, and so the code doesn't check for errors. 
     */
    listen(sockfd,5);
    cout << "Waiting for clients ..." << endl;
    client_length = sizeof(client_addr);
      
    // accept many connections simultaneously
    while(true){
        if(threadpool.size() < 2){
            // process is blocket until client connects, then it wakes up the process.
            newsockfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_length);
            if(newsockfd < 0){
                cout << "Error when accepting client connection" << endl;
                close(sockfd);
                return 0;
            }cout << "Client connection accepted!" << endl;

            // create memory for thread pointer. Using shared_ptr to make it more thread safe
            boost::shared_ptr<boost::thread> *t = new boost::shared_ptr<boost::thread>();
            
            // call method mainLogic (method deletes the thread and removes it from threadpool).
            // Using make_shared t avoid memory leak in case of failure in creating object (should also be faster)
            *t = boost::make_shared<boost::thread>(boost::bind(&Logic::mainLogic, boost::ref(threadpool), t, newsockfd));
            //not for server - *t = boost::make_shared<boost::thread>(boost::bind(&sleep_print, 1000, boost::ref(threadpool), t));

            // add the created thread to the pool
            threadpool.add_thread(t->get());
            cout << "Number of threads: " << threadpool.size() << endl;
        }
    }
    // wait until all threads are finished
    threadpool.join_all();
    
    close(sockfd);
    
    return 0;
}