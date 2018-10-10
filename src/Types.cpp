#include "Types.h"
#include "Types.h"
#include "Types.h"
#include "Logger.h"

#include <sstream>

namespace slnp {

const char* get_result_string(ResultCode res) {
	switch (res) {
	case ResultOK:
		return ("OK");
	case ResultFailure:
		return ("generic error");
	case ResultFactoryNotAvailable:
		return ("factory not available");
	case ResultClassNotAvailable:
		return ("class not available");
	case ResultInstanceNotFound:
		return ("instance not found");
	case ResultMethodNotFound:
		return ("method not found");
	case ResultSignatureMismatch:
		return ("argument mismatch");
	case ResultException:
		return ("exception");
	default:
		return ("unknown");
	}
}

/**************************************************************************************************************
 * Value
 **************************************************************************************************************/
Value::Value() :
		type(BOOLEAN) {
	u.b = false;
}

Value::Value(bool b) :
		type(BOOLEAN) {
	u.b = b;
}

Value::Value(int i) :
		type(INTEGER) {
	u.i = i;
}

Value::Value(double f) :
		type(FLOAT) {
	u.f = f;
}

Value::Value(const char* s) :
		type(STRING) {
	u.s = _strdup(s);
	if (u.s == NULL)
		LOGE("Insufficient memory");
}

Value::Value(const Value &obj) :
		type(obj.type) {
	if (type == BOOLEAN)
		u.b = obj.u.b;
	if (type == INTEGER)
		u.i = obj.u.i;
	if (type == FLOAT)
		u.f = obj.u.f;
	if (type == STRING) {
		u.s = _strdup(obj.u.s);
		if (u.s == NULL)
			LOGE("Insufficient memory");
	}
}

Value::~Value() {
	release();
}

bool Value::getBool() const {
	return u.b;
}

void Value::setBool(bool b) {
	release();
	type = BOOLEAN;
	u.b = b;
}

double Value::getFloat() const {
	return u.f;
}

void Value::setFloat(double f) {
	release();
	type = FLOAT;
	u.f = f;
}

int Value::getInt() const {
	return u.i;
}

void Value::setInt(int i) {
	release();
	type = INTEGER;
	u.i = i;
}

const char* Value::getString() const {
	return u.s;
}

size_t Value::getStringSize() const {
	return strlen(u.s);
}

void Value::setString(const char* s) {
	release();
	type = STRING;
	u.s = _strdup(s);
	if (u.s == NULL)
		LOGE("Insufficient memory");
}

ValueType Value::getType() const {
	return type;
}

Value& Value::operator=(const Value &obj) {
	type = obj.type;
	if (type == BOOLEAN)
		u.b = obj.u.b;
	if (type == INTEGER)
		u.i = obj.u.i;
	if (type == FLOAT)
		u.f = obj.u.f;
	if (type == STRING) {
		u.s = _strdup(obj.u.s);
		if (u.s == NULL)
			LOGE("Insufficient memory");
	}
	return *this;
}

std::string Value::str() const {	
	if (type == BOOLEAN)
		return std::to_string(u.b);
	if (type == INTEGER)
		return std::to_string(u.i);
	if (type == FLOAT)
		return std::to_string(u.f);
	if (type == STRING)
		return u.s;
	return "";
}

void Value::release() {
	if (type == STRING) {
		free((void*) u.s);
		u.s = NULL;
	}
}

/**************************************************************************************************************
 * Request
 **************************************************************************************************************/
Request::Request() :
		instanceID(-1) {
}

Request::Request(const std::string & c) :
		className(c), instanceID(-1) {
}

Request::Request(const std::string& c, int i, const std::string& m) :
		className(c), instanceID(i), methodName(m) {
}

Request::Request(const std::string& c, int i, const std::string& m, const std::vector<Value>& a) :
		className(c), instanceID(i), methodName(m), args(a) {
}

Request& Request::addArg(const Value& arg) {
	args.push_back(arg);
	return *this;
}

size_t Request::getArgsCount() const {
	return args.size();
}

const std::vector<Value>& Request::getArgs() const {
	return args;
}

void Request::setArgs(const std::vector<Value>& args) {
	this->args = args;
}

const std::string& Request::getClassName() const {
	return className;
}

void Request::setClassName(const std::string& className) {
	this->className = className;
}

const std::string& Request::getMethodName() const {
	return methodName;
}

void Request::setMethodName(const std::string& methodName) {
	this->methodName = methodName;
}

int Request::getInstanceId() const {
	return instanceID;
}

void Request::setInstanceId(int instanceId) {
	instanceID = instanceId;
}

bool Request::isValid() {
	if (className.empty() || (instanceID < 0) || methodName.empty())
		return false;
	return true;
}

std::string Request::str() const
{
	std::stringstream ss;
	ss << *this;
	std::string str = ss.str();
	return str;
}

/**************************************************************************************************************
 * Response
 **************************************************************************************************************/
Response::Response() :
		code(ResultMAX) {
}

Response::Response(ResultCode c, const Value& r) :
		code(c), result(r) {
}

ResultCode Response::getCode() const {
	return code;
}

void Response::setCode(ResultCode code) {
	this->code = code;
}

const Value& Response::getResult() const {
	return result;
}

void Response::setResult(const Value& result) {
	this->result = result;
}

std::string Response::str() const
{
	std::stringstream ss;
	ss << *this;
	std::string str = ss.str();
	return str;
}

/**************************************************************************************************************
 * Streams
 **************************************************************************************************************/
#define DELIMITER "|"

std::istream&
operator>>(std::istream& is, Value& obj) {
	char c;
	is >> c;
	is.ignore(1);
	if (c == IPC_VALUE_LOOKUP_TABLE[BOOLEAN].c) {
		bool b;
		is >> b;
		obj.setBool(b);
	} else if (c == IPC_VALUE_LOOKUP_TABLE[INTEGER].c) {
		int i;
		is >> i;
		obj.setInt(i);
	} else if (c == IPC_VALUE_LOOKUP_TABLE[FLOAT].c) {
		double f;
		is >> f;
		obj.setFloat(f);
	} else if (c == IPC_VALUE_LOOKUP_TABLE[STRING].c) {
		int len;
		is >> len;
		char* s = (char*) malloc(sizeof(char) * (len + 1));
		is.get(s, len + 1);
		obj.setString(s);
		free(s);
	}
	return is;
}

std::ostream&
operator<<(std::ostream& os, const Value& obj) {
	if (obj.getType() == BOOLEAN)
		os << IPC_VALUE_LOOKUP_TABLE[BOOLEAN].c << DELIMITER << obj.getBool();
	else if (obj.getType() == INTEGER)
		os << IPC_VALUE_LOOKUP_TABLE[INTEGER].c << DELIMITER << obj.getInt();
	else if (obj.getType() == FLOAT)
		os << IPC_VALUE_LOOKUP_TABLE[FLOAT].c << DELIMITER << obj.getFloat();
	else if (obj.getType() == STRING)
		os << IPC_VALUE_LOOKUP_TABLE[STRING].c << DELIMITER << obj.getStringSize() << DELIMITER << obj.getString();
	return os;
}

std::istream&
operator>>(std::istream& is, Request& obj) {
	int size;
	is >> size;
	is.ignore(1);

	char *className = (char*)malloc(sizeof(char) * (size + 1));
	is.get(className, size + 1);
	obj.setClassName(className);
	free(className);
	is.ignore(1);

	int instance;
	is >> instance;
	obj.setInstanceId(instance);
	is.ignore(1);

	is >> size;
	is.ignore(1);
	if (size) {
		char *methodName = (char*)malloc(sizeof(char) * (size + 1));
		is.get(methodName, size + 1);
		obj.setMethodName(methodName);
		free(methodName);
		is.ignore(1);
	}

	int argCount;
	is >> argCount;
	is.ignore(1);
	for (int i = 0; i < argCount; i++) {
		Value value;
		is >> value;
		obj.addArg(value);
		is.ignore(1);
	}
	return is;
}

std::ostream&
operator<<(std::ostream& os, const Request& obj) {
	os << obj.getClassName().size() << DELIMITER << obj.getClassName().c_str() << DELIMITER;
	os << obj.getInstanceId() << DELIMITER;
	if (obj.getMethodName().empty())
		os << 0 << DELIMITER;
	else
		os << obj.getMethodName().size() << DELIMITER << (obj.getMethodName().empty() ? " " : obj.getMethodName().c_str()) << DELIMITER;
	os << obj.getArgsCount() << DELIMITER;
	for (auto& a : obj.getArgs())
		os << a << DELIMITER;
	return os;
}

std::istream&
operator>>(std::istream& is, Response& obj) {
	int code;
	is >> code;
	obj.setCode((ResultCode) code);
	is.ignore(1);
	Value result;
	is >> result;
	obj.setResult(result);
	return is;
}

std::ostream&
operator<<(std::ostream& os, const Response& obj) {
	os << obj.getCode() << DELIMITER << obj.getResult();
	return os;
}

} /* namespace slnp */
