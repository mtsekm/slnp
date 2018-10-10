#ifndef SRC_SERVER_H_
#define SRC_SERVER_H_

#include "Types.h"
#include "Registry.h"

#include <string>
#include <vector>
#include <functional>
#include <Windows.h>

namespace slnp {

/**
 * This class provides an IPC server using Named Pipes
 */
class Server {
public:
	/**
	 * Constructor
	 *
	 * @param pipe name
	 */
	Server(const std::string& pipe);

	/**
	 * Destructor
	 */
	virtual ~Server();

	/**
	* Sets a factory to create objects
	*/
	void setTargetFactory(FactoryFunction factory);

	/**
	 * Runs server loop.
	 *
	 * Waits for client connection. Creates a thread for processing requests for each client.
	 */
	void run();

private:
	/// This property stores handle to pipe
	HANDLE pipe;
	/// This property stores pipe name
	std::string pipeName;

	/// This method is used for reading from a pipe synchronously
	static std::string read(HANDLE pipe);
	/// This method is used for writing to a pipe synchronously
	static bool write(HANDLE pipe, const slnp::Response& response);
	/// This method parses a request, invokes method and creates a corresponding response
	static Response handleRequest(const std::string& request);

	friend DWORD WINAPI InstanceThread(LPVOID lpvParam);
};

} /* namespace slnp */

#endif /* SRC_SERVER_H_ */
