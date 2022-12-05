#include <fstream>
#include <string>
#include <boost/asio.hpp>
#include <iostream>
using namespace std;
using namespace boost::asio;
using boost::asio::ip::tcp;


int main(int argc, char* argv[])
{
    string serverName = "static-maps.yandex.ru";
    string getCommand = "/1.x/?";
    string shirota, dolgota, map_type;
    cout << "Vvedite shirotu: ";
    cin >> dolgota;
    cout << "Vvedite dolgotu: ";
    cin >> shirota;
    cout << "Vvedite tip: ";
    cin >> map_type;
    getCommand += "ll=" + dolgota + "," + shirota + "&size=650,450&z=13&l=" + map_type + "&z=13";
    std::cout << " Vash zapros " << getCommand << endl;
    std::ofstream outFile("image.png", std::ofstream::out | std::ofstream::binary);

    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(serverName, "http");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;
    tcp::socket socket(io_service);
    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end)
    {
        socket.close();
        socket.connect(*endpoint_iterator++, error);
    }

    boost::asio::streambuf request;
    std::ostream request_stream(&request);

    request_stream << "GET " << getCommand << " HTTP/1.0\r\n";
    request_stream << "Host: " << serverName << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";

    boost::asio::write(socket, request);

    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, "\r\n");

    std::istream response_stream(&response);
    std::string http_version;
    response_stream >> http_version;
    std::string status_message;
    std::getline(response_stream, status_message);

    boost::asio::read_until(socket, response, "\r\n\r\n");

    std::string header;
    while (std::getline(response_stream, header) && header != "\r"){}

    if (response.size() > 0)
    {
        outFile << &response;
    }

    while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error))
    {
        outFile << &response;
    }

    outFile.close();

    return 0;
}
