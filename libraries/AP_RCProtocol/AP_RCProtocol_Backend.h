/*
 * This file is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Code by Andrew Tridgell and Siddharth Bharat Purohit
 */

#pragma once

#include "AP_RCProtocol.h"

#if AP_RCPROTOCOL_ENABLED

#include <AP_HAL/utility/sparse-endian.h>
#include <AP_VideoTX/AP_VideoTX_config.h>

class AP_RCProtocol_Backend {
    friend class AP_RCProtcol;

public:
    AP_RCProtocol_Backend(AP_RCProtocol &_frontend, const AP_RCProtocol::rcprotocol_t _protocol_type);
    virtual ~AP_RCProtocol_Backend() {}
    // process a pulse and return true if processed otherwise SoftSerial will be used
    virtual bool process_pulse(const uint32_t width_s0, const uint32_t width_s1) { return false; }
    virtual void process_byte(uint32_t timestamp_us, uint8_t byte, uint32_t baudrate) {}
    virtual void process_handshake(uint32_t baudrate) {}
    bool new_input();

    // support for receivers that have FC initiated bind support
    virtual void start_bind(void) {}

    // allow for backends that need regular polling
    virtual void update(void) {}
    enum {
        PARSE_TYPE_SIGREAD,
        PARSE_TYPE_SERIAL
    };

    // get number of frames, ignoring failsafe
    uint32_t get_rc_frame_count(void) const {
        return rc_frame_count;
    }

    // reset valid rc frame count
    void reset_rc_frame_count(void) {
        rc_frame_count = 0;
    }

    // get number of frames, honoring failsafe
    uint32_t get_rc_input_count(void) const {
        return rc_input_count;
    }

    uint32_t get_rc_protocols_mask(void) const {
        return frontend.rc_protocols_mask;
    }

    // get UART for RCIN, if available. This will return false if we
    // aren't getting the active RC input protocol via the uart
    AP_HAL::UARTDriver *get_UART(void) const {
        return frontend._detected_with_bytes?frontend.added.uart:nullptr;
    }

    // get an available uart regardless of whether we have detected a protocol via it
    AP_HAL::UARTDriver *get_available_UART(void) const {
        return frontend.added.uart;
    }

    // return true if we have a uart available for protocol handling.
    bool have_UART(void) const {
        return frontend.added.uart != nullptr;
    }

    // is the receiver active, used to detect power loss and baudrate changes
    virtual bool is_rx_active() const {
        return true;
    }

#if AP_VIDEOTX_ENABLED
    // called by static methods to confiig video transmitters:
    static void configure_vtx(uint8_t band, uint8_t channel, uint8_t power, uint8_t pitmode);
#endif

protected:

    struct Channels11Bit_8Chan {
#if __BYTE_ORDER != __LITTLE_ENDIAN
#error "Only supported on little-endian architectures"
#endif
        uint32_t ch0 : 11;
        uint32_t ch1 : 11;
        uint32_t ch2 : 11;
        uint32_t ch3 : 11;
        uint32_t ch4 : 11;
        uint32_t ch5 : 11;
        uint32_t ch6 : 11;
        uint32_t ch7 : 11;
    } PACKED;

    void add_input(uint8_t num_channels, uint16_t *values, bool in_failsafe, int16_t rssi=-1, int16_t rx_link_quality=-1);
    AP_RCProtocol &frontend;

    void log_data(AP_RCProtocol::rcprotocol_t prot, uint32_t timestamp, const uint8_t *data, uint8_t len) const;

    // decode channels from the standard 11bit format (used by CRSF and SBUS)
    void decode_11bit_channels(const uint8_t* data, uint8_t nchannels, uint16_t *values, uint16_t mult, uint16_t div, uint16_t offset);

private:
    uint32_t rc_input_count;
    uint32_t last_rc_input_count;
    uint32_t rc_frame_count;

    const AP_RCProtocol::rcprotocol_t protocol_type;
};

#endif  // AP_RCPROTOCOL_ENABLED
