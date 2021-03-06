/*
 * Timer.h
 *
 *  Created on: Jan 2, 2012
 *      Author: William Boyd
 *				MIT, Course 22
 *              wboyd@mit.edu
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <time.h>
#include <sys/time.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <utility>
#include <vector>
#include "log.h"

#ifdef __MACH__		/* For OSX */
#define timespec timeval
#endif


class Timer {
protected:
    timespec start_time, end_time;
    double elapsed_time;
    bool running;
    std::vector< std::pair<double, const char*> > _timer_splits;
public:
    Timer();
    virtual ~Timer();
    void start();
    void stop();
    void restart();
    void reset();
    void recordSplit(const char* msg);
    double getTime();
    double diff(timespec start, timespec end);
    void printSplits();
};

#endif /* TIMER_H_ */
