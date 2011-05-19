#include "LogStream.hpp"
#include "UintStream.hpp"
#include "HuffmanStream.hpp"

namespace hdt {

StreamElements *StreamElements::getStream(std::string type)
{
	if(type=="http://purl.org/HDT/hdt#streamInteger") {
		return new UintStream();
	} else if(type=="http://purl.org/HDT/hdt#streamLog") {
		return new LogStream();
	} else if(type=="http://purl.org/HDT/hdt#streamHuffman") {
		return new HuffmanStream();
	}
	return new LogStream();
}

}
