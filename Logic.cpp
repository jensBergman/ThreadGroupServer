/* 
 * File:   Logic.cpp
 * Author: jesse
 * 
 * Created on den 1 januari 2016, 19:17
 */

#include "Logic.h"

using namespace std;

Logic::Logic() {
}

Logic::~Logic() {
}

void Logic::mainLogic(boost::thread_group & thg, boost::shared_ptr<boost::thread> * thisTh, int newsockfd) {
int stream_length;
    while(true){
        
        // contains the number of characters read or written
        int stream_length;
        // read-buffer
        char buffer[256];

        // read from client ------- //

        bzero(buffer, 256);
        // read stream from client
        stream_length = read(newsockfd, buffer, 1);

        if (stream_length < 0) {
            cout << "Error reading from socket" << endl;
            close(newsockfd);
            return;
        }
        cout << "Client said: " << buffer[0] << endl;

        send(newsockfd, "I got your message!", 19, 0);
        if (stream_length < 0) {
            cout << "Error, failed to write to client" << endl;
            close(newsockfd);
            return;
        }
        cout << "Responded to client!" << endl;
        if(buffer[0] == 'q'){
            cout << "ended" << endl; break;
        }
    }
    close(newsockfd);


    thg.remove_thread(thisTh->get()); // remove the thread from the threadpool
    delete thisTh; // delete the thread
    thisTh = NULL; // delete operator will not work on the pointer later on


}
