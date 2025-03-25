#ifndef E131_SENDER_WRAPPER_H
#define E131_SENDER_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Opaque struct to hide C++ class details from C
typedef struct E131SenderWrapper E131SenderWrapper;

// Creates an E131Sender instance
E131SenderWrapper* create_e131_sender(const char* ip, uint16_t port);

// Sends data
int send_e131_data(E131SenderWrapper* sender, uint8_t* pixels, uint16_t length);

// Closes the socket
int close_e131_socket(E131SenderWrapper* sender);

// Destroys the E131Sender instance
void destroy_e131_sender(E131SenderWrapper* sender);

#ifdef __cplusplus
}
#endif

#endif // E131_SENDER_WRAPPER_H

