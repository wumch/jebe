
#include "meta.hpp"
#include <iostream>
#include <zmq.h>
#include "net.hpp"

int main()
{
    void *context = zmq_init (2);
    void *frontend = zmq_socket (context, ZMQ_ROUTER);
    void *backend = zmq_socket (context, ZMQ_DEALER);

    {
    	const std::string frontend_uri(std::string("tcp://") + staging::getWanIP() + ":10010");
    	zmq_bind(frontend, frontend_uri.c_str());

    	const std::string backend_uri(std::string("tcp://") + staging::getNatIP() + ":10011");
    	zmq_bind(backend, backend_uri.c_str());

    	std::cout << "frontend:\t" << frontend_uri << std::endl
			<< "backend:\t" << backend_uri << std::endl;
    }
    zmq_device(ZMQ_QUEUE, frontend, backend);

    zmq_close(frontend);
    zmq_close(backend);
    zmq_term(context);

    return 0;
}
