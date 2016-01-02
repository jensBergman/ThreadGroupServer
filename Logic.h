/* 
 * File:   Logic.h
 * Author: jesse
 *
 * Created on den 1 januari 2016, 19:17
 */

#ifndef LOGIC_H
#define	LOGIC_H

#include <iostream>
#include <string>

// necessary networking headers
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

// boost
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

class Logic {
public:
    Logic();
  
    virtual ~Logic();
    
    static void mainLogic(boost::thread_group & thg, boost::shared_ptr<boost::thread> * thisTh, int newsockfd);
    static void readLogic(int newsockfd);
private:
};

#endif	/* LOGIC_H */

