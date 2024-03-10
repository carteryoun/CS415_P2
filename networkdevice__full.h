#ifndef _NETWORKDEVICE__FULL_H_
#define _NETWORKDEVICE__FULL_H_

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
 * for use in assessing CIS 415 Spring 2015
 *
 * It tests the ability to develop a small but complex software system
 * using PThreads to provide concurrency in C.
 *
 * Copyright:
 * (c) 2003 University of Glasgow and Dr Peter Dickman
 * (c) 2015, 2017 University of Oregon and Dr Joe Sventek
 *
 * this software used with the author's permission.
 *
 */


#include "networkdevice.h"

NetworkDevice *NetworkDevice_create(void);

#endif /* _NETWORKDEVICE__FULL_H_ */
