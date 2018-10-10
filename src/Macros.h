#ifndef MACROS_H_
#define MACROS_H_

#include "Logger.h"

#include <cstring>

#define SLNP_BOOLEAN_VALUE(x) args[x].getBool()
#define SLNP_INTEGER_VALUE(x) args[x].getInt()
#define SLNP_FLOAT_VALUE(x) args[x].getFloat()
#define SLNP_STRING_VALUE(x) args[x].getString()

#define SLNP_MATCH_SIGNATURE(signature, args)										\
	if (args.size() != strlen(signature)) return slnp::ResultSignatureMismatch;		\
	for (int i = 0; i < args.size(); i++) {											\
		if (slnp::IPC_VALUE_LOOKUP_TABLE[args[i].getType()].c != signature[i])		\
			return slnp::ResultSignatureMismatch;									\
	}

#define SLNP_NEW_INSTANCE(identifier, signature, ctor)	\
	if (std::strcmp(name.c_str(), #identifier) == 0)	\
	{													\
		slnp::IPCTarget *t;	int id = -1;				\
		try {											\
			t = ctor;									\
			id = t->getRegistryID();					\
		} catch(const std::exception& e) {				\
			LOGE("Exception: %s", e.what());			\
		}												\
		LOGD("New instance %p", t);						\
		return id;										\
	}

#define SLNP_INVOKE_FN(identifier, signature, fn)		\
	if (std::strcmp(name.c_str(), #identifier) == 0)	\
	{													\
		SLNP_MATCH_SIGNATURE(signature, args)			\
		try {											\
			(void) fn;									\
		} catch(const std::exception& e) {				\
			LOGE("Exception: %s", e.what());			\
			return slnp::ResultException;				\
		}												\
		return slnp::ResultOK;							\
	}

#define SLNP_GET_BOOLEAN(identifier, signature, fn)		\
	if (std::strcmp(name.c_str(), #identifier) == 0)	\
	{													\
		SLNP_MATCH_SIGNATURE(signature, args)			\
		try {											\
			result->setBool(fn);						\
		} catch(const std::exception& e) {				\
			LOGE("Exception: %s", e.what());			\
			return slnp::ResultException;				\
		}												\
		return slnp::ResultOK;							\
	}

#define SLNP_GET_INTEGER(identifier, signature, fn)		\
	if (std::strcmp(name.c_str(), #identifier) == 0)	\
	{													\
		SLNP_MATCH_SIGNATURE(signature, args)			\
		try {											\
			result->setInt(fn);							\
		} catch(const std::exception& e) {				\
			LOGE("Exception: %s", e.what());			\
			return slnp::ResultException;				\
		}												\
		return slnp::ResultOK;							\
	}

#define SLNP_GET_FLOAT(identifier, signature, fn)		\
	if (std::strcmp(name.c_str(), #identifier) == 0)	\
	{													\
		SLNP_MATCH_SIGNATURE(signature, args)			\
		try {											\
			result->setFloat(fn);						\
		} catch(const std::exception& e) {				\
			LOGE("Exception: %s", e.what());			\
			return slnp::ResultException;				\
		}												\
		return slnp::ResultOK;							\
	}

#define SLNP_GET_STRING(identifier, signature, fn)		\
	if (std::strcmp(name.c_str(), #identifier) == 0)	\
	{													\
		SLNP_MATCH_SIGNATURE(signature, args)			\
		try {											\
			result->setString(fn);						\
		} catch(const std::exception& e) {				\
			LOGE("Exception: %s", e.what());			\
			return slnp::ResultException;				\
		}												\
		return slnp::ResultOK;							\
	}

#endif /* MACROS_H_ */
