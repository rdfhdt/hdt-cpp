#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <tuple>
#include <algorithm>
#include <HDTManager.hpp>
#include "../src/hdt/HDTFactory.hpp"

using namespace hdt;

/*
 * Create a HDT file from the input nt_file_path according to config_file_path
 */
int
create_hdt_file(const std::string& nt_file_path, const std::string& config_file_path,
		const std::string& hdt_file_path)
{
	int error = 0;
	std::string base_uri_str = "http://example.org/test";
	HDTSpecification spec_map(config_file_path);
	try {
		// Create a HDT file and use specifications
		HDT *hdt = HDTManager::generateHDT(
				nt_file_path.c_str(), base_uri_str.c_str(), NTRIPLES, spec_map);
		// Save to disk
		hdt->saveToHDT(hdt_file_path.c_str());
		delete hdt;
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		error = 1;
	}
	return error;
}

/*
 * Get a propery (subject, predicate) from a HDT header
 */
std::string
get_header_property(const std::string& hdt_file_path, const std::string& subject,
		const std::string& predicate)
{
	std::string element("");
	// Open HDT file
	std::istream *in = new ifstream(hdt_file_path.c_str(), ios::binary);
	if (! in ->good() )
	{
		throw std::runtime_error("Couldn't read header");
	}
	else {
		ControlInformation controlInformation;
		controlInformation.load(*in);
		controlInformation.load(*in);
		// Get header
		Header *header = HDTFactory::readHeader(controlInformation);
		header->load(*in, controlInformation);
		// Find if header has subject and predicate
		IteratorTripleString *it = header->search(subject.c_str(), predicate.c_str(),"");
		if ( it ->hasNext() )
		{
			// Get object (value)
			element = it ->next()->getObject();
		}
		delete it;
		delete header;
	}
	if ( element.size() <= 0)
	{
		throw std::runtime_error("No property found");
	}
	return element;
}

template<typename Out>
void
split(const std::string &s, char delim, Out result)
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		*(result++) = item;
	}
}

/*
 * Split a string into a vector according to delim
 */
std::vector<std::string>
split(const std::string &s, char delim)
{
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}

/*
 * Queries for the HDT header.
 * Each query is a property in config file and its counterpart in the HDT
 * header.
 */
std::map<std::string, std::tuple<std::string, std::string>> queries =
{
	{ "dictionary.type", std::make_tuple("_:dictionary", "<http://purl.org/dc/terms/format>")},
	{ "dict.block.size", std::make_tuple("_:dictionary", "<http://purl.org/HDT/hdt#dictionaryblockSize>")},
	{ "triples.type", std::make_tuple("_:triples", "<http://purl.org/dc/terms/format>")},
	{ "triplesOrder", std::make_tuple("_:triples", "<http://purl.org/HDT/hdt#triplesOrder>")}
};

/*
 * Given a HDT file and a config file, check whether the HDT header actually
 * has the same properties defined in the config file
 */
int
check_header(const std::string& hdt_file_path, const std::string& config_file_path)
{
	int error = 0;
	std::ifstream config_file;
	std::string line;
	// For a given config file
	config_file.open( config_file_path.c_str() );
	if ( config_file.is_open() )
	{
		// Get each configuration line
		while( std::getline(config_file, line) )
		{
			// Find property and value
			std::vector<std::string> elems = split(line, ' ');
			std::string first = elems.front();
			std::string last = elems.back();
			// If property need to be checked
			if ( queries.find( first ) != queries.end() )
			{
				// Check that the HDT header has same value for this property
				std::string subject, predicate;
				std::tie(subject, predicate) = queries[first];
				std::string value = get_header_property(hdt_file_path, subject, predicate);
				// This is necessary for literals.
				value.erase( std::remove(value.begin(), value.end(), '"'), value.end() );
				if ( last != value )
				{
					std::cerr << "Property '" << first << "' does not match. " << last << "(expected) != " << value << "(actual)" << std::endl;
					error = 1;
				}
			}
		}
	}
	return error;
}

/*
 * Read an HDT file and iterate through its triples
 */
int
read_hdt_file(const std::string& hdt_file_path)
{
	int error = 0;
	try {
		HDT *hdt = HDTManager::mapIndexedHDT(hdt_file_path.c_str(), NULL);
		// search everything
		IteratorTripleString *it = hdt ->search("", "", "");
		while (it ->hasNext() )
		{
			// Iterate and check ids are OK
			TripleString *ts = it ->next();
			if (ts ->getObject().size() <= 0
					|| ts ->getSubject().size() <= 0
					|| ts ->getPredicate().size() <= 0)
			{
				error = 1;
				std::cerr << "Cannot get triples properly:" << ts ->getObject() << std::endl;
				break;
			}
		}
		delete it;
		delete hdt;
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		error = 1;
	}
	return error;
}

int
main(int argc, char** argv)
{
	// Given these configuration files
	std::vector<std::string> elem = {
		"../presets/dictionaryfour.hdtcfg",
		"../presets/dictionaryliteral.hdtcfg",
		"../presets/ops.hdtcfg"};
	std::string nt_file_path = "../data/test.nt";
	std::string hdt_file_path = "./test.hdt";
	int err_creations = 0;
	int err_checking = 0;
	int err_readings = 0;
	// For each of them, check that
	//  1. HDT is created
	//  2. Header has same properties
	//  3. Iterate triples
	for (std::vector<std::string>::const_iterator iter = elem.begin(); iter != elem.end(); ++iter)
	{
		err_creations += create_hdt_file(nt_file_path, *iter, hdt_file_path);
		err_checking += check_header(hdt_file_path, *iter);
		err_readings += read_hdt_file(hdt_file_path);
		std::remove(hdt_file_path.c_str());
	}
	std::cout << "Creations: " << err_creations << std::endl;
	std::cout << "Checkings: " << err_checking << std::endl;
	std::cout << "Readings: " << err_readings << std::endl;
	// If some error, fail test
	if ( err_creations > 0 || err_readings > 0 || err_checking > 0)
		return 1;

	// For wrong files, make sure creation fails
	std::vector<std::string> wrong_files = {
		"../presets/wrong_dictionaryfour.hdtcfg"
	};
	err_creations = 0;
	for (std::vector<std::string>::const_iterator iter = wrong_files.begin();
			iter != wrong_files.end();
			++iter)
	{
		err_creations += create_hdt_file(nt_file_path, *iter, hdt_file_path);
	}
	std::cout << "Expected fails: " << wrong_files.size()
		<< ", Actual fails: " << err_creations << std::endl;
	// If expected fails not equal to actual fails, then fail test
	if ( err_creations != wrong_files.size() )
		return 1;
	return 0;
}
