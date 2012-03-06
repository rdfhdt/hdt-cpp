#include "LogStream.hpp"
#include "LogStream2.hpp"
#include "UintStream.hpp"
#include "HuffmanStream.hpp"
#include "WaveletStream.hpp"

#include <HDTVocabulary.hpp>

namespace hdt {

StreamElements *StreamElements::getStream(std::string type)
{
	if(type==HDTVocabulary::STREAM_TYPE_INTEGER) {
		return new UintStream();
	} else if(type==HDTVocabulary::STREAM_TYPE_LOG2) {
		return new LogStream2();
	} else if(type==HDTVocabulary::STREAM_TYPE_HUFFMAN) {
		return new HuffmanStream();
	} else if(type==HDTVocabulary::STREAM_TYPE_WAVELET) {
		return new WaveletStream();
	}
	return new LogStream();
}

}
