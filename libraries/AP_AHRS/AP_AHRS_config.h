#pragma once

#include <AP_HAL/AP_HAL_Boards.h>
#include <AP_InertialSensor/AP_InertialSensor_config.h>

#ifndef AP_AHRS_ENABLED
#define AP_AHRS_ENABLED 1
#endif

#ifndef AP_AHRS_BACKEND_DEFAULT_ENABLED
#define AP_AHRS_BACKEND_DEFAULT_ENABLED AP_AHRS_ENABLED
#endif

#ifndef AP_AHRS_DCM_ENABLED
#define AP_AHRS_DCM_ENABLED AP_AHRS_BACKEND_DEFAULT_ENABLED && AP_INERTIALSENSOR_ENABLED
#endif

#ifndef HAL_NAVEKF2_AVAILABLE
// EKF2 slated compiled out by default in 4.5, slated to be removed.
#define HAL_NAVEKF2_AVAILABLE 0
#endif

#ifndef HAL_NAVEKF3_AVAILABLE
#define HAL_NAVEKF3_AVAILABLE 1
#endif

#ifndef AP_AHRS_SIM_ENABLED
#define AP_AHRS_SIM_ENABLED AP_SIM_ENABLED
#endif

#ifndef AP_AHRS_POSITION_RESET_ENABLED
#define AP_AHRS_POSITION_RESET_ENABLED (BOARD_FLASH_SIZE>1024)
#endif

