#ifndef TYPES_H_
#define TYPES_H_

#include <iostream>
#include <cstring>
#include <vector>

namespace slnp {

const int BUFSIZE = 512;

/**
 * This enum is used to return an error code from invoked method
 */
enum ResultCode {
	ResultOK,               	//!< Call was successful and response contains result of operation
	ResultFailure,          	//!< Operation failed due to a generic error
	ResultFactoryNotAvailable,	//!< A factory is not registered in Registry
	ResultClassNotAvailable,	//!< Class is not registered in object factory
	ResultInstanceNotFound,		//!< Instance is not found in Registry
	ResultMethodNotFound,		//!< Method is not registered for an IPC call
	ResultSignatureMismatch,	//!< Argument types do not match
	ResultException,			//!< An exception was thrown during method invocation
	ResultMAX,					//!< ResultMAX Result is not valid
};

const char* get_result_string(ResultCode res);

/**
 * This enum specifies types that are supported by this library.
 */
enum ValueType {
	BOOLEAN,        	//!< Denotes a boolean value
	INTEGER,        	//!< Denotes an integer value
	FLOAT,          	//!< Denotes a double value
	STRING,         	//!< Denotes a string value
	IPCValueType_MAX	//!< IPCValueType_MAX Value is not valid
};

/**
 * This class is used for storing and transferring values over IPC.
 */
class Value {
public:
	/**
	 * Constructor.
	 */
	Value();

	/**
	 * Constructor, sets boolean value
	 *
	 * @param b boolean
	 */
	Value(bool b);

	/**
	 * Constructor, sets integer value
	 *
	 * @param i integer
	 */
	Value(int i);

	/**
	 * Constructor, sets float value
	 *
	 * @param f float
	 */
	Value(double f);

	/**
	 * Constructor, sets string value
	 *
	 * @param s string
	 */
	Value(const char* s);

	/**
	 * Copy constructor
	 *
	 * @param obj to copy from
	 */
	Value(const Value &obj);

	/**
	 * Destructor, releases resources
	 */
	~Value();

	bool getBool() const;
	void setBool(bool b);

	double getFloat() const;
	void setFloat(double f);

	int getInt() const;
	void setInt(int i);

	const char* getString() const;
	size_t getStringSize() const;
	void setString(const char* s);

	ValueType getType() const;

	Value& operator=(const Value &rhs);
	std::string str() const;

private:
	/// This property stores the type of stored value
	ValueType type;
	/// This property stores value
	union {
		bool b;
		int i;
		double f;
		const char* s;
	} u;

	/// Release resources
	void release();
};

/**
 * Lookup table matching ipc value type with a character.
 */
const struct {
	ValueType type;
	char c;
} IPC_VALUE_LOOKUP_TABLE[IPCValueType_MAX] = { { BOOLEAN, 'b' }, { INTEGER, 'i' }, { FLOAT, 'f' }, { STRING, 's' } };

/**
 * This class is used for serializing an IPC request during transfer from client to server.
 */
class Request {
public:
	/**
	 * Constructor, invalid request.
	 */
	Request();

	/**
	* Constructor, use for creating class instances.
	*/
	Request(const std::string& className);

	/**
	 * Constructor
	 *
	 * @param className
	 * @param instanceId
	 * @param funcName
	 */
	Request(const std::string& className, int instanceId, const std::string& method);

	/**
	 * Constructor
	 *
	 * @param className
	 * @param instanceId
	 * @param methodName
	 * @param args
	 */
	Request(const std::string& className, int instanceId, const std::string& methodName, const std::vector<Value>& args);

	size_t getArgsCount() const;
	const std::vector<Value>& getArgs() const;
	void setArgs(const std::vector<Value>& args);
	Request& addArg(const Value& arg);

	const std::string& getClassName() const;
	void setClassName(const std::string& className);

	const std::string& getMethodName() const;
	void setMethodName(const std::string& methodName);

	int getInstanceId() const;
	void setInstanceId(int instanceId);

	bool isValid();

	std::string str() const;

private:
	/// This property stores an alias to a registered class
	std::string className;
	/// This property stores instance index of class
	int instanceID;
	/// This property stores member method name
	std::string methodName;
	/// This property stores the list of arguments of member method
	std::vector<Value> args;
};

/**
 * This class is used for serializing an IPC response during transfer from server to client.
 */
class Response {
public:
	/**
	 * Constructor
	 *
	 * By default result code is set to ResultMAX.
	 */
	Response();

	/**
	 * Constructor, sets code and result.
	 *
	 * @param code the result code of invocation
	 * @param result return value of invoked method if any.
	 */
	Response(ResultCode code, const Value& result);

	ResultCode getCode() const;
	void setCode(ResultCode code);

	const Value& getResult() const;
	void setResult(const Value& result);

	std::string str() const;

private:
	/// This property stores the result code of invocation
	ResultCode code;
	/// This property stores the return value of invoked method
	Value result;
};

std::istream&
operator>>(std::istream& is, slnp::Value& obj);

std::ostream&
operator<<(std::ostream& os, const slnp::Value& obj);

std::istream&
operator>>(std::istream& is, Request& obj);

std::ostream&
operator<<(std::ostream& os, const Request& obj);

std::istream&
operator>>(std::istream& is, Response& obj);

std::ostream&
operator<<(std::ostream& os, const Response& obj);

} /* namespace slnp */

#endif /* TYPES_H_ */
