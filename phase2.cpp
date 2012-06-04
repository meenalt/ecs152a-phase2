//Jason Wong

#include <iostream>
#include <stdlib.h>
#include <cmath>
#include "Host.h"
#include <iomanip>

#define BANDWIDTH 11534336.0  		//11 * 2^20
#define DIFS 0.0001
#define SIFS 0.00005
#define ACKSIZE 64

//Forward Declarations
void Initialize(char**);
void Simulate();	//Simulates the network
void Stats();
void Process(int host);
double timeGen(double rate);

//Global variables
Host **hosts;
int eventNum;	//Number of events
int hostNum;	//Number of Hosts
double current_time;	//Current time
double ACKTIME;		//time to send an ACK
int active;	//active number of hosts still transmitting
int num;
unsigned int transmitted;	//Number of bits transmitted
int susCount;
int failCount;

int main(int argc, char *argv[]) {
//rate, # of events, # of hosts, T
susCount = failCount = 0;
  Initialize(argv);
  Simulate();
  Stats();
  return 0;
}
//Initilizes all the variable and generates all of the events
void Initialize(char** argv) {
  transmitted = 0;
  num = 0;
  srand48(time(NULL));
  srand(time(NULL)); // Seeds
  double rate = atof(argv[1]);
  eventNum = atoi(argv[2]);
  hostNum = atoi(argv[3]);
  active = hostNum;
  int i;
  bool firstHost[hostNum];
  hosts = new Host*[hostNum];
  Event *event, *eventNew;
  int hostSelector;
  double arrival = 0;
  for(i = 0; i < hostNum; i++)//Used for implementation purposes
  {
    hosts[i] = new Host(eventNum, atof(argv[4]), rate);
  }
  ACKTIME = (64*8)/BANDWIDTH;
}

void Simulate() {
  current_time = 0;
  int i;
  double min;
  int minHost = 0;
  min = hosts[0]->firstTime( current_time);
  bool firstEvent;
  int count = 1;
  for(i = 1; i < hostNum; i++)
  {
    if((hosts[i]->firstTime(current_time) < min) && (hosts[i]->firstTime( current_time ) >= 0))
    {
      min = hosts[i]->firstTime(current_time);
      minHost = i;
      current_time = min;
    }
  }
  Process(minHost);
  while(active > 0)
  {

//DEBUG
   // cout << count << endl;
    count++;
//DEBUG
//if( count > 10000)
//  cout << "infinite" << endl;

    firstEvent = true;
    for(i = 0; i < hostNum; i++)
    {
      hosts[i]->updateTime(current_time);
      if((!hosts[i]->isEmpty()) && (((min >= hosts[i]->nextRequest(current_time)) && (min >= 0)) || (firstEvent == true)))
      {
        min = hosts[i]->nextRequest(current_time);
        minHost = i;
        firstEvent = false;
      }
    }
    current_time = min;
    for(i = 0; i < hostNum;i++)
    {
      if( (!hosts[i]->isEmpty() ) && (i != minHost))
        hosts[i]->incrementBackoff(current_time);
    }
    hosts[minHost]->clearBackoff();
    Process(minHost);
  }
}
void Stats()
{
  cout << transmitted / current_time   << endl;
  cout << "Sus:  "  << susCount << "   FAIL:   " << failCount << endl;
  double totalTime = 0;
  for(int i = 0; i < hostNum; i++)
  {
    totalTime += hosts[i]->getTotalTime();
  }
  cout << "Average Network Delay: " << totalTime / hostNum << endl;
}

void Process(int processHost)
{
  int i;
  bool globalConflict = false;
  bool conflict[hostNum];
  Event *processEvent = hosts[processHost]->getFirst();
  double processTime;
if(processEvent == NULL)
  cout << "processprob\n";

//Check for simultaneous DIFS collisions
  for(i =0; i < hostNum; i++)
  {
    if((i != processHost) && (!hosts[i]->isEmpty()))
    {
      conflict[i] = hosts[i]->checkConflict(current_time, DIFS);//Check for simultaneous transmisttion
      if(conflict[i] == true )
        globalConflict = true;
    }
  }
  current_time += DIFS;
  if( !globalConflict )  //not 2 things transmitted at same time
  {
    hosts[processHost]->transmissionTime(DIFS);
    for(i = 0; i < hostNum; i++)
    {
      if( (i != processHost ) && (!hosts[i]->isEmpty()))
        hosts[i]->backoffHandler(current_time, DIFS);
    }
  } else {
    for(i =0; i < hostNum; i++)
    {
      if(conflict[i])
      {
        hosts[i]->conflictHandler(current_time, DIFS);
      }
    }
    hosts[processHost]->updateBackoff();
  }

//No DIFS collisions thus handling packet
  if( !globalConflict )
  {
    processTime = (processEvent->getSize() * 8 ) / BANDWIDTH;
    transmitted += processEvent->getSize();
    current_time += processTime;
    hosts[processHost]->transmissionTime(processTime);
    for(i = 0; i < hostNum; i++)
    {
      if( (i != processHost ) && (!hosts[i]->isEmpty()))
      {
        hosts[i]->backoffHandler(current_time, processTime);
      }
    }
  }

//SIFS period
  if( !globalConflict )
  {
    for(i =0; i < hostNum; i++)
    {
      if((i != processHost) && (!hosts[i]->isEmpty()))
      {
        conflict[i] = hosts[i]->checkConflict(current_time, SIFS);//Check for simultaneous transmisttion
        if(conflict[i] == true )
          globalConflict = true;
      }
    }
    current_time += SIFS;
    if( !globalConflict )  //2 things transmitted at same time
    {
      hosts[processHost]->transmissionTime(SIFS);
      for(i = 0; i < hostNum; i++)
      {
        if( (i != processHost ) && (!hosts[i]->isEmpty()))
          hosts[i]->backoffHandler(current_time, SIFS);
      }
    } else {
      for(i =0; i < hostNum; i++)
      {
        if(conflict[i])
        {
          hosts[i]->conflictHandler(current_time, SIFS);
        }
      }
      hosts[processHost]->updateBackoff();
    }

//No SIFS collision so ACK handling
    if(!globalConflict)
    {
      hosts[processHost]->transmissionTime(ACKTIME);
      current_time += ACKSIZE / BANDWIDTH;
      transmitted += ACKSIZE;
      for(i = 0; i < hostNum; i++)
      {
        if( (i != processHost ) && (!hosts[i]->isEmpty()))
        {
          hosts[i]->backoffHandler(current_time, ACKTIME);
        }
      }
    } else {
    }
  }
  if(globalConflict)
  {
    failCount++;
 //   cout << " fail    " << failCount << endl;; 
  } else { 
    hosts[processHost]->remove();
    if(hosts[processHost]->isEmpty())
      active--;
    susCount++;
 //   cout << "sucess   " <<  susCount << endl;
  }
}

/*
double timeGen(double rate)
{
  double u;
  u = drand48();
  return((-1/rate)*log(1-u));
}*/
