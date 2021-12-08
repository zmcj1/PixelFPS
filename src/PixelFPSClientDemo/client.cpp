//asio lib
#include "asio.hpp"
#include "asio/ts/buffer.hpp"
#include "asio/ts/internet.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

//namespaces
using namespace std;
using namespace asio;
using namespace asio::ip;

int main(int argc, char** argv)
{
    string ip_address = "124.70.1.25";
    int port = 17971;

    if (argc == 3)
    {
        ip_address = argv[1];
        port = stoi(string(argv[2]));
    }

    error_code ec;

    io_context context;

    tcp::endpoint endPoint(make_address(ip_address.c_str(), ec), port);

    tcp::socket socket(context);

    socket.connect(endPoint, ec);

    if (!ec)
    {
        cout << "Connected!\n";
    }
    else
    {
        cout << "Failed!\n" + ec.message() + "\n";
    }

    if (socket.is_open())
    {
        this_thread::sleep_for(2000ms); //wait for receive msg

        size_t bytes = socket.available();
        cout << "Bytes Available:" << bytes << "\n";

        if (bytes > 0)
        {
            vector<char> data_buffer(bytes);
            socket.read_some(buffer(data_buffer.data(), data_buffer.size()), ec);

            for (auto& c : data_buffer)
            {
                cout << c;
            }
        }
    }

    cin.get();

    return 0;
}