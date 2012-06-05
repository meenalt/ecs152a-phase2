//
//  Header.h
//  
//
//  Created by Jason Wong on 6/2/12
//  Copyright (c) 2012 University of California, Davis. All rights reserved.
//

#ifndef _HOST_H_
#define _HOST_H_

#include "Host.h"
#include "Event.h"


class Host
{
public:
    Host(int eventNum, double timeout, double rate);		//Host constructor
    ~Host();							//Host destructor
    double firstTime();						//Retrives the time of the first event
    Event* getFirst();						//Retrives the first event
    void backoffHandler(double time, double processTime);	//Handles the backoff times during a process
    void updateBackoff();					//Creates a new backoff time
    double nextRequest(double time);				//Returns time of the next scan
    void incrementBackoff(double time);				//Updates the backoff time during a process
    bool isEmpty();						//Checks if there are events remaining
    bool checkConflict(double time, double processTime);	//Checks if there is a transmission during the time of a process
    void conflictHandler(double time, double processTime);	//Handles the conflict
    void clearBackoff();					//Sets backoff to 0
    void updateTime(double time);				//updates the time used for incrementing backoff
    void remove(double time);					//Removes the first event in the list
    double getTotalTime();					//Returns the total queueing and transmission time of all events in the list

private:
    EventList *eventList;					//The event List
    double backoff;						//Current backoff time
    double queueingTime;					//Time between ACK recieved and request arrival time
    double lastTime;						//Stores last time to determine how much time passed to increment the backoff
    double T;							//Backoff generation variable
    int timeoutNum;						//Number of times waitedo n backoff
};

#endif
