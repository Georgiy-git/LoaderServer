#include "Connector.hpp"

int main() {
	unsigned int port = 53888;
	setlocale(LC_ALL, "RU");
	io_context io_context;
	ip::tcp::acceptor acceptor(io_context, ip::tcp::endpoint(ip::tcp::v4(), port));
	Connector connect(io_context, acceptor, port);
	io_context.run();
}