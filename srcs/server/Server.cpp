#include "Server.hpp"

/* Request */
Server::Request::Request() {
	this->_buffer = "";
	this->_length = -1;
	this->_sent = 0;
}

Server::Request::Request(Server::Request const &x) :
	_buffer(x._buffer), _length(x._length) {
}

Server::Request::~Request() {
}

Server::Request &Server::Request::operator=(Server::Request const &x) {
	this->_buffer = x._buffer;
	this->_length = x._length;
	this->_sent = x._sent;
	return (*this);
}

void Server::Request::parseHeader(std::string const &header) {
	int pos;
	int lf;

	pos = 0;
	while ((lf = header.find("\r\n", pos)) != std::string::npos) {
		if (pos != 0)
			_headers.insert(ft::headerPair(std::string(header.begin() + pos, header.begin() + lf)));
		pos = lf + 2;
	}
	_headers.insert(ft::headerPair(std::string(header, pos)));
}


/* Server */
/* exception */
const char *Server::CreateException::what() const throw() {
	return ("ServerException: Failed to create server");
}

const char *Server::ListenException::what() const throw() {
	return ("ServerException: Failed to open server");
}

const char *Server::AcceptException::what() const throw() {
	return ("ServerException: Failed to accept connection");
}

const char *Server::ReceiveException::what() const throw() {
	return ("ServerException: Failed to receive packet");
}

const char *Server::SendException::what() const throw() {
	return ("ServerException: Failed to send packet");
}

/* coplien */
Server::Server() {
}

Server::Server(Server const &x) {
}

Server::~Server() {
	std::multimap<int, Request *>::iterator it;
	std::multimap<int, Request *>::iterator n_it;

	it = this->_request.begin();
	while (it != this->_request.end()) {
		n_it = it;
		++n_it;
		delete it->second;
		this->_request.erase(it);
		it = n_it;
	}
}

Server &Server::operator=(Server const &x) {
	return (*this);
}

int Server::getPort() {
	return (this->_port);
}

void Server::socketBind() {
	if ((this->_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw CreateException();
	ft::memset((char *)&this->_addr, 0, sizeof(this->_addr));
	this->_addr.sin_family = AF_INET;
	this->_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	this->_addr.sin_port = htons(this->_port);
	if (bind(this->_socket, (sockaddr *)&this->_addr, sizeof(this->_addr)) == -1)
		throw CreateException();	
}

void Server::run() {
	if (listen(this->_socket, 512) == -1)
		throw ListenException();
	std::cout << this->_socket << " is being run (port: " << this->_port << ")" << std::endl;
}

int Server::accept() {
	int socket;

	if ((socket = ::accept(this->_socket, 0, 0)) == -1)
		throw AcceptException();
	fcntl(socket, F_SETFL, O_NONBLOCK);
	this->_request.insert(std::make_pair<int, Request *>(socket, new Request));
	return (socket);
}

int Server::recv(int socket) {
	char buf[RECV_BUFFER_SIZE + 1];
	std::string buffer = "";
	int len;

	len = ::recv(socket, buf, RECV_BUFFER_SIZE, 0);
	buf[len] = 0;
	buffer += buf;
	if (len == -1)
		throw ReceiveException();
	if (len == 0)
	{
		std::cout << "[disconnected;]" << std::endl;
		::close(socket);
		return (ERR_RECV);
	}
	this->_request[socket]->_buffer += buffer;
	if (this->_request[socket]->_length == -1 && this->_request[socket]->_buffer.find("\r\n\r\n") == std::string::npos)
		return (WAIT_RECV);
	if (this->_request[socket]->_length == -1) {
		this->_request[socket]->parseHeader( this->_request[socket]->_buffer.substr(0, this->_request[socket]->_buffer.find("\r\n\r\n")) );
		// chunked
		if (this->_request[socket]->_headers.find("Transfer-Encoding") != this->_request[socket]->_headers.end() &&
			this->_request[socket]->_headers["Transfer-Encoding"] == "chunked") {
			this->_request[socket]->_length = -1;
		}
		else { // normal
			this->_request[socket]->_length = 0;
			if (this->_request[socket]->_headers.find("Content-Length") != this->_request[socket]->_headers.end())
				this->_request[socket]->_length = ft::atoi(const_cast<char *>(this->_request[socket]->_headers["Content-Length"].c_str()));
		}
	}
	if (this->_request[socket]->_length != -1 &&
	this->_request[socket]->_buffer.substr(this->_request[socket]->_buffer.find("\r\n\r\n") + 4).length() >= this->_request[socket]->_length) {
		std::cout << std::endl << "RECV ▼ (size: " << this->_request[socket]->_length << ")" << std::endl;
		std::cout << "[" << this->_request[socket]->_buffer << "]" << std::endl;
		return (ALL_RECV);
	}
	if (this->_request[socket]->_length == -1 &&
		this->_request[socket]->_headers.find("Transfer-Encoding") != this->_request[socket]->_headers.end() &&
		this->_request[socket]->_headers["Transfer-Encoding"] == "chunked" &&
		this->_request[socket]->_buffer.find("\r\n0\r\n") != std::string::npos) {
		std::cout << std::endl << "RECV chunked ▼ (size: " << this->_request[socket]->_length << ")" << std::endl;
		std::cout << "[" << this->_request[socket]->_buffer << "]" << std::endl;
		return (ALL_RECV);
	}
	return (WAIT_RECV);
}

int Server::send(int socket) {
	std::string buf;
	int len;
	int buf_size;

	std::string body;
	std::string header;
	/* tmp */
	body = "hello world\nSocket: " + ft::to_string(this->_socket) + "\nPort: " + ft::to_string(this->_port) + "\n";
	header = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: " + ft::to_string(body.length()) + "\n\n";

	std::string response = header + body;
	std::cout << std::endl << "SEND ▼" << std::endl;
	std::cout << "[" << response << "]" << std::endl;

	/* re */
	buf_size = response.length() - this->_request[socket]->_sent < SEND_BUFFER_SIZE ?
		response.length() - this->_request[socket]->_sent : SEND_BUFFER_SIZE;
	buf = std::string(response, this->_request[socket]->_sent, buf_size);
	len = ::send(socket, buf.c_str(), buf.length(), 0);
	if (len == -1)
		throw SendException();
	if (len == 0)
		return (ERR_SEND);
	this->_request[socket]->_sent += len;
	if (this->_request[socket]->_sent >= response.length()) {
		return (ALL_SEND);
	}
	return (WAIT_SEND);
}

/*
	std::string body = "hello world\nSocket: " + std::to_string(this->_socket) + "\nPort: " + std::to_string(this->_port) + "\n";
	std::string header = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: " + std::to_string(body.length()) + "\n\n";

	write(socket, (header + body).c_str(), (header + body).length());
 */