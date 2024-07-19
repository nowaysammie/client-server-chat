#include "../include/Client.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Enter IP address!" << std::endl;
        return 0;
    }
    Client client;
    std::cout << (int)client.init(argv[1]) << std::endl;
    uint8_t state = C_DISCONNECTED;
    while (state != C_CONNECTED)
    {
        state = client.connectToServer();
    }
    client.printStartScreen();
    while (state != C_SHUTDOWN)
    {
        state = client.toPoll();
        if (state == 0)
        {
            continue;
        }
        state = C_OK;
        while (state != C_POLL_END && state != C_SHUTDOWN)
        {
            state = client.eventHandler();
        }
    }
    return 0;
}