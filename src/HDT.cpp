/* HDT.cpp
 * Copyright (C) 2010, Javier D. Fernandez & Miguel A, Martinez-Prieto
 * all rights reserved.
 *
 * HDT main
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 * Contacting the authors:
 *   Javier D. Fernandez:         jfergar@infor.uva.es
 *   Miguel A Martinez-Prieto:  migumar2@infor.uva.es
 *
 */

#include <iostream>

#include "HDTLoader.h" 
#include "RDFParser.h"
#include "StatsGen.h"

using namespace std;

/** Console
 * @return void
 */
void console() {
	string command;
	//use as a console

	cout << "\n   <ENTERING CONSOLE...>\n";
	cout << "   (Type help for commands)\n\n";
	char commandChar[256];
	vector < string > subcommand;
	int i = 0;
	while (true) {
		cout << "\n\n$>";
		cin.getline(commandChar, 256);
		command = commandChar;

		//split as space separator
		stringExplode(command, " ", &subcommand);

		for (i = 0; i < subcommand.size(); i++) {

			if (subcommand[i] == "parse") {
				//verify arguments
				if (i == (subcommand.size() - 1)) {
					cout << "\n !!Command error. Try to provide filename\n";
					break;
				}
				char * input = new char[subcommand[i + 1].length() + 1];
				strcpy(input, subcommand[i + 1].c_str()); /* optarg has the argument of -i */
				RDFParser *parser = new RDFParser();
				parser->parse(input);
				delete parser;
				delete input;
				break;

			} else if (subcommand[i] == "load") {
				//verify arguments
				if (i == (subcommand.size() - 1)) {
					cout << "\n !!Command error. Try to provide filename\n";
					break;
				}
				char * input = new char[subcommand[i + 1].length() + 1];
				strcpy(input, subcommand[i + 1].c_str()); /* optarg has the argument of -i */
				HDTLoader *loader = new HDTLoader();
				loader->load(input);
				loader->console();
				delete loader;
				delete input;
				break;

			} else if (subcommand[i] == "exit") {
				exit(0);
			} else if (subcommand[i] == "help") {
				printHelpConsole();
				break;
			} else {
				cout << "\n" << subcommand[i]
						<< ": command not found. Type help for commands";
				break;
			}
		}
		subcommand.clear();
	}
}

/** Main
 * @param argc Description of the param.
 * @param argv_b Description of the param.
 * @return The expected result
 */
int main(int argc, char* argv[]) {
	printCredits();

	// Reads input parameters from command line.
	if (argc >= 3) {
		if (strcmp(argv[1], "-p") == 0 || strcmp(argv[1], "--parse") == 0) {
			// p option for 'parsing'	
			// argv[2] contains the config file
			RDFParser *parser = new RDFParser();
			parser->parse(argv[2]);
			delete parser;
		} else {
			if (strcmp(argv[1], "-l") == 0 || strcmp(argv[1], "--load") == 0) {
				// l option for 'loading'
				// argv[2] locates the HDT representation
				HDTLoader *loader = new HDTLoader();
				loader->load(argv[2]);

				if (argc >= 4) {
					string format = "ntriples";
					string output = "output.rdf";
					bool transform = false;
					int i = 0;
					for (i = 3; i < argc;) {
						if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i],
								"--serialize") == 0) {
							transform = true;
							if ((i + 1) < argc) {
								if (argv[i + 1][0] != '-') {
									format = argv[i + 1];
									//verify format
									if (format != "ntriples" && format
											!= "turtle" && format
											!= "rdfxml-xmp" && format
											!= "rdfxml-abbrev" && format
											!= "rdfxml" && format != "rss-1.0"
											&& format != "atom" && format
											!= "dot" && format != "rss-1.0"
											&& format != "json-triples"
											&& format != "json") {
										cout << "<WARNING> The format:"
												<< format
												<< " is possibly not allowed by the serializer.\n\n";
									}
									i = i + 2;
								} else
									i = i + 1;
							} else {
								i = i + 1;
							}
						} else if (strcmp(argv[i], "-o") == 0 || strcmp(
								argv[i], "--output") == 0) {
							if ((i + 1) < argc) {
								if (argv[i + 1][0] != '-') {
									output = argv[i + 1];
									i = i + 2;
								} else
									i = i + 1;
							} else {
								i = i + 1;
							}
						} else {
							cout
									<< "   <ERROR> Wrong parameters.\nFor help, type: hdt -h\n\nEntering Console mode...."
									<< endl;
							loader->console();
							delete loader;
							exit(0);
						}
					}
					if (transform == true) {
						loader->serialize((char *) output.c_str(),
								(char *) format.c_str());
					} else {
						cout
								<< "   <ERROR> Wrong parameters.\nFor help, type: hdt -h\n\nEntering Console mode..."
								<< endl;
						loader->console();
					}
				} else {
					loader->console();
				}

				delete loader;

			} else if ((argc == 4) && (strcmp(argv[1], "-s") == 0 || strcmp(
					argv[1], "--stats") == 0)) {
				StatsGen *stats = new StatsGen();
				stats->readFromDataset(argv[2], argv[3]);
				//stats->process(argv[3]);
				delete stats;
			} else if ((argc == 4) && (strcmp(argv[1], "-S") == 0 || strcmp(
					argv[1], "--Stats") == 0)) {
				StatsGen *stats = new StatsGen();
				stats->readFromHDT(argv[2], argv[3]);
				stats->process(argv[3]);
				delete stats;
			} else {
				// Bad option
				cout << "   <ERROR> '" << argv[1][0]
						<< "' is not a valid execution option\n\nFor help, type: hdt -h"
						<< endl;
			}
		}
	} else if (argc == 2) {
		if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
			printHelp(argv[0]);
		} else if (strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "--console")
				== 0) {
			console();
		} else {
			// Bad number of parameters
			cout
					<< "   <ERROR> Wrong number of parameters.\nFor help, type: hdt -h"
					<< endl;
		}
	} else {
		// Bad number of parameters
		cout
				<< "   <ERROR> Wrong number of parameters.\nFor help, type: hdt -h"
				<< endl;
	}
}
