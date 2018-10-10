#include <Client.h>
#include <iostream>

int main(int argc, char **argv) {
	slnp::Client c("\\\\.\\pipe\\slnp-ex");
	if (!c.init()) {
		std::cerr << "Error: could not initialize client" << std::endl;
		return 1;
	}

	std::cout << "*** create an instance of Sample class" << std::endl;
	int instance_id;
	slnp::Response res;
	slnp::Request req0("Sample");
	req0.addArg("RemoteSample1"); // pass an argument to constructor
	res = c.call(req0);
	if (res.getCode() == slnp::ResultOK) {
		instance_id = res.getResult().getInt();
	} else {
		std::cerr << "Error: " << get_result_string(res.getCode()) << std::endl;
		return res.getCode();
	}

	std::cout << "*** invoke the sumf() method of new object" << std::endl;
	slnp::Request req1("Sample", instance_id, "sumf");
	req1.addArg(1.2).addArg(3.4); // pass two floats
	res = c.call(req1);
	if (res.getCode() == slnp::ResultOK) {
		std::cerr << "server response: '" << res.getResult() << "'" << std::endl;
	} else {
		std::cerr << "Error: " << get_result_string(res.getCode()) << std::endl;
		return res.getCode();
	}

	std::cout << "*** invoking two long operations async..." << std::endl;
	auto f1 = c.call_async(slnp::Request("Sample", instance_id, "heavy"));
	auto f2 = c.call_async(slnp::Request("Sample", instance_id, "heavy"));	
	res = c.call(slnp::Request("Sample", instance_id, "sumf"));
	std::cout << "*** try to sync invoke a method with bad arguments while server is busy" << std::endl;
	if (res.getCode() == slnp::ResultOK) {
		std::cerr << "server response: '" << res.getResult() << "'" << std::endl;
	}
	else {
		std::cerr << "Error: " << get_result_string(res.getCode()) << std::endl;
	}

	std::cout << "*** get the result of long operations" << std::endl;
	res = f1.get();
	if (res.getCode() == slnp::ResultOK) {
		std::cerr << "server response f1: '" << res.getResult() << "'" << std::endl;
	}
	else {
		std::cerr << "Error: " << get_result_string(res.getCode()) << std::endl;
		return res.getCode();
	}
	res = f2.get();
	if (res.getCode() == slnp::ResultOK) {
		std::cerr << "server response f2: '" << res.getResult() << "'" << std::endl;
	}
	else {
		std::cerr << "Error: " << get_result_string(res.getCode()) << std::endl;
		return res.getCode();
	}

	std::cout << "client exit 0" << std::endl;
	return 0;
}
