#include "e131_sender.h"
#include <iostream>
#include <random>
#include "sleep.h"

int main(){
    const int SIZE = 512*96;
    unsigned char arr[SIZE];

    //Make random array for 96 universe leds
    std::random_device rd; 
    std::mt19937 mt(rd()); 
    std::uniform_int_distribution<int> dist(2, 255); 
    for(int i = 0; i < SIZE; i++){
        arr[i] = (unsigned char)dist(mt);
    }

    //Senders ip
    const char * ip= "192.168.50.72";
    //Initialize E131Sender
    E131Sender * sender = new E131Sender(ip);
    for(;;){
        //Send Random Array
        sender->send(arr, SIZE);
        //Move to next frame
        sender->next();
        //Sleep for 1 sec
        usleep(1000000);
    }

}
