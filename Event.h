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

//A request that has arrived in to the host
class Event {
public:
    Event(long double t, int s) :		//Event constructor
    time( t ), size( s ), next( NULL ) {};
    ~Event() {};				//Event destructor
    Event* Next();				//returns the next node
    void setNext(Event*);			//Sets the pointer to next node
    long double getTime();			//gets the event time
    int getSize();				//Gets the size of the frame
    
private:
    Event *next;				//pointer to next event in the list
    long double time;				//arrival time of the request
    int size;					//Size of frame
};


//Stores arrivals in a linked list in order
class EventList {
public:
    EventList(int eventNum, double rate);	//Constrcutor
    ~EventList();				//Destructor
    void insert(Event *e);			//Insert Event
    Event* getFirst();				//Retrieves the first event
    double firstTime();				//Retrieves the time of the first event
    bool isEmpty();				//Returns if the eventList is empty
    bool needRequest(double time);		//Checks if the event has occurred with respect to the current time
    void remove();				//Removes an event
private:
    Event *first;				//front of event list
    Event *last;				//last element in the event list
};


#endif
