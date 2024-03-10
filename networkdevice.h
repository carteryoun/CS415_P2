#ifndef _NETWORKDEVICE_H_
#define _NETWORKDEVICE_H_

/*
 * Author:    Peter Dickman
 * Revised by Joe Sventek
 * Version:   1.5
 * Last edit: 2003-02-25
 * Edited: 4 May 2015
 * Edited: 10 November 2017
 *
 * This file is a component of the test harness and or sample
 * solution to the NetworkDriver exercise (re)developed in May 2015
 * for use in assessing CIS 415 Spring 2015
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
#include "destination.h"
#include "pid.h"

typedef struct network_device NetworkDevice;

struct network_device {
    /* instance-specific data */
    void *self;

    /* Returns 1 if successful, 0 if unsuccessful */
    /* May take a substantial time to return      */
    /* If unsuccessful you can try again, but if you fail repeatedly give */
    /* up and just accept that this packet cannot be sent for some reason */
    int (*sendPacket)(NetworkDevice *nd, PacketDescriptor *pd);

    /* tell the network device to use the indicated PacketDescriptor     */
    /* for next incoming data packet; once a descriptor is used it won't */
    /* be reused for a further incoming data packet; you must register   */
    /* another PacketDescriptor before the next packet arrives           */
    void (*registerPD)(NetworkDevice *nd, PacketDescriptor *pd);

    /* The thread blocks until the registered PacketDescriptor has been   */
    /* filled with an incoming data packet. The PID field of the packet   */
    /* indicates the local application process which should be given it.  */
    /* This should be called as soon as possible after the previous       */
    /* call to registerPD() to wait for a packet */
    /* Only 1 thread may be waiting. */
    void (*awaitIncomingPacket)(NetworkDevice *nd);
};

#endif /* _NETWORKDEVICE_H_ */
