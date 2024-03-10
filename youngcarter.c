/* CS 415 Project 2
* Name: Carter Young; 
* Duck ID: cartery
* UO: 951690164

* This is my own work, except for ideas and conversations with Freddy Lopez
* and Sydney Whiting.
* Freddy Lopez and Ash (don't know last name) suggested the use of a 2D buffer which
* I had previously implemented but forgotten about
*/

#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include "BoundedBuffer.h"
#include "destination.h"
#include "diagnostics.h"
#include "fakeapplications.h"
#include "packetdescriptor.h"
#include "queue.h"
#include "pid.h"
#include "freepacketdescriptorstore__full.h"
#include "freepacketdescriptorstore.h"
#include "packetdriver.h"
#include "packetdescriptorcreator.h"
#include "networkdevice.h"
#include "networkdevice__full.h"

#define MAX_PID 10
#define MAX_BUFFER_SIZE 11

typedef struct thread_data {
  NetworkDevice *nd;
  FreePacketDescriptorStore *fpds;
  BoundedBuffer *bb;
} ThreadData; // struct to help set up threads

void *init_packet_thread(void *arg); // helper function for init
PID getPID(PacketDescriptor *pd);
BoundedBuffer *sendBuffer; // sendBuffer function to clean up write/read
BoundedBuffer *pidBuffers[MAX_PID]; // Main buffer for PIDs
BoundedBuffer *wipBuffer; // Buffer for WiP PDs
BoundedBuffer *receiveBuffer[MAX_PID+1]; // init buffer
FreePacketDescriptorStore *newfpds;

  
/* INIT_PACKET_DRIVER 
* Called before any other methods, to allow you to initialize
* data structures and start any internal threads.
* Arguments:
* nd: the NetworkDevice that you must drive,
* mem_start, mem_length: some memory for PacketDescriptors
* fpds: You hand back a FreePacketDescriptorStore into
* which PacketDescriptors built from the memory
* described in args 2 & 3 have been put */
void init_packet_driver(NetworkDevice *nd, void * mem_start,
unsigned long mem_length, FreePacketDescriptorStore **fpds) {
  // Check if args are valid
  if (nd == NULL || mem_start == NULL || fpds == NULL) {
    DIAGNOSTICS("Error: critical field(s) left NULL, cannot init, exiting.\n");
    exit(EXIT_FAILURE);
  }

  // Create FPDS using mem_start and mem_length
  FreePacketDescriptorStore *newfpds = FreePacketDescriptorStore_create(mem_start, mem_length);

  unsigned long newfpds_size = newfpds->size(newfpds);

  if(newfpds_size <= 0) {
    DIAGNOSTICS("Error: The FPDS is empty!\n");
    exit(EXIT_FAILURE);
  }

  // Create buffers required by thread
  BoundedBuffer *bb = BoundedBuffer_create(newfpds_size);
  if(bb == NULL) {
    DIAGNOSTICS("Error: No items in bounded buffer, exiting.\n");
    exit(EXIT_FAILURE);
  }

  // Initialize BoundedBuffer for sendBuffer
  sendBuffer = BoundedBuffer_create(MAX_BUFFER_SIZE);

  // Initialize BoundedBuffer for wipBuffer
  wipBuffer = BoundedBuffer_create(MAX_BUFFER_SIZE);
  
  // Initialize BoundedBuffer for each PID
  for(int i = 0; i < MAX_PID+1; i++) {
    receiveBuffer[i] = BoundedBuffer_create((int)newfpds_size/MAX_BUFFER_SIZE);
  }

  // Create instance for ThreadData on stack
  ThreadData td;
  td.nd = nd;
  td.fpds = *fpds;
  td.bb = bb;
  
  // Create as many threads as necessary
  pthread_t packet_thread;
  if(pthread_create(&packet_thread, NULL, init_packet_thread, &td) != 0) {
    DIAGNOSTICS("Error: Threads cannot be created, exiting.\n");
    exit(EXIT_FAILURE);
  }

  // Return FPDS
  *fpds = newfpds;
  
  pthread_detach(packet_thread);
}

/* INIT_PACKET_THREAD is the entry point for the thread created in the function above.
* The thread continuously gets free PDs from FPDS, registers them with ND, waits for
* more packets, then writes them to BB.
*
* The 'arg' parameter is expected to be a pointer to a 'ThreadData' struct which contains
* ND, FPDS, and BB instances that the thread will use
*
*/
void *init_packet_thread(void *arg) {
  // Are we in business?
  if(arg == NULL) {
    // If arg is NULL, set error code to -1, exit and set err_code to -1
    DIAGNOSTICS("Error: no args to init_packet_thread, exiting.\n");
    exit(EXIT_FAILURE);
  }

  // Cast void pointer to TD pointer
  ThreadData *td = (ThreadData *)arg;

  // Extract the ND, FPDS, and BB from the TD struct
  NetworkDevice *nd = td->nd;
  FreePacketDescriptorStore *fpds = td->fpds;
  BoundedBuffer *bb = td->bb;

  // Pointer declaration to a PD, used to hold instances from FPDS
  PacketDescriptor *pd = NULL;

  
  while(newfpds != NULL) {
    if(newfpds->nonblockingGet(fpds, &pd) && pd != NULL) {
  // Enter main loop, init PD, infinitely get free PDs, register them with ND, wait again, write to BB
      
      initPD(pd);

      // Get a free PD from the FPDS. Will block if there are no free PDs
      newfpds->blockingGet(fpds, &pd);

      // Register the PD with the ND. The ND will use the PD for the next batch of data packets
      nd->registerPD(nd, pd);

      // Wait for an incoming packet. Function call will block until a packet arrives and fills PD
      nd->awaitIncomingPacket(nd);

      // Write packet to the BB. Will block if BB is full
      bb->blockingWrite(bb, (void *)pd);

    /* 1. Packet has been written to BB
    *  2. Packet ready for further processing
    *  3. Add signal to notify the other thread a packet is available
    */
    }
  }
  return 0;
}

/* BLOCKING_SEND_PACKET
*  Function keeps trying to send packet until it succeeds. If it doesn't,
*  a diagnostic method is printed and the function tries again.
*  Note: Set limit to avoid infinite failure
*/
void blocking_send_packet(PacketDescriptor *pd) {
  initPD(pd);
  if(pd == NULL) {
    return;
  } else {
    sendBuffer->blockingWrite(sendBuffer, pd);
  }
}

int nonblocking_send_packet(PacketDescriptor *pd) {
  initPD(pd);
  if(pd == NULL) {
    return 0;
  } else {
  return sendBuffer->nonblockingWrite(sendBuffer, pd);
  }
}
/* These calls hand in a PacketDescriptor for dispatching
* The nonblocking call must return promptly, indicating whether or
* not the indicated packet has been accepted by your code
* (it might not be if your internal buffer is full) 1=OK, O=not OK
* The blocking call will usually return promptly, but there may be
* a delay while it waits for space in your buffers.
* Neither call should delay until the packet is actually sent!! */

/*
* BLOCKING_GET_PACKET
* only returns when a packet has been received for the indicated
* process, first arg points at it.
* Incorporate bounded buffer to catch packets that are WiP
*/
void blocking_get_packet(PacketDescriptor **pd, PID pid) {
  initPD(*pd);
  if(pd == NULL || pid > MAX_PID) {
    printf("Error: invalid args to blocking_get_packet");
    return;
  }
  
  // First, check WiP buffer
  if(!wipBuffer->nonblockingRead(wipBuffer, (void **)pd)) {
  // If no packet is found, blockRead
    BoundedBuffer *bb = receiveBuffer[pid];
    bb->blockingRead(bb, (void **)pd);
  }
}

/*
* NONBLOCKING_GET_PACKET
* must return promptly, with 1 if packet was found
* 0 if no packet was waiting.
* Incorporate bounded buffer to catch packets that are WiP
*/
int nonblocking_get_packet(PacketDescriptor **pd, PID pid) {
  initPD(*pd);
  if(pd == NULL || pid > MAX_PID) {
    printf("Error: Invalid args to nonblocking_get_packet\n");
    return 0; // Error
  }
  
  // First, check the wipBuffer
  if(wipBuffer->nonblockingRead(wipBuffer, (void **)pd)) {
    return 1; // Found a packet!
  }
  
  // If no packet in wip, check the other BB
  BoundedBuffer *bb = receiveBuffer[pid];
  if(bb->nonblockingRead(bb, (void **)pd)) {
    return 1; // Found a packet in the PID buffer
  }

  return 0; // If not packets, return 0  
}

