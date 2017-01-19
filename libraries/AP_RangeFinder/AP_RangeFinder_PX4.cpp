/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <AP_HAL/AP_HAL.h>

#if CONFIG_HAL_BOARD == HAL_BOARD_PX4 || CONFIG_HAL_BOARD == HAL_BOARD_VRBRAIN
#include <AP_BoardConfig/AP_BoardConfig.h>
#include "AP_RangeFinder_PX4.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <drivers/drv_range_finder.h>
#include <drivers/drv_hrt.h>
#include <uORB/topics/distance_sensor.h>
#include <stdio.h>
#include <errno.h>

extern const AP_HAL::HAL& hal;

uint8_t AP_RangeFinder_PX4::num_px4_instances = 0;

/*
   The constructor also initialises the rangefinder. Note that this
   constructor is not called until detect() returns true, so we
   already know that we should setup the rangefinder
*/
AP_RangeFinder_PX4::AP_RangeFinder_PX4(RangeFinder &_ranger, uint8_t instance, RangeFinder::RangeFinder_State &_state) :
    AP_RangeFinder_Backend(_ranger, instance, _state),
    _last_max_distance_cm(-1),
    _last_min_distance_cm(-1)
{
    _fd = open_driver();

    // consider this path used up
    num_px4_instances++;

    if (_fd == -1) {
        hal.console->printf("Unable to open PX4 rangefinder %u\n", num_px4_instances);
        set_status(RangeFinder::RangeFinder_NotConnected);
        return;
    }

    // average over up to 20 samples
    if (ioctl(_fd, SENSORIOCSQUEUEDEPTH, 20) != 0) {
        hal.console->printf("Failed to setup range finder queue\n");
        set_status(RangeFinder::RangeFinder_NotConnected);
        return;
    }

    // initialise to connected but no data
    set_status(RangeFinder::RangeFinder_NoData);
}

/*
   close the file descriptor
*/
AP_RangeFinder_PX4::~AP_RangeFinder_PX4()
{
    if (_fd != -1) {
        close(_fd);
    }
}

extern "C" {
    int ll40ls_main(int, char **);
    int trone_main(int, char **);
    int mb12xx_main(int, char **);
};

/*
   open the PX4 driver, returning the file descriptor
*/
int AP_RangeFinder_PX4::open_driver(void)
{
    if (num_px4_instances == 0) {
        /*
          we start the px4 rangefinder drivers on demand
        */
        const char *strLl40ls = "ll40ls";
        if (AP_BoardConfig::px4_start_driver(ll40ls_main, strLl40ls, "-X start")) {
            hal.console->printf("Found external %s sensor\n", strLl40ls);
        }
        if (AP_BoardConfig::px4_start_driver(ll40ls_main, strLl40ls, "-I start")) {
            hal.console->printf("Found internal %s sensor\n", strLl40ls);
        }
        const char *strTrone = "trone";
        const char *strStart = "start";
        if (AP_BoardConfig::px4_start_driver(trone_main, strTrone, strStart)) {
            hal.console->printf("Found %s sensor\n", strTrone);
        }
        const char *strMb12xx = "mb12xx";
        if (AP_BoardConfig::px4_start_driver(mb12xx_main, strMb12xx, strStart)) {
            hal.console->printf("Found %s sensor\n", strMb12xx);
        }
    }
    // work out the device path based on how many PX4 drivers we have loaded
    char path[] = RANGE_FINDER_BASE_DEVICE_PATH "n";
    path[strlen(path)-1] = '0' + num_px4_instances;
    return open(path, O_RDONLY);
}

/*
   see if the PX4 driver is available
*/
bool AP_RangeFinder_PX4::detect(RangeFinder &_ranger, uint8_t instance)
{
    int fd = open_driver();
    if (fd == -1) {
        return false;
    }
    close(fd);
    return true;
}

void AP_RangeFinder_PX4::update(void)
{
    if (_fd == -1) {
        set_status(RangeFinder::RangeFinder_NotConnected);
        return;
    }

    struct distance_sensor_s range_report;
    float sum = 0;
    uint16_t count = 0;

    if (_last_max_distance_cm != ranger._max_distance_cm[state.instance] ||
        _last_min_distance_cm != ranger._min_distance_cm[state.instance]) {
        float max_distance = ranger._max_distance_cm[state.instance]*0.01f;
        float min_distance = ranger._min_distance_cm[state.instance]*0.01f;
        if (ioctl(_fd, RANGEFINDERIOCSETMAXIUMDISTANCE, (unsigned long)&max_distance) == 0 &&
            ioctl(_fd, RANGEFINDERIOCSETMINIUMDISTANCE, (unsigned long)&min_distance) == 0) {
            _last_max_distance_cm = ranger._max_distance_cm[state.instance];
            _last_min_distance_cm = ranger._min_distance_cm[state.instance];
        }
    }


    while (::read(_fd, &range_report, sizeof(range_report)) == sizeof(range_report) &&
           range_report.timestamp != _last_timestamp) {
            // take reading
            sum += range_report.current_distance;
            count++;
            _last_timestamp = range_report.timestamp;
    }

    // if we have not taken a reading in the last 0.2s set status to No Data
    if (AP_HAL::micros64() - _last_timestamp >= 200000) {
        set_status(RangeFinder::RangeFinder_NoData);
    }

    if (count != 0) {
        state.distance_cm = sum / count * 100.0f;
        state.distance_cm += ranger._offset[state.instance];

        // update range_valid state based on distance measured
        update_status();
    }
}

#endif // CONFIG_HAL_BOARD
