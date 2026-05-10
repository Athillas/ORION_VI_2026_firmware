#ifndef VACUUM_STATE_H
#define VACUUM_STATE_H

#include <cstdint>

enum VacuumState : uint8_t
{
    IDLE        = 0,
    COLLECTING  = 1,
    DROPPING    = 2,
    CLEANING    = 3,
    ABORT       = 4,
    ERROR       = 5
};


/*
    The state transition matrix.
    
                IDLE    COLLECTING  DROPPING    CLEANING    ABORT       ERROR
    IDLE        1       1           0           1           1           1
    COLLECTING  0       1           1           0           1           1
    DROPPING    1       0           1           1           1           1
    CLEANING    1       0           0           1           1           1
    ABORT       1       0           0           0           1           1
    ERROR       1       0           0           0           0           1
*/

#endif