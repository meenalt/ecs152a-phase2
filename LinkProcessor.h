//
//  Header.h
//  
//
//  Created by Sanjay Tambe on 5/28/12.
//  Copyright (c) 2012 University of California, Davis. All rights reserved.
//

#ifndef _LINKPROCESSOR_H_
#define _LINKPROCESSOR_H_

#include "PacketList.h"

class Packet
{
public:
    Packet(double t);
    ~Packet() {};
    Packet* getNext();				//Get next ptr
    Packet* Run(double t);			//Check for packet desparture, returns ptr if Packet is finished
    double getTime();				//Get remaining process time
    void setNext( Packet* );			//Set next pointer
    
private:
    double time;				//remaining time
    Packet *next;
};

class LinkProcessor
{
public:
    LinkProcessor(int size);
    ~LinkProcessor();
    void Process( double, Packet_Node* );	//Check for arrivals/ departures
    void Arrive(Packet_Node*);			//Process arrivals
    void Departure( double );				//Process Departures
    void stats();				//returns basic statistics
    
private:
    int length;					//Buffer current length
    int maxBuffer;				//Buffer size
    Packet *first;				//Front of queue
    Packet *last;				//Back of queue
    int PacketLoss;				//Packets lost due to overflow
    double prev_time;				//Last event time value 
    double utilization_time;
    double queue_time;				//keeps track of queue legnth time
    double total_time;
};

#endif
