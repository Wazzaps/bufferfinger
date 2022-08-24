#include <stdio.h>
#include "../c/bufferfinger.h"

static void hexdump_stderr(size_t len, uint8_t* buf) {
    size_t addr = 0;
    const size_t hexdump_bytes_per_line = 8;
    while (len > 0) {
        fprintf(stderr, "%04zx: ", addr);

        // Print hex bytes
        for (size_t i = 0; i < hexdump_bytes_per_line; i++) {
            // if (i % 4 == 0) {
            fprintf(stderr, " ");
            // }
            if (len > i) {
                fprintf(stderr, "%02x", buf[i]);
            } else {
                fprintf(stderr, "  ");
            }
        }

        // Print ascii bytes
        fprintf(stderr, "  ");
        for (size_t i = 0; i < hexdump_bytes_per_line; i++) {
            if (len > i) {
                // Is printable
                if (buf[i] >= ' ' && buf[i] <= '~') {
                    fprintf(stderr, "%c", buf[i]);
                } else {
                    fprintf(stderr, ".");
                }
            }
        }

        len = len < hexdump_bytes_per_line ? 0 : len - hexdump_bytes_per_line;
        buf += hexdump_bytes_per_line;
        addr += hexdump_bytes_per_line;
        fprintf(stderr, "\n");
    }
}

static void send_byte(void* context, uint8_t b) {
    // context contains the peer `bf_state`
    bf_process_byte((struct bf_state*) context, b);
}

static void handle_packet(void* context, uint8_t channel, uint16_t pkt_size, uint8_t* pkt) {
    fprintf(stderr, "\n[#%p] Got packet on channel %d:\n", context, channel);
    hexdump_stderr(pkt_size, pkt);
}

int main() {
    struct bf_state bf1 = {0};
    struct bf_state bf2 = {0};
    bf1.send_byte = bf2.send_byte = send_byte;
    bf1.handle_packet = bf2.handle_packet = handle_packet;
    // context shall contain the peer
    bf1.context = &bf2;
    bf2.context = &bf1;

    printf("[#%p] I am client 1\n", bf1.context);
    printf("[#%p] I am client 2\n", bf2.context);

    bf_send_packet(&bf1, 0, 4, (uint8_t[]) {0xde, 0xad, 0xbe, 0xef});
    bf_send_packet(&bf1, 1, sizeof("Hello, world!")-1, (uint8_t*) "Hello, world!");
    bf_send_packet(&bf2, 0, 4, (uint8_t[]) {0xde, 0xad, 0xbe, 0xef});
    bf_send_packet(&bf2, 1, sizeof("Hello, world!")-1, (uint8_t*) "Hello, world!");

    return 0;
}