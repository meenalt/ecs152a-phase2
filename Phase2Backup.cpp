//
//  Phase2.cpp
//  
//  Jason Wrote most of it!
//  Created by Sanjay Tambe on 5/28/12.
//  Copyright (c) 2012 University of California, Davis. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include "PacketList.h"
#include "LinkProcessor.h"

//Forward Declarations
void Initialize(char**);
void Simulate(int);
void Stats();
bool CheckCollision();
void ProcessHosts(int h);
void WaitForNext(int h);
void DIFS2(int h);
void Transmitting(int h);
void WaitForACK(int h);
void SmallBackoff(int h);
void BigBackoff(int h);

//Global variables
int totalbytes = 0;
int state[20];
int DIFS[20];
double FrameLength[20];
int TransTime[20];
int Backoff[20];
int Trans[20];
int N;
int T;
double lambda;
long int size;
int channelavailable = 1;
int flag = 0;
double throughput;


int main(int argc, char *argv[]) {
    // input = size, T, lambda, N
    Initialize(argv);
    size = atoi(argv[1]);
    T = atoi(argv[2]);
    lambda = atof(argv[3]);
    N = atoi(argv[4]);
    for (int i = 0; i < size; i++) {
        for (int h = 0; h < N; h++) {
            ProcessHosts(h);
            //printf("In Current State N: %d", N);
        }
        flag = CheckCollision();

    }
    throughput = totalbytes/(size * .01);
    Stats();
    return 0;
}

void ProcessHosts(int h) {
    if(state[h] == 0) WaitForNext(h); 
    if(state[h] == 1) DIFS2(h);
    if(state[h] == 2) Transmitting(h);
    if(state[h] == 3) WaitForACK(h);
    if(state[h] == 4) SmallBackoff(h);
    if(state[h] == 5) BigBackoff(h);
    
    
}

void WaitForNext(int h) {
    //printf("Current State WaitForNext = %d\n", state[h]);
    if (channelavailable == 1) {
        DIFS[h] = 10;
        double u;
        u = drand48();

        double y = (1544*u);
        double x = .07254; //this is just 112/1544
        FrameLength[h] = y;

        //FrameLength[h] = ((-1/lambda)*log(1-u));
        //double y = FrameLength[h] * x;
        double z = (y*x);
        TransTime[h] = z;
        //printf("TransTime = %d\n", TransTime[h]);
        Trans[h] = 0;
        state[h] = 1;
        x = (u * T * N);
        Backoff[h] = x;
        state[h] = 5;	//Big backoff
        
    }
    
    if(channelavailable == 0) {
        double u;
        int x;
        u = drand48();
        x = (u * T * N);
        Backoff[h] = x;
        state[h] = 5;	//Big Backoff
        //channel is not available. doing the large backoff..
    }
    
}

void DIFS2(int h) { 
    //printf("Current State DIFS2 = %d\n", DIFS[h]);
    DIFS[h] = DIFS[h] - 1;
    if(DIFS[h] == 1){
        channelavailable = 0; // initial DIFS complete. channel has been claimed.
        state[h] = 2;	//Transmitting
       // printf("Now in state[2] for HOST %d\n", h);
    }
}

void Transmitting(int h) {
    //printf("Current State Transmitting = %d\n", state[h]);
    Trans[h] = Trans[h] + 1;
    if(TransTime[h] == Trans[h]) state[h] = 3; //transmission complete. will wait for ACK.
}

void WaitForACK(int h) {
  //  printf("Current State WaitForAck = %d\n", state[h]);
    if(flag == 1) {
        double u;
        int x;
        u = drand48();
        x = (u * T);
        Backoff[h] = x;
        state[h] = 4;	//Small Backoff
    }
    else {
        //successful transmission
        totalbytes = totalbytes  + FrameLength[h];
        state[h] = 0;	//Wait for Next
        channelavailable = 1;
    }
}

void SmallBackoff(int h) {
    //printf("Current State SmallBackoff = %d\n", state[h]);
    Backoff[h] = Backoff[h] - 1;
    if(Backoff[h] == 1) {
      state[h] = 0;	//Wait for Next
    }
}

void BigBackoff(int h) {
    //printf("Current State BigBackoff = %d\n", state[h]);
    Backoff[h] = Backoff[h] - 1;
    if(Backoff[h] == 1) {
        state[h] = 1;	//DIFS2
        DIFS[h] = 10; 
    }
    
}
bool CheckCollision() {
   // printf("Checking collisions\n");
    int k = 0;
    for(int i = 0; i < N; i++) {
        if(state[i] == 2) k = k+1;	//Transmitting
    }
    if( k > 1) return 1;
    else return 0;
}

void Initialize(char** argv) {
   // printf("initializing\n");
    channelavailable = 1;
    for (int i = 0; i < N; i++){
       state[i] = 0;	//Waiting
    }
}

void Stats()
{
    //printf("Total bytes = %d, Throughput = %d \n", totalbytes, throughput);
    cout << "Total bytes = " << totalbytes << " Throughput = " << throughput << endl;
}
