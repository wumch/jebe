
#include "../predef.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <boost/numeric/ublas/vector_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <msgpack.hpp>
#include "../document.hpp"
#include "filesystem.hpp"

int main(int argc, char* argv[]) {
        // serializes multiple objects using msgpack::packer.
//        msgpack::sbuffer buffer;
//
//        msgpack::packer<msgpack::sbuffer> pk(&buffer);
//        pk.pack(std::string("Log message ... 1"));
//        pk.pack(std::string("Log message ... 2"));
//        pk.pack(std::string("Log message ... 3"));

        // deserializes these objects using msgpack::unpacker.
        msgpack::unpacker pac;

        uint64_t fsize = staging::filesize(argv[1]);
        char* buf = new char[fsize + 1];
        buf[fsize] = 0;
        FILE* fp = fopen(argv[1], "rb");
        while (!feof(fp))
        {
        	size_t chunks = fread(buf + ftell(fp), fsize, 1, fp);
//        	if (chunks != 1)
//        	{
//        		CS_DIE("IO-error:" << ferror(fp) << ", current pos:" << ftell(fp));
//        	}
        }
        std::cout << "file-size:" << fsize << ", already read:" << ftell(fp) << std::endl;

        // feeds the buffer.
        pac.reserve_buffer(fsize);
        memcpy(pac.buffer(), buf, fsize);
        pac.buffer_consumed(fsize);

        // now starts streaming deserialization.
        msgpack::unpacked result;
        while(pac.next(&result)) {
            std::cout << result.get() << std::endl;
        }

        // results:
        // $ g++ stream.cc -lmsgpack -o stream
        // $ ./stream
        // "Log message ... 1"
        // "Log message ... 2"
        // "Log message ... 3"
}
