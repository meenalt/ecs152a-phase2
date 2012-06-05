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
unsigned int transmitted;	//Number of bits transmitted
int susCount;	//Sucessful transfer
int failCount;	//Collision Counter

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
  //Seed
  srand48(time(NULL));

  //Arguement loading
  double rate = atof(argv[1]);
  eventNum = atoi(argv[2]);
  hostNum = atoi(argv[3]);

  //Initializes variables
  active = hostNum;
  hosts = new Host*[hostNum];
  Event *event, *eventNew;
  transmitted = 0;	//Set number of transmitted bytes to 0

  //Host generation including event generation
  for(int i = 0; i < hostNum; i++)
  {
    hosts[i] = new Host(eventNum, atof(argv[4]), rate);
  }

  //Calculates time for acks
  ACKTIME = (64*8)/BANDWIDTH;
}

//Begins the simulation of the network
void Simulate() {
  //Variable Declarations
  int i;
  double min;
  int minHost = 0;
  bool firstEvent;

/*****************************
  Simulation begin
*****************************/

  //First event process
  current_time = 0;
  min = hosts[0]->firstTime();
  for(i = 1; i < hostNum; i++)
  {
    if((hosts[i]->firstTime() < min))
    {
      min = hosts[i]->firstTime();
      minHost = i;
    }
  }
  current_time = min;
  Process(minHost);

  //Sucessive event process
  while(active > 0)	
  //Active denotes how many hosts still have events in need of processing
  {
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
  }//while
/**************************************
	Simulation End
**************************************/
}

//Calculates the statistics used for the output
void Stats()
{
  double throughput = transmitted / current_time;
  cout << "Throughput: " << throughput   << endl;
  double totalTime = 0;
  for(int i = 0; i < hostNum; i++)
  {
    totalTime += hosts[i]->getTotalTime();
  }
  cout << "Total time: "  << totalTime << endl;
  cout << "Average Network Delay: " << totalTime / throughput  << endl;
}

//Handles the event deemed to occur
void Process(int processHost)
{
  //Variable initializations
  int i;
  bool globalConflict = false;	//Anytime this is true means there was a conflict
  bool conflict[hostNum];	//keeps track of hosts that cause conflicts
  Event *processEvent = hosts[processHost]->getFirst();
  double processTime;

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

//DIFS handler if there was no conflict
  if( !globalConflict )  //not 2 things transmitted at same time
  {
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

//Packet transmission if there was no conflict during DIFS state
  if( !globalConflict )
  {
    processTime = (processEvent->getSize() * 8 ) / BANDWIDTH;
    transmitted += processEvent->getSize();
    current_time += processTime;
    for(i = 0; i < hostNum; i++)
    {
      if( (i != processHost ) && (!hosts[i]->isEmpty()))
      {
        hosts[i]->backoffHandler(current_time, processTime);
      }
    }
  }

//SIFS conflict detection
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

//SIFS handler if no collision
    if( !globalConflict )
    {
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
    hosts[processHost]->remove(current_time);
    if(hosts[processHost]->isEmpty())
      active--;
    susCount++;
 //   cout << "suscess   " <<  susCount << endl;
  }
}

