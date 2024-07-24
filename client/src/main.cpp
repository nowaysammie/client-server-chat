#include "Client.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Enter IP address!" << std::endl;
        return 0;
    }
    else
    {
        Client client;
        uint8_t state = client.init(argv[1]);
        if (state != C_SHUTDOWN)
        {
            if (state != C_NO_VALID_IP)
            {
                state = C_DISCONNECTED;
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
                    while (state != C_HANDLE_END && state != C_SHUTDOWN)
                    {
                        state = client.eventHandler();
                    }
                }
            }
            else
            {
                std::cout << "No valid IP-address" << std::endl;
            }
        }
        else
        {
            std::cout << "Client shuts down!" << std::endl;
        }
    }
    return 0;
}