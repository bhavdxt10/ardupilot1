#pragma once

#include <AP_HAL/AP_HAL_Boards.h>

#ifndef HAL_EXTERNAL_AHRS_ENABLED
#define HAL_EXTERNAL_AHRS_ENABLED BOARD_FLASH_SIZE > 1024
#endif

#ifndef AP_EXTERNAL_AHRS_BACKEND_DEFAULT_ENABLED
#define AP_EXTERNAL_AHRS_BACKEND_DEFAULT_ENABLED HAL_EXTERNAL_AHRS_ENABLED
#endif

#ifndef AP_EXTERNAL_AHRS_MICROSTRAIN5_ENABLED
#define AP_EXTERNAL_AHRS_MICROSTRAIN5_ENABLED AP_EXTERNAL_AHRS_BACKEND_DEFAULT_ENABLED
#endif

#ifndef AP_EXTERNAL_AHRS_MICROSTRAIN7_ENABLED
#define AP_EXTERNAL_AHRS_MICROSTRAIN7_ENABLED AP_EXTERNAL_AHRS_BACKEND_DEFAULT_ENABLED
#endif

#ifndef AP_MICROSTRAIN_ENABLED
#define AP_MICROSTRAIN_ENABLED AP_EXTERNAL_AHRS_MICROSTRAIN5_ENABLED || AP_EXTERNAL_AHRS_MICROSTRAIN7_ENABLED
#endif

#ifndef AP_EXTERNAL_AHRS_VECTORNAV_ENABLED
#define AP_EXTERNAL_AHRS_VECTORNAV_ENABLED AP_EXTERNAL_AHRS_BACKEND_DEFAULT_ENABLED
#endif

#ifndef AP_EXTERNAL_AHRS_INERTIAL_LABS_ENABLED
#define AP_EXTERNAL_AHRS_INERTIAL_LABS_ENABLED AP_EXTERNAL_AHRS_BACKEND_DEFAULT_ENABLED
#endif

#ifndef AP_EXTERNAL_AHRS_ADNAV_ENABLED
#define AP_EXTERNAL_AHRS_ADNAV_ENABLED AP_EXTERNAL_AHRS_BACKEND_DEFAULT_ENABLED
#endif
