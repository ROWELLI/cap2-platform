#include "e131_sender_wrapper.h"
#include "e131_sender.h"

// Define the wrapper struct
struct E131SenderWrapper {
    E131Sender* instance;
};

// Creates an E131Sender instance
E131SenderWrapper* create_e131_sender(const char* ip, uint16_t port) {
    return new E131SenderWrapper{new E131Sender(ip, port)};
}

// Sends data using E131Sender
int send_e131_data(E131SenderWrapper* sender, uint8_t* pixels, uint16_t length) {
    if (sender && sender->instance) {
        return sender->instance->send(pixels, length);
    }
    return -1;  // Error: Invalid sender instance
}

// Closes the socket
int close_e131_socket(E131SenderWrapper* sender) {
    if (sender && sender->instance) {
        return sender->instance->e131_socket_close();
    }
    return -1;  // Error: Invalid sender instance
}

// Destroys an E131Sender instance
void destroy_e131_sender(E131SenderWrapper* sender) {
    if (sender) {
        delete sender->instance;
        delete sender;
    }
}

