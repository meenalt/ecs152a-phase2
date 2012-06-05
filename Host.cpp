#include <iostream>
#include "Host.h"
#include <stdlib.h>

Host::Host(int eventNum, double timeout, double rate)
{
  eventList = new EventList(eventNum, rate);
  T = timeout;
  timeoutNum = 0;
  backoff = 0;
  queueingTime = 0;
  empty = false;
  transmission = 0;
}

void Host::insertEvent(Event *e)
{
  eventList->insert(e);
}

//Returns the number of events in the list, used for debuging
int Host::count()
{
  return (eventList->count() );
}

//Returns the time of the first node
double Host::firstTime( double time )
{
  return eventList->firstTime(time);
}

//Removoes the next event in the event queue
Event* Host::getFirst()
{
  timeoutNum = 0;
  Event *RVevent;
  RVevent = eventList->getFirst();
  return RVevent;
  
}

//Handles the backoffs
void Host::backoffHandler(double time, double processTime)
{
double a = backoff;
if(a < 0 )
  cout << "BIG ERROR\n";
  needRequest = eventList->needRequest(time);
  if( needRequest == true )
  {
    if(backoff > processTime)
    {
      while(backoff < processTime)//Keep going until backoff time is longer than the process time
      {
        processTime -= backoff;	//Remaining process time
        this->updateBackoff();	//New backoff time
      } 
    } else {
      if(backoff == 0 )
      {
        this->updateBackoff();
      } else {
        while(backoff < processTime)//Keep going until backoff time is longer than the process time
        {
          processTime -= backoff;	//Remaining process time
          this->updateBackoff();	//New backoff time
        } 
        backoff -= processTime;	//update the backoff since processTime will be negative
      }
    }
//if(backoff < 0 )
//  cout << "error\n";
  }
}

void Host::updateBackoff()
{
  timeoutNum++;
  double u = drand48();
  backoff = (timeoutNum * T) * u;
  //queueingTime = backoff;
}

//Determines the time of the next request
double Host::nextRequest(double time)
{ 
//cout << time+backoff << "    ||     " << this->firstTime(time) << endl; 
  if(this->firstTime( time ) <= time)
  {
    if(backoff > 0)
    {
      return (time + backoff);
    } else {
      return time;
    }
  } else {
    return this->firstTime( time );
  }
}

void Host::incrementBackoff(double time)
{
double a = backoff;
  if((backoff >= 0) && (eventList->needRequest( time)))
  {
    if(backoff != 0)
    {
      backoff -= (time - lastTime);
    } else {
      this->updateBackoff();
    }
    if(backoff < 0)
      cout << "ERROR backoff less than zero" << endl;
  }
}

bool Host::isEmpty()
{ return empty; }

//Used if theres a collision
void Host::reInsert(Event *e)
{
  eventList->reInsert(e);
  empty = false;
}

bool Host::checkConflict(double time, double processTime)
{
  if(((time > eventList->firstTime(time)) && ( backoff < processTime)) || ((eventList->firstTime( time ) - time  <  processTime) && ( eventList->firstTime(time) - time > 0)) )
  { return true; } else { return false; }
}

void Host::conflictHandler(double time, double processTime)
{
/*  cout << "Backoff: " << backoff << endl;
  cout << "FirstTime: " << this->firstTime(time) << endl;
  cout << "Time: " << time << endl;
  cout << "ProcessTime: " << processTime << endl;
*/

  this->backoffHandler(time, processTime);
  //cout << "New backoff: " << backoff << endl;
}

void Host::clearBackoff()
{  backoff = 0; }

void Host::updateTime( double time )
{
  lastTime = time;
}

void Host::remove(double time)
{
  queueingTime += time - this->firstTime(time);
//cout << time - this->firstTime(time) << endl;;
  timeoutNum = 0;
if(queueingTime < 0 )
  cout << time << "     " << this->firstTime(time) << endl;
  if( !eventList->isEmpty() )
  {
    eventList->remove();
    empty = eventList->isEmpty();
  } else {empty = true;}
}


void Host::transmissionTime(double time)
{  transmission += time; }

double Host::getTotalTime()
{  return (queueingTime); }
