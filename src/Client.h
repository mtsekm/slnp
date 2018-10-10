#ifndef SRC_CLIENT_H_
#define SRC_CLIENT_H_

#include "Types.h"

#include <functional>
#include <mutex>
#include <future>
#include <Windows.h>

namespace slnp {

typedef std::function<void(const Response&)> IPCFunction;

/**
 * This class provides an IPC client using Named Pipes
 */
class Client {
public:
	/**
	 * Constructor
	 *
	 * @param pipe name
	 */
	Client(const std::string& pipe);

	/**
	 * Destructor
	 */
	virtual ~Client();

	/**
	 * Initialize pipe and connect to server
	 *
	 * @return true if successful, false otherwise
	 */
	bool init();

	/**
	 * Releases resources
	 *
	 * @return true if successful, false otherwise
	 */
	bool term();

	/**
	 * Performs a synchronous request
	 *
	 * @param request to perform
	 * @return response from server
	 */
	Response call(const Request& request);

	/**
	 * Performs an asynchronous request
	 *
	 * @param request to perform
	 * @return response from server
	 */
	std::future<Response> call_async(const Request& request);

private:
	/// This flag is set to true if pipe is ready to use
	bool initialized;
	/// This property stores a handle to a named pipe
	HANDLE pipe;
	/// This mutex serialises writes
	std::mutex pipeMutex;
	/// This property stores pipe name
	std::string pipeName;

	/// Reads from pipe
	std::string read();
	
	/// Writes to pipe
	bool write(const Request& request);
};

} /* namespace slnp */

#endif /* SRC_CLIENT_H_ */
