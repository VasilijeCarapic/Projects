#include "Networking/Links/UdpStreamLink.h"
#include "Networking/Links/LinkManager.h"

#include "IPAddress.h"
#include <cstdint>

void UdpStreamLink::Init(){
    for (int i = 0; i < SSTREAM_CONNECTIONS_MAX_NUMBER; i++) {
        udp_streams[i].slot_taken = false; 
        udp_streams[i].is_streaming = false; 
        udp_streams[i].sid = 0; 
    
    }
}

int UdpStreamLink::findSlotBySid(uint32_t sid){
    for(int i = 0; i < SSTREAM_CONNECTIONS_MAX_NUMBER; i++){
        if (udp_streams[i].slot_taken && udp_streams[i].sid == sid ){
            return i; 
        }
    }
    return -1; 
}

/**
 * @brief finds first free sid 
 * 
 * @return int: position of the first free sid 
 */
int UdpStreamLink::findFreeSlot(){
    for(int i = 0; i < SSTREAM_CONNECTIONS_MAX_NUMBER; i++){
        if(udp_streams[i].slot_taken == false ){
            return i; 
        }
    }
    return -1; 
}

bool UdpStreamLink::StreamCreate(uint32_t sid, const char* ip, uint16_t port){

    IPAddress udpIP; 

    /* delete stream if it exists */
    int curr_sid = findSlotBySid(sid); 

    if(curr_sid >= 0){ 
        /*stop sid*/
        udp_streams[curr_sid].udp.stop();

        udp_streams[curr_sid].slot_taken = false; 

        udp_streams[curr_sid].is_streaming = false; 
    }

    /* find free id slot */
    int slot = findFreeSlot();
    if (slot < 0) {
        return false; 
    }

    /* bad IP format */
    if(udpIP.fromString(ip) == false){
        return false;
    }

    udp_streams[slot].ip            = udpIP; 
    udp_streams[slot].port          = port; 
    udp_streams[slot].sid           = sid; 
    udp_streams[slot].slot_taken    = true; 
    udp_streams[slot].is_streaming  = false;
    
    udp_streams[slot].udp.begin(this->udp_streams[slot].port);

    return true; 

}

/**
 * @brief   Auto-allocating variant of StreamCreate(): finds the first free
 *          slot (0-indexed), uses the slot index as the sid so it stays in
 *          lockstep with the STM32's AllocateStream() (also a 0-indexed
 *          "first free" search), creates the stream on it, and immediately
 *          calls StreamStart() since no separate "stream start" command
 *          will follow for a sid the GUI never knew in advance.
 *
 * @param   ip: target IP address for the stream.
 * @param   port: target UDP port for the stream.
 * @param   outSid: filled with the assigned sid on success, untouched on failure.
 * @retval  true if slot allocation, socket creation, and start all succeeded.
 * @retval  false otherwise (pool full, bad IP, or start failed -> slot rolled back).
 */
bool UdpStreamLink::StreamCreateAuto(const char* ip, uint16_t port, uint32_t* outSid){
    int slot = findFreeSlot();
    if (slot < 0) {
        return false;
    }

    uint32_t sid = (uint32_t)slot;

    if (!StreamCreate(sid, ip, port)) {
        return false;
    }

    if (!StreamStart(sid)) {
        StreamStop(sid); /* rollback */
        return false;
    }

    if (outSid != nullptr) *outSid = sid;
    return true;
}

bool UdpStreamLink::StreamStart(uint32_t sid){
    int slot = findSlotBySid(sid); 
    
    if(slot < 0) {
        return false; 
    }

    udp_streams[slot].is_streaming = true; 
    return true; 
}

bool UdpStreamLink::StreamPause(uint32_t sid){
    int slot = findSlotBySid(sid); 

    if (slot < 0) { return false; }

    udp_streams[slot].is_streaming = false; 

    return true; 
}

/**
 * @brief   Pauses the stream (is_streaming = false) without releasing the
 *          slot, sid, or UDP socket. Deliberately does NOT free the slot:
 *          the STM32's "stream stop" handler only clears is_streaming and
 *          keeps in_use set, so this side must mirror that or sid
 *          allocation between ESP and STM32 will drift out of sync on the
 *          next "stream create".
 */
bool UdpStreamLink::StreamStop(uint32_t sid){ 
    int slot = findSlotBySid(sid); 

    if (slot < 0) { return false; }

    udp_streams[slot].is_streaming = false; 

    return true; 
}

bool UdpStreamLink::SendSample(uint32_t sid, const uint8_t* data, size_t size){
    int slot = findSlotBySid(sid); 

    if(slot < 0 || !udp_streams[slot].slot_taken || !udp_streams[slot].is_streaming){
        return false;         
    }

    udp_streams[slot].udp.beginPacket(udp_streams[slot].ip, udp_streams[slot].port);

    udp_streams[slot].udp.write(data, size);
    
    return udp_streams[slot].udp.endPacket() == 1; 
}

bool UdpStreamLink::IsSlotTaken(uint32_t sid){ 
    int slot = findSlotBySid(sid); 

    if (slot < 0) return false; 
    
    return (udp_streams[slot].slot_taken);  

}