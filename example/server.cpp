#include <Server.h>
#include <Macros.h> // required for SLNP_NEW_INSTANCE

#include "Sample.h" // sample IPCTarget based class

// 
int factory(const std::string& name, const std::vector<slnp::Value>& args) {
	SLNP_NEW_INSTANCE(Sample, "s", new Sample(SLNP_STRING_VALUE(0)));
	return NULL;
}

int main(int argc, char **argv) {	
	// create some IPCTarget objects on server
	// these are automatically registered upon creation
	// their IPC instance IDs are 0, 1 and 2
	Sample s1("test1");
	Sample s2("test2");
	Sample s3("test3");

	// just prints tracked objects for conformation
	slnp::Registry::instance().list();
	
	// Setup server & run
	// A target factory function must be set for creating objects
	slnp::Server s("\\\\.\\pipe\\slnp-ex");
	s.setTargetFactory(factory);
	s.run();

	return 0;
}