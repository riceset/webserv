#include "PollWrapper.hpp"

int main() {
	std::vector<int> ports; // ports = config.get_listener();
	ports.push_back(80);
	ports.push_back(8080);
	std::vector<Listener> listeners;
	for (std::vector<int>::iterator i = ports.begin(); i != ports.end(); ++i)
	{
		Listener listener(*i);
		listeners.push_back(listener);
	}
	PollWrapper pw(listeners);

	for (;;) {
		std::cout << std::endl << "Waiting for event" << std::endl;
		pw.wait();

		int event_num = pw.getEventNumber();
		for (int i = 0; i < event_num; i++) {
			if (pw.is_listener(i))
			{
				std::cout << "Accepting connection" << std::endl;
				try {
					pw.accept(i);
				}
				catch (std::exception &e) {
					std::cerr << e.what() << std::endl;
					exit(EXIT_FAILURE);
				}
				break;
			}
			else if (pw.is_pollin_event(i))
			{
				std::cout << "Reading from connection" << std::endl;
				try {
					pw.read(i);
				}
				catch (std::exception &e) {
					std::cerr << e.what() << std::endl;
					exit(EXIT_FAILURE);
				}
				break;
			}
			else if (pw.is_pollout_event(i))
			{
				std::cout << "Writing to connection" << std::endl;
				try {
					pw.write(i);
				}
				catch (std::exception &e) {
					std::cerr << e.what() << std::endl;
					exit(EXIT_FAILURE);
				}
				break;
			}
		}
	}
}