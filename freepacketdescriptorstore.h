#ifndef _FREEPACKETDESCRIPTORSTORE_H_
#define _FREEPACKETDESCRIPTORSTORE_H_

/*
 * Author:    Peter Dickman
 * Revised by Joe Sventek
 * Version:   1.4
 * Last edit: 2003-02-18
 * Edited: 4 May 2015
 * Edited: 10 November 2017
 *
 * This file is a component of the test harness and or sample
 * solution to the NetworkDriver exercise (re)developed in May 2015
 * for use in assessing CIS 415 Spring Project 2
 *
 * It tests the ability to develop a small but complex software system
 * using PThreads to provide concurrency in C.
 *
 * Copyright:
 * (c) 2003 University of Glasgow and Dr Peter Dickman
 * (c) 2015,2017 University of Oregon and Dr Joe Sventek
 *
 * this software used with the author's permission.
 *
 */

#include "packetdescriptor.h"

/* As usual, the blocking versions only return when they succeed. */
/* The nonblocking versions return 1 if they worked, 0 otherwise. */
/* The Get methods set their final arg if they succeed.           */

typedef struct free_packet_descriptor_store FreePacketDescriptorStore;
struct free_packet_descriptor_store {
    void *self;
    void (*blockingGet)(FreePacketDescriptorStore *fpds, PacketDescriptor **pd);
    int (*nonblockingGet)(FreePacketDescriptorStore *fpds, PacketDescriptor **pd);
    void (*blockingPut)(FreePacketDescriptorStore *fpds, PacketDescriptor *pd);
    int (*nonblockingPut)(FreePacketDescriptorStore *fpds, PacketDescriptor *pd);
    unsigned long (*size)(FreePacketDescriptorStore *fpds);
};

#endif /* _FREEPACKETDESCRIPTORSTORE_H_ */
