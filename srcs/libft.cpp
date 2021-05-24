#include <webserv.hpp>

bool		ft::isspace(char c) {
	if (c == 0x20 || (0x09 <= c && c <= 0x0d))
		return (true);
	return (false);
}

int			ft::atoi(char *str) {
	int		ret;
	int		minus;

	ret = 0;
	minus = 1;
	while ((9 <= *str && *str <= 13) || *str == 32)
		str++;
	if (*str == '+')
		str++;
	else if (*str == '-')
	{
		minus = -1;
		str++;
	}
	while ('0' <= *str && *str <= '9')
	{
		ret *= 10;
		ret += *str - '0';
		str++;
	}
	return (minus * ret);
}

std::string	ft::to_string(int n)
{
	std::string ret = "";
	int number = n;

	if (n < 0)
		number *= -1;
	while (number) {
		ret += static_cast<char>(number % 10 + '0');
		number /= 10;
	}
	if (n < 0)
		ret += '-';
	return (std::string(ret.rbegin(), ret.rend()));
}

void	*ft::memset(void *_src, int val, size_t size) {
	unsigned char *src;
	unsigned char *ptr;

	src = reinterpret_cast<unsigned char *>(_src);
	ptr = src;
	while (size-- > 0)
		*(src++) = val;
	return (ptr);
}

void	ft::fd_zero(::fd_set *fds) {
	ft::memset(fds, 0, sizeof(*fds));
}

void	ft::fd_set(int fd, ::fd_set *fds) {
	fds->fds_bits[fd / 32] |= (1 << (fd % 32));
}

void	ft::fd_sets(int fd, struct fds *fds) {
	fd_set(fd, &fds->read);
	fd_set(fd, &fds->write);
	fd_set(fd, &fds->except);
}

void	ft::fd_clr(int fd, ::fd_set *fds) {
	fds->fds_bits[fd / 32] &= (~(1 << (fd % 32)));
}

void	ft::fd_clrs(int fd, struct fds *fds) {
	fd_clr(fd, &fds->read);
	fd_clr(fd, &fds->write);
	fd_clr(fd, &fds->except);
}

bool	ft::fd_isset(int fd, ::fd_set *fds) {
	if ((fds->fds_bits[fd / 32] & (1 << (fd % 32))) != 0)
		return (true);
	return (false);
}

std::pair<std::string, std::string> ft::headerPair(std::string str) {	
	int pos = str.find(": ");
	return (std::make_pair<std::string, std::string>(std::string(str, 0, pos), std::string(str, pos + 2)));
}


bool ft::hasEmptyLine(std::string const &str) {
	std::string line;
	int pos;
	int lf;

	pos = 0;
	while ((lf = str.find("\r\n", pos)) != std::string::npos) {
		line = std::string(str.begin() + pos, str.begin() + lf);
		if (line.size() == 0)
			return (true);
		pos = lf + 2;
	}
	line = std::string(str.begin() + pos, str.end());
	if (line.size() == 0)
		return (true);
	return (false);
}
