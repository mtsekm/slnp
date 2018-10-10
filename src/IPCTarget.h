#ifndef IPCTARGET_H_
#define IPCTARGET_H_

#include "Types.h"

#include <vector>
#include <string>

namespace slnp {

/**
 * \brief This class provides a simple interface to enable remote method invocation via Registry.
 *
 * Derived classes must implement invoker method and use provided macros, see Macros.h
 */
class IPCTarget {
public:
	/**
	 * Constructor.
	 *
	 * \brief Adds this class to IPCRegistry with given alias.
	 *
	 * @param alias for child class
	 */
	IPCTarget(const std::string& alias);

	/**
	 * Destructor.
	 */
	virtual ~IPCTarget();

	/**
	 * Invokes a member method with given arguments.
	 * \warning You must keep the prototype of invoker as is, including argument names, in your derived classes.
	 *
	 * @param name of method
	 * @param args argument list
	 * @param result will store return value if call is successful
	 *
	 * @return Result_OK if successful
	 */
	virtual ResultCode invoker(const std::string& name, const std::vector<Value>& args, Value* result) = 0;

	/**
	* Returns registry id.
	*/
	int getRegistryID() const;

private:
	int _registryID;
	//! This property stores alias for class
	std::string _alias;

	friend class Registry;
};

} /* namespace slnp */

#endif /* IPCTARGET_H_ */
