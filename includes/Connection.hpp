#ifndef CONNECTION_HPP
# define CONNECTION_HPP

#include <string>

class Connection {
	private:
		int m_clientFd;
		std::string m_clientbuf;
		std::string m_serverbuf;
	public:
		explicit Connection(int fd);
		~Connection();
		int getSocket() const;
		void readData();
		void writeData();
		void processRequest();
};

#endif
