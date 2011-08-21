/*
 * RDFParserNtriplesCallback.cpp
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#include "RDFParserNtriplesCallback.hpp"
#include "../util/fileUtil.hpp"

#include <fstream>

using namespace std;

namespace hdt {

RDFParserNtriplesCallback::RDFParserNtriplesCallback()
{
}

RDFParserNtriplesCallback::~RDFParserNtriplesCallback() {

}

void RDFParserNtriplesCallback::doParse(const char *fileName, const char *baseUri, RDFNotation notation, RDFCallback *callback) {
	ifstream in(fileName);

	string line;
	TripleString ts;

	size_t filePos;


	while(in.good()){
		filePos = in.tellg();
		getline(in, line);

		int pos = 0;
		size_t firstIndex = 0;
		size_t lastIndex = 0;
		bool errorParsing = false;

		vector<string> node(3);

		while (true) {
			line = line.substr(firstIndex);

			if (line == "." || line == "\n" || line == "" || line.at(0) == '#')
				break;

			//obvious space
			if (line.at(0) == ' ') {
				//do nothing
				lastIndex = 0;
			}
			//URI
			else if (line.at(0) == '<') {
				lastIndex = line.find(">");
				//check size of pos
				if (pos > 2) {
					errorParsing = true;
					break;
				}
				node[pos] = line.substr(0, lastIndex + 1);
				pos++;
			}
			//Literal
			else if (line.at(0) == '"') {
				lastIndex = line.find('"', 1);
				//check if literal is escaped
				while (true) {
					bool escaped = false;
					int temp = lastIndex - 1;

					while (temp > 0 && line.at(temp) == '\\') {
						if (escaped)
							escaped = false;
						else
							escaped = true;
						temp--;
					}

					if (!escaped)
						break;
					lastIndex++;
					if (lastIndex == line.length())
						//Cannot find the (unescaped) end
						errorParsing = true;
					lastIndex = line.find('"', lastIndex);
					if (lastIndex == string::npos)
						//Cannot find the (unescaped) end
						errorParsing = true;
				}

				// literal can extend to a bit more than just the ",
				// also take into account lang and datatype strings
				if (line.at(lastIndex + 1) == '@') {
					// find end of literal/lang tag
					lastIndex = line.find(' ', lastIndex + 1) - 1;
				} else if (line.at(lastIndex + 1) == '^') {
					lastIndex = line.find('>', lastIndex + 1);
				}
				//check size of pos
				if (pos > 2) {
					errorParsing = true;
					break;
				}
				node[pos] = line.substr(0, lastIndex + 1);
				pos++;
			}
			//blank, a variable, a relative predicate
			else if (line.at(0) == '_' || (line.find(":") != string::npos)) {
				lastIndex = line.find(" ");
				//check size of pos
				if (pos > 2) {
					errorParsing = true;
					break;
				}
				node[pos] = line.substr(0, lastIndex);
				pos++;
			}
			//parameter or variable ---> obviate for Triples. In future, add to a Hash
			else if (line.at(0) == '@' || line.at(0) == '?') {
				break;
			}
			// test if number
			else {
				// else it is a parsing error
				lastIndex = line.find(" ");

				if(lastIndex!=string::npos) {
					for (size_t j = 0; j < lastIndex; j++) {
						if (!isdigit(line.at(j)) && line.at(j) != '.' && line.at(j)
								!= ',' && line.at(j) != '-') {
							errorParsing = true;
						}
					}
				} else {
					errorParsing = true;
				}

				if (errorParsing == false) {
					node[pos] = line.substr(0, lastIndex);
					pos++;
				}

				break;
			}

			firstIndex = lastIndex + 1;
		}

		if (errorParsing == true || (pos != 0 && pos != 3)) {
			throw "Error parsing ntriples file.";
		}

		ts.setAll(node[0], node[1], node[2]);
		callback->processTriple(ts, filePos);
	}
}

}
