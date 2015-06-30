/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

/* 
   DataFlash Remote(via MAVLink) logging
*/

#include <AP_HAL.h>

#if HAL_OS_POSIX_IO
#include "DataFlash.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <AP_Math.h>
#include <stdio.h>
#include <time.h>
#include "../AP_HAL/utility/RingBuffer.h"


extern const AP_HAL::HAL& hal;

// initialisation
void DataFlash_MAVLink::Init(const struct LogStructure *structure, uint8_t num_types)
{
    memset(_block_num, 0, sizeof(_block_num));  
    DataFlash_Backend::Init(structure, num_types);

    _initialised = true;
    _logging_started = true; // in actual fact, we throw away
                             // everything until a client connects.
                             // This stops calls to start_new_log from
                             // the vehicles
}


/* Write a block of data at current offset */
void DataFlash_MAVLink::WriteBlock(const void *pBuffer, uint16_t size)
{   
    if (!_initialised || !_sending_to_client || !_writes_enabled) {
        return;
    }

    uint16_t copied = 0;
    while (copied < size) {
        uint16_t remaining_to_copy = size - copied;
        uint16_t _curr_remaining = _block_max_size - _latest_block_len;
        uint16_t to_copy = (remaining_to_copy > _curr_remaining) ? _curr_remaining : remaining_to_copy;
        memcpy(&_buf[_cur_block_address][_latest_block_len], &((const uint8_t *)pBuffer)[copied], to_copy);
        copied += to_copy;
        _latest_block_len += to_copy;
        if (_latest_block_len == _block_max_size) {
            _block_num[_cur_block_address] = _latest_block_num++;
            send_log_block(_cur_block_address);  //block full send it
            _cur_block_address = next_block_address();
            _latest_block_len = 0;
        }
    }
}

//Get address of (hopefully empty) block to ovewrite
int8_t DataFlash_MAVLink::next_block_address()
{

    uint8_t oldest_block_address = 0;
    for(uint8_t block = 0; block < _total_blocks; block++){
        if(_block_num[oldest_block_address] > _block_num[block]){
            oldest_block_address = block;
            if (_block_num[oldest_block_address] == 0) {
                break;
            }
        }
    }
    
    if (_block_num[oldest_block_address] != 0) {
        perf_count(_perf_overruns);
    }
    //printf("%d \n", oldest_block_address);
    return oldest_block_address;
}

void DataFlash_MAVLink::handle_ack(mavlink_channel_t chan, uint32_t block_num)
{
    if (!_initialised) {
        return;
    }
    if(block_num == MAV_REMOTE_LOG_DATA_BLOCK_STOP){
        // heads up - if you stop logging and start logging, your console
        // will get a misleading "APM Initialising" message.
        // printf("Received stop-logging packet\n");
        _sending_to_client = false;
        return;
    }
    if(block_num == MAV_REMOTE_LOG_DATA_BLOCK_START && !_sending_to_client){
        // printf("\nStarting New Log!!\n");
        _sending_to_client = true;
        memset(_block_num, 0, sizeof(_block_num));
        _chan = chan;
        _latest_block_num = 0;
        _cur_block_address = 0;
        _latest_block_len = 0;
        _front.StartNewLog();
        return;
    }
    for(uint8_t block = 0; block < _total_blocks; block++){
        if(_block_num[block] == block_num) {
            _block_num[block] = 0;                  //forget the block if ack is received
            return;
        }
    }
}
void DataFlash_MAVLink::remote_log_block_status_msg(mavlink_channel_t chan,
                                                    mavlink_message_t* msg)
{
    mavlink_remote_log_block_status_t packet;
    mavlink_msg_remote_log_block_status_decode(msg, &packet);
    if(packet.block_status == 0){
        handle_retry(packet.block_cnt);
    } else{
        handle_ack(chan, packet.block_cnt);
    }
}

void DataFlash_MAVLink::handle_retry(uint32_t block_num)
{
    if (!_initialised) {
        return;
    }
    for(uint8_t block = 0; block < _total_blocks; block++){
        if(_block_num[block] == block_num) {
            send_log_block(block);
            return;
        }
    }
}
//TODO: handle full txspace properly
void DataFlash_MAVLink::send_log_block(uint32_t block_address)
{

    if (!_initialised || comm_get_txspace(_chan) < 255){
       return; 
    }
    //printf("Data Sent!!\n");
    mavlink_msg_remote_log_data_block_send(_chan,_block_max_size,_block_num[block_address],_buf[block_address]);
    
}
#endif // HAL_OS_POSIX_IO

