//
//  PacketList.h
//  
//
//  Created by Meenal Tambe on 5/28/12.
//  Copyright (c) 2012 University of California, Davis. All rights reserved.
//

#ifndef _EVENT_H_
#define _EVENT_H_

#include <iostream>

using namespace std;

class Event {
public:
    Event(long double t, int s) :
    time( t ), size( s ), next( NULL ) {};
    ~Event() {};
    Event* Next();	//Next Node
    void setNext(Event*);	//Sets the next ptr
    long double getTime();		//gets the event time
    int getSize();	//Gets the service time for the packet
    
private:
    Event *next;
    long double time;		//Arrival time
    int size;	//Size of frame
};


//Stores arrivals in a linked list in order
class EventList {
public:
    EventList(int eventNum, double rate);
    ~EventList();
    void insert(Event *e);
    Event* getFirst();	//Removes event, returns 1st event
    int count();
    double firstTime( double time);
    bool isEmpty();
    void reInsert(Event *e);
    bool needRequest(double time);
    void remove();
private:
    Event *first;		//front of event list
    Event *last;
    bool empty;
};


#endif
