#include <cstdint>
#include <e131.h>
#include <string>
#include "err.h"
#define BUFSIZE 512
class E131Sender{
    public:
        E131Sender(const char * ip, uint16_t port=E131_DEFAULT_PORT){
            if ((sockfd = e131_socket()) < 0)
                err(EXIT_FAILURE, "e131_socket");
            if (e131_unicast_dest(&dest, ip, port) < 0)
                err(EXIT_FAILURE, "e131_unicast_dest");
        }
        int send(uint8_t * pixels, uint16_t length);
        int e131_socket_close();
        int next();
    private:
        int sockfd;
        e131_addr_t dest;
        e131_packet_t packet;
};