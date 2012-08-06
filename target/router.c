
#include <stdint.h>
#include <zmq.h>

int main (void)
{
    void *context = zmq_init (1);

    //  Socket facing clients
    void *frontend = zmq_socket (context, ZMQ_ROUTER);
//    int64_t hwm = 10 << 20;
//    zmq_setsockopt(frontend, ZMQ_HWM, &hwm, sizeof(hwm));
    zmq_bind (frontend, "tcp://211.154.172.172:10010");

    //  Socket facing services
    void *backend = zmq_socket (context, ZMQ_DEALER);
    zmq_bind (backend, "tcp://192.168.88.1:10011");

    //  Start built-in device
    zmq_device (ZMQ_QUEUE, frontend, backend);

    //  We never get here…
    zmq_close (frontend);
    zmq_close (backend);
    zmq_term (context);
    return 0;
}
