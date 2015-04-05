// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

/// @file	AP_MotorsHexa.h
/// @brief	Motor control class for Hexacopters

#ifndef __AP_MOTORS_HEXA_H__
#define __AP_MOTORS_HEXA_H__

#include <AP_Common.h>
#include <AP_Math.h>        // ArduPilot Mega Vector/Matrix math Library
#include <RC_Channel.h>     // RC Channel Library
#include "AP_MotorsMatrix.h"    // Parent Motors Matrix library

/// @class      AP_MotorsHexa
class AP_MotorsHexa : public AP_MotorsMatrix {
public:

    /// Constructor
    AP_MotorsHexa(RC_Channel& rc_roll, RC_Channel& rc_pitch, RC_Channel& rc_throttle, RC_Channel& rc_yaw, AP_Actuator_Channel* rc_out[], uint16_t loop_rate, uint16_t speed_hz = AP_MOTORS_SPEED_DEFAULT) : AP_MotorsMatrix(rc_roll, rc_pitch, rc_throttle, rc_yaw, rc_out, loop_rate, speed_hz) {
    };

    // setup_motors - configures the motors for a hexa
    virtual void        setup_motors();

protected:

};

#endif  // AP_MOTORSHEXA
