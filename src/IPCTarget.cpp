#include "IPCTarget.h"

#include "Logger.h"
#include "Macros.h"
#include "Registry.h"

namespace slnp {

IPCTarget::IPCTarget(const std::string& alias) :
		_alias(alias) {
	LOG_ENTER;
	_registryID = Registry::instance().add(_alias, this);
	LOG_LEAVE;
}

IPCTarget::~IPCTarget() {
	LOG_ENTER;
	Registry::instance().remove(_alias, this);
	LOG_LEAVE;
}

int IPCTarget::getRegistryID() const {
	return _registryID;
}

} /* namespace slnp */
