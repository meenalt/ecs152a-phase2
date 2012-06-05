#include <iostream>
#include <cmath>
#include <stdlib.h>
#include "Event.h"

#define BANDWIDTH 1441792.0


//Generates the time between packets
double timeGen(double rate)
{
  double u;
  u = drand48();
  return((-1/rate)*log(1-u));
}

//Generates the size of the packet
double sizeGen(double rate)
{
  rate = 8223;
  int size;
  size = timeGen(rate) * BANDWIDTH;
  if(size > 1544)
  { return 1544; }
  else
  {
    if(size < 0)
    { return 1; }
    else
    { return size; }
  }
}

//Returns next event
Event* Event::Next()
{  return next;  }

//Sets the next event
void Event::setNext(Event *e)
{
  next = e;
}

//Returns the time of the event
long double Event::getTime()
{  return time;  }

//Returns the size of the event
int Event::getSize()
{  return size;  }

//EventList constructor
EventList::EventList(int eventNum, double rate)
{
  double arrival = timeGen(rate);
  Event *event = new Event(arrival, sizeGen(rate));
  first = event;	//Sets first element in the list
  last = event;		//Sets last element which is the first element when size = 1
  event = NULL;

  //Event Generation
  for(int i = 1; i < eventNum; i++)
  {
    arrival += timeGen(rate);	//Arrival time generation
    event = new Event(arrival, sizeGen(rate));
    this->insert(event);
    event = NULL;
  }
}

//Insert event into the event list
void EventList::insert(Event *e)
{  
  last->setNext(e);
  last = e;
}

//Returns the first element
Event* EventList::getFirst()
{
  return first;
}

//Returns the time of the first Event
double EventList::firstTime()
{
  if(first != NULL)	//Checks if first event exists
  {  
    return( first->getTime() ); 
  } else { 
    return(-1); 
  }
}

//Returns whether the list is empty
bool EventList::isEmpty()
{
  if(first == NULL)
  {
    return true;
  } else {
    return false;
  }
}

//Checks whether the event needs to be processed
//If the event has occured yet
bool EventList::needRequest(double time)
{
  if(!this->isEmpty())
  {
    if( first->getTime() <= time )
    { return true; } else { return false; }
  } else { return false; }
}

//removes event from the list
void EventList::remove()
{
    Event *temp = first;
    first = first->Next();
    delete temp;
}
