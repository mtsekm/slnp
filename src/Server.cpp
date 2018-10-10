#include "Server.h"
#include "Server.h"
#include "Server.h"
#include "Registry.h"
#include "Logger.h"

#include <sstream>

namespace slnp {

DWORD WINAPI InstanceThread(LPVOID);

Server::Server(const std::string& pipe) :
		pipeName(pipe) {
	LOG_ENTER;
	LOG_LEAVE;
}

Server::~Server() {
	LOG_ENTER;
	LOG_LEAVE;
}

void Server::setTargetFactory(FactoryFunction factory)
{
	Registry::instance().setTargetFactory(factory);
}

void Server::run() {
	LOG_ENTER;
	bool connected = false;
	DWORD dwThreadId = 0;
	HANDLE hThread = NULL;
	while (1) {
		pipe = CreateNamedPipe(
			pipeName.c_str(),
			PIPE_ACCESS_DUPLEX,       // read/write access 
			PIPE_TYPE_MESSAGE |       // message type pipe 
			PIPE_READMODE_MESSAGE |   // message-read mode 
			PIPE_WAIT,                // blocking mode 
			PIPE_UNLIMITED_INSTANCES, // max. instances  
			BUFSIZE,                  // output buffer size 
			BUFSIZE,                  // input buffer size 
			0,                        // client time-out 
			NULL);                    // default security attribute 

		if (pipe == INVALID_HANDLE_VALUE) {
			LOGE("CreateNamedPipe failed, GLE=%d.\n", GetLastError());
			return;
		}

		LOGI("Main thread awaiting client connection on %s", pipeName.c_str());
		// Wait for the client to connect; if it succeeds, 
		// the function returns a nonzero value. If the function
		// returns zero, GetLastError returns ERROR_PIPE_CONNECTED. 
		connected = ConnectNamedPipe(pipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

		if (connected) {
			LOGD("Client connected, creating a processing thread");

			// Create a thread for this client. 
			hThread = CreateThread(
					NULL,              // no security attribute
					0,                 // default stack size
					InstanceThread,    // thread proc
					(LPVOID) pipe,     // thread parameter
					0,                 // not suspended
					&dwThreadId);      // returns thread ID

			if (hThread == NULL) {
				LOGE("CreateThread failed, GLE=%d", GetLastError());
				return;
			} else
				CloseHandle(hThread);
		} else
			CloseHandle(pipe);  // The client could not connect, so close the pipe. 
	}
	LOG_LEAVE;
}

// This routine is a thread processing function to read from and reply to a client
// via the open pipe connection passed from the main loop. Note this allows
// the main loop to continue executing, potentially creating more threads of
// of this procedure to run concurrently, depending on the number of incoming
// client connections.
DWORD WINAPI InstanceThread(LPVOID lpvParam) {
	HANDLE hHeap = GetProcessHeap();
	TCHAR* pchRequest = (TCHAR*) HeapAlloc(hHeap, 0, BUFSIZE * sizeof(TCHAR));
	TCHAR* pchReply = (TCHAR*) HeapAlloc(hHeap, 0, BUFSIZE * sizeof(TCHAR));

	DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
	BOOL fSuccess = FALSE;
	HANDLE hPipe = NULL;

	// Do some extra error checking since the app will keep running even if this
	// thread fails.

	if (lpvParam == NULL) {
		printf("\nERROR - Pipe Server Failure:\n");
		printf("   InstanceThread got an unexpected NULL value in lpvParam.\n");
		printf("   InstanceThread exitting.\n");
		if (pchReply != NULL)
			HeapFree(hHeap, 0, pchReply);
		if (pchRequest != NULL)
			HeapFree(hHeap, 0, pchRequest);
		return (DWORD) - 1;
	}

	if (pchRequest == NULL) {
		printf("\nERROR - Pipe Server Failure:\n");
		printf("   InstanceThread got an unexpected NULL heap allocation.\n");
		printf("   InstanceThread exitting.\n");
		if (pchReply != NULL)
			HeapFree(hHeap, 0, pchReply);
		return (DWORD) - 1;
	}

	if (pchReply == NULL) {
		printf("\nERROR - Pipe Server Failure:\n");
		printf("   InstanceThread got an unexpected NULL heap allocation.\n");
		printf("   InstanceThread exitting.\n");
		if (pchRequest != NULL)
			HeapFree(hHeap, 0, pchRequest);
		return (DWORD) - 1;
	}

	// Print verbose messages. In production code, this should be for debugging only.
	LOGI("InstanceThread created, receiving and processing messages");

	// The thread's parameter is a handle to a pipe object instance. 

	hPipe = (HANDLE) lpvParam;

	// Process the incoming messages
	while (1) {
		std::string str = Server::read(hPipe);
		if (!str.empty()) {
			Response response = Server::handleRequest(str);
			std::stringstream ss;
			ss << response;
			std::string responsestr = ss.str();
			if (!Server::write(hPipe, response))
				break;
		} else break;
	}

	// Flush the pipe to allow the client to read the pipe's contents 
	// before disconnecting. Then disconnect the pipe, and close the 
	// handle to this pipe instance. 

	FlushFileBuffers(hPipe);
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);

	HeapFree(hHeap, 0, pchRequest);
	HeapFree(hHeap, 0, pchReply);

	LOGI("InstanceThread terminated");
	return 1;
}

std::string Server::read(HANDLE pipe) {
	LOG_ENTER;
	bool ret;
	DWORD cbRead;
	TCHAR chBuf[BUFSIZE];
	std::string str;
	do {
		ret = ReadFile(pipe, chBuf, BUFSIZE * sizeof(TCHAR), &cbRead, NULL);
		if (!ret || cbRead == 0) {
			if (GetLastError() == ERROR_BROKEN_PIPE) {
				LOGE("InstanceThread: client disconnected", GetLastError());
			} else {
				LOGE("InstanceThread ReadFile failed, GLE=%d", GetLastError());
			}
			break;
		}

		if (!ret && GetLastError() != ERROR_MORE_DATA)
			LOGD("Got %lu bytes: '%s'", cbRead, chBuf);
		str.append(chBuf);
	} while (!ret);

	if (!ret) {
		LOGE("ReadFile from pipe failed. GLE=%d", GetLastError());
		str = "";
	}
	LOG_LEAVE;
	return str;
}

bool Server::write(HANDLE pipe, const slnp::Response& response) {
	LOG_ENTER;
	DWORD cbWritten = 0;
	std::string message = response.str();
	LOGD("sending '%s", message.c_str());
	bool ret = WriteFile(
			pipe,			// handle to pipe
			message.c_str(),	// message
			message.size() + 1,	// message length
			&cbWritten,		// number of bytes written
			NULL);			// not overlapped I/O

	if (!ret || message.size() + 1 != cbWritten) {
		LOGE("InstanceThread WriteFile failed, GLE=%d", GetLastError());
		return false;
	}
	LOG_LEAVE;
	return true;
}

Response Server::handleRequest(const std::string& request) {
	LOG_ENTER;
	Request req;
	std::stringstream ss;
	ss << request;
	ss >> req;
	LOGD("Incoming request: %s", request.c_str());

	Response response;
	if (req.getInstanceId() < 0 && req.getMethodName().empty()) {
		// create instance
		LOGD("Creating an instance of '%s', argCount: %d", req.getClassName().c_str(), req.getArgsCount());
		int id = Registry::instance().createTarget(req.getClassName(), req.getArgs());
		if (id) {
			response.setCode(slnp::ResultOK);
			response.setResult(id);
			LOGD("New %s class is created with id %d", req.getClassName().c_str(), id);
		} else {
			response.setCode(slnp::ResultFailure);
		}
	} else {
		// call member method
		IPCTarget* instance = Registry::instance().getClassInstance(req.getClassName(), req.getInstanceId());
		if (instance) {
			LOGD("Invoking %s on %s:%d", req.getMethodName().c_str(), req.getClassName().c_str(), req.getInstanceId());
			Value result;
			ResultCode ret = instance->invoker(req.getMethodName(), req.getArgs(), &result);
			response.setCode(ret);
			if (ret == slnp::ResultOK) {
				response.setResult(result);
				LOGD("Invocation is successful");
			} else {
				LOGE("Error during invocation: %s", get_result_string(ret));
			}
		} else {
			response.setCode(ResultInstanceNotFound);
			LOGE("Could not find instance of class %s", req.getClassName().c_str());
		}
	}

	LOG_LEAVE;
	return response;
}

} /* namespace slnp */


