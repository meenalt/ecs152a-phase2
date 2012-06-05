#include <iostream>
#include <cmath>
#include <stdlib.h>
#include "Event.h"

#define BANDWIDTH 1441792.0

double timeGen(double rate)
{
  double u;
  u = drand48();
  return((-1/rate)*log(1-u));
}

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

Event* Event::Next()
{  return next;  }

void Event::setNext(Event *e)
{
  next = e;
}

long double Event::getTime()
{  return time;  }

int Event::getSize()
{  return size;  }

EventList::EventList(int eventNum, double rate)
{
 // srand48( time(NULL));
 // srand(time(NULL));
  double arrival = timeGen(rate);
  Event *event = new Event(arrival, sizeGen(rate));
  first = event;
  last = event;
  event = NULL;
  for(int i = 1; i < eventNum; i++)
  {
    arrival += timeGen(rate);
//cout << arrival << endl;
    event = new Event(arrival, sizeGen(rate));
    this->insert(event);
    event = NULL;
  }
  empty = false;
}

void EventList::insert(Event *e)
{  
  last->setNext(e);
  last = e;
}

Event* EventList::getFirst()
{
  return first;
}

int EventList::count()
{
  int count = 1;
  Event *event = first;
  while(event != NULL)
  {
    count++;
    event = event->Next();
  }
  return count;
}

double EventList::firstTime( double time)
{
  if(first != NULL)
  {  return( first->getTime() ); } else { return(-1); }
}

bool EventList::isEmpty()
{ return empty; }

void EventList::reInsert(Event *e)
{
  e->setNext(first);
  first = e;
if(first == NULL)
  cout << "first null\n";
  empty = false;
if(first == first->Next())
  cout <<"BIIIIIIGGGG PROBLEMNO\n";
}

bool EventList::needRequest(double time)
{
  if(!this->isEmpty())
  {
    if( first->getTime() <= time )
    { return true; } else { return false; }
  } else { return false; }
}

void EventList::remove()
{
    Event *temp = first;
    first = first->Next();
if ((first != NULL)&&(temp->getTime() > first->getTime()))
  cout << "Shits gonna happen\n";
    if( first == NULL )
      empty = true;
    delete temp;
}
