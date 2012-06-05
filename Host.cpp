#include <iostream>
#include "Host.h"
#include <stdlib.h>

//Host constrcutor
Host::Host(int eventNum, double timeout, double rate)
{
  //Eventlist Generation which represents the list of requests
  eventList = new EventList(eventNum, rate);
  T = timeout;
  timeoutNum = 0;
  backoff = 0;
  queueingTime = 0;
}

//Returns the time of the first node
double Host::firstTime()
{
  return eventList->firstTime();
}

//Removoes the next event in the event queue
Event* Host::getFirst()
{
  Event *RETevent;
  RETevent = eventList->getFirst();
  return RETevent;
  
}

//Handles the backoffs
void Host::backoffHandler(double time, double processTime)
{
  //Checks if we need to create a backoff time
  //such by determining whether an event has occured yet
  if( eventList->needRequest( time ) )
  {
    if(backoff == 0 )	
    {
      this->updateBackoff();		//No backoff time so all we do is generate one
    } else {
      while(backoff < processTime)	//Keep going until backoff time is longer than the process time
      {
        processTime -= backoff;		//Remaining process time
        this->updateBackoff();		//New backoff time
      } 
      backoff -= processTime;		//update the backoff w/ remaining process time
    }
  }
}

//Generates a new backoff Time
void Host::updateBackoff()
{
  timeoutNum++;
  double u = drand48();
  backoff = (timeoutNum * T) * u;
}

//Determines the time of the next request
double Host::nextRequest(double time)
{ 
  if(this->firstTime() <= time)		//We actually need the request
  {
    if(backoff > 0)			//if there is a backoff time
    {
      return (time + backoff);		//Time of next scan
    } else {
      return time;			//No backoff time thus return current time for request
    }
  } else {
    return this->firstTime();		//No event waiting to occur, return next event time
  }
}

//Increments the backoff during a process
void Host::incrementBackoff(double time)
{
  if((backoff >= 0) && (eventList->needRequest(time)))	//function only needed if there is a backoff
  {							//and the event need to occur
    if(backoff != 0)
    {
      backoff -= (time - lastTime);			//Backoff time
    } else {
      this->updateBackoff();				//No backoff time means we need to generate one
    }

/*Error checking to ensure backoff is always greater than 0
***********************************************************/
    if(backoff < 0)
      cout << "ERROR backoff less than zero" << endl;
/*********************************************************/
  }
}

//Checks if the event list is empty
bool Host::isEmpty()
{ return eventList->isEmpty(); }

//Checks if the next event will cause a conflict with what is being processed
bool Host::checkConflict(double time, double processTime)
{
  if(((time > eventList->firstTime()) && ( backoff < processTime)) || ((eventList->firstTime() - time  <  processTime) && ( eventList->firstTime() - time > 0)) )
  { return true; } else { return false; }
}

//Handles the conflict if one exists
void Host::conflictHandler(double time, double processTime)
{
  this->backoffHandler(time, processTime);
}

//Clears the backoff
void Host::clearBackoff()
{  backoff = 0; }

//updates the lastTime used for backoff incrementation
void Host::updateTime( double time )
{
  lastTime = time;
}

//Removes an event from the event list
void Host::remove(double time)
{
  queueingTime += time - this->firstTime();
  timeoutNum = 0;
  if( !eventList->isEmpty() )
    eventList->remove();
}

//retrives the time used for average network delay
double Host::getTotalTime()
{  return (queueingTime); }
