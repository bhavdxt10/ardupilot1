#pragma once

#include <AP_SerialManager/AP_SerialManager.h>

#ifndef AP_SCRIPTING_SERIALDEVICE_NUM_PORTS
#define AP_SCRIPTING_SERIALDEVICE_NUM_PORTS 3
#endif

class AP_Scripting;

class AP_Scripting_SerialDevice
{
public:
    /* Do not allow copies */
    CLASS_NO_COPY(AP_Scripting_SerialDevice);

    AP_Scripting_SerialDevice() {}

    AP_Int8 enable;

    void init(void);

public:
    class Port : public AP_SerialManager::RegisteredPort {
    public:
        friend class AP_Scripting_SerialDevice;
        void init(void);

        uint8_t get_protocol_index(void) { return state.protocol; }

    private:
        bool is_initialized() override {
            return true;
        }
        bool tx_pending() override {
            return false;
        }

        bool init_buffers(const uint32_t size_rx, const uint32_t size_tx);

        uint32_t txspace() override;
        void _begin(uint32_t b, uint16_t rxS, uint16_t txS) override;
        size_t _write(const uint8_t *buffer, size_t size) override;
        ssize_t _read(uint8_t *buffer, uint16_t count) override;
        uint32_t _available() override;
        void _end() override {}
        void _flush() override {}
        bool _discard_input() override;

        ByteBuffer *readbuffer;
        ByteBuffer *writebuffer;
        uint32_t last_size_tx;
        uint32_t last_size_rx;

        HAL_Semaphore sem;
    };

    Port ports[AP_SCRIPTING_SERIALDEVICE_NUM_PORTS];
};
