#include "e131_sender.h"
#include <cstring>
#include <unistd.h>

int E131Sender::e131_socket_close(){
    return close(sockfd);
}

int E131Sender::send(uint8_t * pixels, uint16_t length){
    int universe_count = length/512;
    int last_universe = length%512;
    for(int universe = 1; universe <= universe_count; universe++){
        e131_pkt_init(&packet, universe, BUFSIZE);
        memcpy(&packet.frame.source_name, "E1.31 LED Controller", 21);
        memcpy(&(packet.dmp.prop_val[1]), pixels, 512);
        if (e131_send(sockfd, &packet, &dest) < 0)
            return 1;
        // e131_pkt_dump(stderr, &packet);
        pixels+=512;
    }
    if(!last_universe) return 0;
    e131_pkt_init(&packet, universe_count+1, BUFSIZE);
    memcpy(&packet.frame.source_name, "E1.31 LED Controller", 21);
    memcpy(&(packet.dmp.prop_val[1]), pixels, last_universe);
    if (e131_send(sockfd, &packet, &dest) < 0)
        return 1;
    // e131_pkt_dump(stderr, &packet);
    return 0;
}

int E131Sender::next(){
    return ++(packet.frame.seq_number);
}