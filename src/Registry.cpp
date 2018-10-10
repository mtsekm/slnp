#include "Registry.h"
#include "Logger.h"

#include <iostream>
#include <algorithm>

namespace slnp {

Registry& Registry::instance() {
	static Registry instance;
	return instance;
}

void Registry::setTargetFactory(FactoryFunction f)
{
	LOG_ENTER;
	_factory = f;
	LOG_LEAVE;
}

int Registry::createTarget(const std::string& name, const std::vector<Value>& args) {
	IPCTarget* target = NULL;
	LOG_ENTER;
	int id = -1;
	if (_factory) {
		id = _factory(name, args);
		if (target) {
			_mutex.lock();
			_instances.push_back(target);
			_mutex.unlock();
			LOGD("Class '%s:%p' is created!", name.c_str(), target);
		}		
	} else 
		LOGE("No factory is set!");
	LOG_LEAVE;
	return id;
}

Registry::Registry() :
	_id(0) {
	LOG_ENTER;
	LOG_LEAVE;
}

Registry::~Registry() {
	LOG_ENTER;
	_mutex.lock();
	// delete class instances created over IPC only
	for (InstanceVector::iterator it = _instances.begin(); it != _instances.end(); ++it) {
		delete *it;
	}
	_mutex.unlock();
	LOG_LEAVE;
}

IPCTarget* Registry::getClassInstance(const std::string& className, int id) {
	LOG_ENTER;
	IPCTarget* instance = NULL;
	InstanceMap m;
	try {
		m = _classes.at(className);
	} catch (const std::out_of_range& oor) {
		(void) oor;
		LOGE("Class '%s' is not registered!", className.c_str());
		return instance;
	}
	try {
		instance = m.at(id);
	} catch (const std::out_of_range& oor) {
		(void) oor;
		LOGE("Instance id '%d' is not found!", id);
	}
	if (instance)
		LOGD("Found instance %p of class '%s'", instance, className.c_str());
	LOG_LEAVE;
	return instance;
}

void Registry::list() {
	LOG_ENTER;
	for (auto& c : _classes) {
		std::cout << "Class '" << c.first << "' has " << c.second.size() << " instances:" << std::endl;
		for(auto& i: c.second) {
			std::cout << i.first << " - " << i.second << std::endl;
		}
	}
	LOG_LEAVE;
}

int slnp::Registry::add(const std::string& className, IPCTarget* instance) {
	LOG_ENTER;
	_mutex.lock();
	instance->_registryID = _id;
	_classes[className][_id] = instance;
	LOGD("Instance %p is registered for class '%s' with id %d.", instance, className.c_str(), instance->_registryID);
	_id++;
	_mutex.unlock();
	LOG_LEAVE;
	return instance->_registryID;
}

void slnp::Registry::remove(const std::string& className, IPCTarget* instance) {
	LOG_ENTER;
	_mutex.lock();
	for (auto c : _classes) {
		if (c.first == className) {
			InstanceMap classInstances = c.second;
			for (auto k : classInstances) {
				if (k.second == instance) {
					LOGD("Removed instance: %p from class '%s'", instance, className.c_str());
					classInstances.erase(k.first);
				}
			}
		}
	}
	_mutex.unlock();
	LOG_LEAVE;
}

} /* namespace slnp */

