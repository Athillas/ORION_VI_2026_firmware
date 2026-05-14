// VacuumConfig.h
#ifndef VACUUM_CONFIG_H
#define VACUUM_CONFIG_H

#include <cstdint>

namespace VacuumConfig
{
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
    enum VacuumState : uint8_t
    {
        IDLE        = 0,
        COLLECTING  = 1,
        DROPPING    = 2,
        CLEANING    = 3,
        ABORT       = 4,
        ERROR       = 5
    };
    
    enum VacuumCommand : uint8_t
    {
        TO_IDLE             = 0,
        TO_COLLECTING       = 1,
        TO_DROPPING         = 2,
        TO_CLEANING         = 3,
        TO_ABORT            = 4,
        CLEAR_ERRORS        = 5
    };

    constexpr uint8_t TRANSITION_MASK[6] = {
        0b101111,  // IDLE: can go to states 0,1,3,4,5
        0b011011,  // COLLECTING: can go to 1,2,4,5
        0b101110,  // DROPPING: can go to 0,2,3,4,5
        0b101100,  // CLEANING: can go to 0,3,4,5
        0b101100,  // ABORT: can go to 0,4,5
        0b100000   // ERROR: can only go to 5
    };

    constexpr bool canTransition(VacuumState current, VacuumCommand target)
    {
        return (TRANSITION_MASK[current] >> target) & 1;
    }
}


#endif