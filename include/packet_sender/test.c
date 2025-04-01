#include "e131_sender.h"
#include <iostream>
#include <random>
#include "e131_sender_wrapper.h"

int main() {
    const char* ip = "192.168.1.100";
    uint16_t port = 5568;

    E131SenderWrapper* sender = create_e131_sender(ip, port);
    if (!sender) {
        printf("Failed to create E131 sender\n");
        return 1;
    }

    uint8_t pixels[512] = {255, 0, 0};  // Example data
    int result = send_e131_data(sender, pixels, sizeof(pixels));

    if (result < 0) {
        printf("Failed to send E131 data\n");
    } else {
        printf("E131 data sent successfully\n");
    }

    close_e131_socket(sender);
    destroy_e131_sender(sender);

    return 0;
}

