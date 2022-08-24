#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// --- <Config> ---
#define BF_MAX_TRANSMISSION_UNIT 1600
#define BF_NUM_CHANNELS 2
#define BF_SYNC_SIGNAL "BFinger"
#define BF_MINISYNC_SIGNAL '\n'
// --- </Config> ---

struct bf_packet_header {
    uint8_t pkt_channel;
    uint16_t pkt_len;
    uint16_t pkt_checksum;
} __attribute__((packed, scalar_storage_order("little-endian")));

union bf_packet_header_u {
    struct bf_packet_header parsed;
    uint8_t raw[sizeof(struct bf_packet_header)];
};

struct bf_state {
    // --- <Callbacks> ---
    void (*send_byte)(void* context, uint8_t b);
    void (*handle_packet)(void* context, uint8_t channel, uint16_t pkt_size, uint8_t* pkt);
    void* context;
    // --- </Callbacks> ---

    // Receive
    bool synced;
    uint8_t sync_index;
    uint8_t packet_header_index;
    union bf_packet_header_u packet_header;
    uint16_t packet_index;
    uint8_t recv_packet[BF_MAX_TRANSMISSION_UNIT];
};

/// Feeds a single received byte into the protocol.
/// If the byte just completed a packet, the channel on which it was received is returned.
///
/// If a packet was received, then `state->packet` and `state->packet_size` contain the packet
/// (until the next call to this function).
///
/// \param bf The bufferfinger context
/// \param b The byte read
/// \return The channel number which received a packet, or -1 on no packet
void bf_process_byte(struct bf_state* bf, uint8_t b);

/// Sends a given packet using `bf->send_byte`
///
/// \param bf The BufferFinger context
/// \param channel The channel to send on
/// \param pkt_size Buffer size
/// \param pkt Buffer to send
void bf_send_packet(struct bf_state* bf, uint8_t channel, uint16_t pkt_size, uint8_t* pkt);

/// Calculates the checksum of the buffer
uint16_t bf_crc16(size_t size, const void* buf);
