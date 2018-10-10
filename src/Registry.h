#ifndef REGISTRY_H_
#define REGISTRY_H_

#include "IPCTarget.h"

#include <unordered_map>
#include <mutex>

namespace slnp {
typedef std::function < int(const std::string&, const std::vector<Value>&)> FactoryFunction;

/**
 * This class is used to store and map IPC enabled classes and their instances.
 */
class Registry {
public:
	/**
	 * Returns singleton instance
	 *
	 * @return singleton
	 */
	static Registry& instance();

	Registry(Registry const&) = delete;
	void operator=(Registry const&) = delete;

	/**
	* Sets a factory which creates the instances of target classes.
	*/
	void setTargetFactory(FactoryFunction factory);

	/**
	* Creates an instance of class via factory.
	*
	* @param alias of registered class
	* @param args argument list
	*
	* @return registery id of new instance
	*/
	int createTarget(const std::string& name, const std::vector<Value>& args);

	/**
	 * Returns an instance of a registered class
	 *
	 * @param alias of registered class
	 * @param index instance id
	 *
	 * @return an IPCTarget instance if found, NULL otherwise
	 */
	IPCTarget* getClassInstance(const std::string& alias, int index);

	void list();

private:
	typedef std::vector<IPCTarget*> InstanceVector;
	typedef std::unordered_map<int, IPCTarget*> InstanceMap;
	typedef std::unordered_map<std::string, InstanceMap> ClassMap;

	/// Internal id counter
	int _id;
	/// This function is used for creating instances of targets
	FactoryFunction _factory;
	/// This property stores instances created over IPC
	InstanceVector _instances;
	/// This property stores registered classes and their instances
	ClassMap _classes;
	/// This mutex serializes access to class list
	std::mutex _mutex;

	/**
	 * Constructor
	 */
	Registry();

	/**
	 * Destructor
	 */
	virtual ~Registry();

	/**
	 * Add an instance of given class from registry
	 *
	 * @param alias to class
	 * @param instance of class
	 *
	 * @return id of instance
	 */
	int add(const std::string& alias, IPCTarget* instance);

	/**
	 * Removes an instance of given class from registry
	 *
	 * @param alias to class
	 * @param instance of class
	 */
	void remove(const std::string& alias, IPCTarget* instance);

	friend class IPCTarget;
};

} /* namespace slnp */

#endif /* REGISTRY_H_ */
