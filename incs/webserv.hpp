#ifndef WEBSERV_HPP
# define WEBSERV_HPP

/* header CPP */
# include <iostream>
# include <algorithm>
# include <vector>
# include <map>
# include <stack>
# include <ctime>
# include <cstdio>
# include <queue>
# include <string>
# include <fstream>
# include <utility>

# include <ctime>

/* header C */
# include <unistd.h>
# include <fcntl.h>
# include <signal.h>
#include <string.h>
# include <sys/stat.h>
#include <sys/wait.h>
# include <dirent.h>
# include <cstdio>

/* network C */
# include <sys/socket.h>
# include <netinet/in.h>

/* type C */
# include <sys/stat.h>
# include <sys/types.h>
# include <stddef.h>

/* macro */
# define DEFAULT_CONF "./sample.conf"

# define RECV_BUFFER_SIZE 65536
# define ERR_RECV -1
# define WAIT_RECV 0
# define ALL_RECV 1

# define SEND_BUFFER_SIZE 65536
# define ERR_SEND -1
# define WAIT_SEND 0
# define ALL_SEND 1

# define CGI_BUFFER_SIZE 2048

/* color */
# define RESET		"\033[0m"
# define RED		"\033[31m"
# define YELLOW		"\033[33m"

/* libft */
/* enum */
enum LogLevel {
	Log,
	Warning,
	Error
};

namespace ft {
	/* log */
	void Log(LogLevel lv, std::string const &log);

	/* base64 */
	namespace base64 {
		static std::string alphabet =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789+/";

		std::string encode(std::string const &str);
		std::string decode(std::string const &str);
	};

	/* class, struct */
	struct fds {
		fd_set read;
		fd_set write;
		fd_set except;
		timeval timeout;
	};

	/* func */
	bool		isspace(char c);
	int			atoi(char *str);
	int			htod(char *str);
	int			index(char c, const char *arr);
	void		*memset(void *src, int val, size_t size);
	size_t		strlcpy(char *dst, char *src, size_t size);
	int			tolower(int c);

	void trim_space(std::string &str);
	void trim_chunked(std::string &str);

	int getLength(std::string const &buffer);
	bool getChunked(std::string const &buffer);

	std::string	to_string(int n);

	/* fd_set */
	void		fd_zero(::fd_set *fds);
	void		fd_set(int fd, ::fd_set *fds);
	void		fd_sets(int fd, fds *fds);
	void		fd_clr(int fd, ::fd_set *fds);
	void		fd_clrs(int fd, fds *fds);
	bool		fd_isset(int fd, ::fd_set *fds);

	/* key, value */
	std::pair<std::string, std::string> headerPair(std::string str);

	/* template */
	template <typename Iterator>
	void		transform(Iterator first, Iterator last, int (*func)(int)) {
		while (first != last) {
			*first = func(*first);
			++first;
		}
	}
}

#endif
