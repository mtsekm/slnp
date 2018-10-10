#include "Client.h"
#include "Logger.h"

#include <sstream>

namespace slnp {

Client::Client(const std::string& pipe) :
		pipeName(pipe), initialized(false) {
	LOG_ENTER;
	LOG_LEAVE;
}

Client::~Client() {
	LOG_ENTER;
	LOG_LEAVE;
}

Response Client::call(const Request& request) {
	LOG_ENTER;
	Response r;
	if (!initialized) {
		LOGE("Client is not initalized!");
		return r;
	}
	// send request, read response
	pipeMutex.lock();
	write(request);
	std::stringstream ress;
	ress << read();
	ress >> r;
	pipeMutex.unlock();
	LOG_LEAVE;
	return r;
}

std::future<Response> Client::call_async(const Request& request) {
	return std::async(&slnp::Client::call, this, request);
}

bool Client::init() {
	LOG_ENTER;
	DWORD dwMode = PIPE_READMODE_MESSAGE;
	while (1) {
		pipe = CreateFile(
				pipeName.c_str(),
				GENERIC_READ | GENERIC_WRITE,
				0,              // no sharing
				NULL,           // default security attributes
				OPEN_EXISTING,  // opens existing pipe
				0,              // default attributes
				NULL);          // no template file

		// Break if the pipe handle is valid. 
		if (pipe != INVALID_HANDLE_VALUE)
			break;

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 
		if (GetLastError() != ERROR_PIPE_BUSY) {
			LOGE("Could not open pipe. . GLE=%d", GetLastError());
			return false;
		}

		// All pipe instances are busy, so wait for 20 seconds. 
		if (!WaitNamedPipe(pipeName.c_str(), 20000)) {
			LOGI("Could not open pipe: 20 second wait timed out.");
			return false;
		}
	}

	// The pipe connected; change to message-read mode. 
	bool ret = SetNamedPipeHandleState(pipe, &dwMode, NULL, NULL);
	if (!ret) {
		LOGE("SetNamedPipeHandleState failed. GLE=%d", GetLastError());
		return false;
	}

	initialized = true;
	LOGD("Connected");
	LOG_LEAVE;
	return true;
}

bool Client::term() {
	LOG_ENTER;
	if (!initialized) {
		LOGE("Client is not initalized!");
		return false;
	}
	CloseHandle(pipe);
	LOG_LEAVE;
	return true;
}

std::string Client::read() {
	LOG_ENTER;
	std::string str;
	if (!initialized) {
		LOGE("Client is not initalized!");
		return str;
	}

	bool ret;
	DWORD cbRead;
	TCHAR chBuf[BUFSIZE];
	do {
		ret = ReadFile(pipe, chBuf, BUFSIZE * sizeof(TCHAR), &cbRead, NULL);
		if (!ret && GetLastError() != ERROR_MORE_DATA)
			break;
		LOGD("Got %lu bytes: '%s'", cbRead, chBuf);
		str.append(chBuf);
	} while (!ret);

	if (!ret) {
		LOGE("ReadFile from pipe failed. GLE=%d", GetLastError());
	}
	LOG_LEAVE;
	return str;
}

bool Client::write(const Request& request) {
	LOG_ENTER;
	DWORD cbWritten = 0;
	std::string message = request.str();
	LOGD("sending '%s", message.c_str());
	bool ret = WriteFile(
			pipe,				// pipe handle
			message.c_str(),	// message
			message.size() + 1,	// message length
			&cbWritten,			// bytes written
			NULL);				// not overlapped

	if (!ret) {
		LOGE("WriteFile to pipe failed. GLE=%d", GetLastError());
		return false;
	}
	LOG_LEAVE;
	return true;
}

} /* namespace slnp */

