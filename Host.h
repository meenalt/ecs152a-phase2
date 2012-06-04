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
    Host(int eventNum, double timeout, double rate);
    ~Host();
    void insertEvent(Event *e);   
    int count();
    double firstTime( double time);
    Event* getFirst();
    void backoffHandler(double time, double processTime);
    void updateBackoff();
    double nextRequest(double time);
    void incrementBackoff(double time);
    bool isEmpty();
    void reInsert(Event *e);
    bool checkConflict(double time, double processTime);
    void conflictHandler(double time, double processTime);
    void clearBackoff();
    void updateTime(double time);
    void remove();
    void transmissionTime(double time);
    double getTotalTime();

private:
    EventList *eventList;
    double backoff;
    double prevBackoff;
    double queueingTime;
    double lastTime;
    bool empty;
    double T;
    int timeoutNum;
    bool needRequest;
    double transmission;
};

#endif
