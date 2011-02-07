/* RDFSyntaxMediator.cpp
 * Copyright (C) 2010, Javier D. Fernandez & Miguel A, Martinez-Prieto
 * all rights reserved.
 *
 * RDF syntaxes mediator (parser,serializar) to compact representations.
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
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A Martinez-Prieto:  migumar2@infor.uva.es
 *
 */

#include "RDFSyntaxMediator.h"

/** Base constructor */
RDFSyntaxMediator::RDFSyntaxMediator() {
}

string *node = new string[3];

/** RDF Triple Handle
 * @param dictionaryTriples Description of the param.
 * @param triple Description of the param.
 * @return void
 */
void RDFTripleHandle(void* dictionaryTriples, const raptor_statement* triple) {

	char *returned_subject;
	char *returned_predicate;
	char *returned_object;

	returned_subject = (char*) raptor_statement_part_as_string(triple->subject,
			triple->subject_type, NULL, NULL);
	node[0] = returned_subject;

	returned_predicate = (char*) raptor_statement_part_as_string(
			triple->predicate, triple->predicate_type, NULL, NULL);
	node[1] = returned_predicate;

	returned_object = (char*) raptor_statement_part_as_string(triple->object,
			triple->object_type, triple->object_literal_datatype,
			triple->object_literal_language);
	node[2] = returned_object;

	((DictionaryTriples *) dictionaryTriples)->dictionary->checkTriple(node);
	//cout<<"triple:"<<node[0]<<" "<<node[1]<<" "<<node[2]<< " .\n";
	*(((DictionaryTriples *) dictionaryTriples)->ntriples)
			= *(((DictionaryTriples *) dictionaryTriples)->ntriples) + 1;

	free ( returned_subject), free(returned_predicate), free(returned_object);

}

/** RDF Full Triple Handle
 * @param dictionaryTriples Description of the param.
 * @param triple Description of the param.
 * @return void
 */
void RDFFullTripleHandle(void* dictionaryTriples,
		const raptor_statement* triple) {

	char *returned_subject;
	char *returned_predicate;
	char *returned_object;

	returned_subject = (char*) raptor_statement_part_as_string(triple->subject,
			triple->subject_type, NULL, NULL);
	node[0] = returned_subject;

	returned_predicate = (char*) raptor_statement_part_as_string(
			triple->predicate, triple->predicate_type, NULL, NULL);
	node[1] = returned_predicate;

	returned_object = (char*) raptor_statement_part_as_string(triple->object,
			triple->object_type, triple->object_literal_datatype,
			triple->object_literal_language);
	node[2] = returned_object;

	//cout<<"triple:"<<node[0]<<" "<<node[1]<<" "<<node[2]<< " .\n";

	int subject =
			((DictionaryTriples *) dictionaryTriples)->dictionary->retrieveID(
					node[0], VOC_SUBJECT);
	int predicate =
			((DictionaryTriples *) dictionaryTriples)->dictionary->retrieveID(
					node[1], VOC_PREDICATE);
	int object =
			((DictionaryTriples *) dictionaryTriples)->dictionary->retrieveID(
					node[2], VOC_OBJECT);

	((DictionaryTriples *) dictionaryTriples)->triples->insert(subject,
			predicate, object);

	free ( returned_subject), free(returned_predicate), free(returned_object);

}

/** Serialize Triple
 * @param rdf_serializer Description of the param.
 * @param triple Description of the param.
 * @return void
 */
void serialize_triple(void* rdf_serializer, const raptor_statement* triple) {
	raptor_serialize_statement((raptor_serializer*) rdf_serializer, triple);
}

/** relay_namespaces saves namespaces for serializing
 * @param namespacesData Description of the param.
 * @param nspace Description of the param.
 * @return void
 */
void relay_namespaces(void* namespacesData, raptor_namespace *nspace) {
	((vector<raptor_namespace*>*) namespacesData)->push_back(nspace);
}

/** Parsing
 * @param pathFile Description of the param.
 * @param dictionaryTriples Description of the param.
 * @return The expected result
 */
bool RDFSyntaxMediator::parsing(char *pathFile,
		DictionaryTriples dictionaryTriples) {
	unsigned char *uri_string;
	raptor_uri *uri, *base_uri;

	raptor_init();

	rdf_parser = raptor_new_parser("guess");

	if (dictionaryTriples.triples == NULL) //parse dictionary
	{
		raptor_set_statement_handler(rdf_parser, &dictionaryTriples,
				RDFTripleHandle);
	} else {
		raptor_set_statement_handler(rdf_parser, &dictionaryTriples,
				RDFFullTripleHandle);
	}
	raptor_set_namespace_handler(rdf_parser, &namespaces, relay_namespaces);

	uri_string = raptor_uri_filename_to_uri_string(pathFile);
	uri = raptor_new_uri(uri_string);
	base_uri = raptor_uri_copy(uri);

	raptor_parse_file(rdf_parser, uri, base_uri);

	raptor_free_parser( rdf_parser);

	raptor_free_uri(base_uri);
	raptor_free_uri(uri);
	raptor_free_memory(uri_string);

	raptor_finish();
	return true;
}

/** Init Serialize
 * @param pathFile Description of the param.
 * @param format Description of the param.
 * @return The expected result
 */
bool RDFSyntaxMediator::init_serialize(char *pathFile, char *format) {

	unsigned char *uri_string;
	raptor_uri *uri, *base_uri;

	raptor_init();

	rdf_parser = raptor_new_parser("ntriples");

	try {
		rdf_serializer = raptor_new_serializer(format); //format can be e.g. rdfxml
		raptor_serialize_start_to_filename(rdf_serializer, pathFile);

		raptor_start_parse(rdf_parser, NULL);
		raptor_set_statement_handler(rdf_parser, rdf_serializer,
				serialize_triple);

		int i = 0;

		//set namespaces if presented in previous parser
		if (namespaces.size() > 0) {
			for (i = 0; i < namespaces.size(); i++)
				;
			{
				raptor_serialize_set_namespace_from_namespace(rdf_serializer,
						namespaces[i]);
			}
		}
	}

	catch (char * str) {
		cout << "Exception while conversing to " << format << ": " << str
				<< '\n';
	}

	return true;
}

/** Serialize Statement
 * @param subject Description of the param.
 * @param predicate Description of the param.
 * @param object Description of the param.
 * @return The expected result
 */
bool RDFSyntaxMediator::serialize_statement(string subject, string predicate,
		string object) {

	size_t buffer_len;

	//buffer_len = subject.length()+predicate.length()+object.length()+5;
	//unsigned char* buffer = new unsigned char[buffer_len];
	//cout<<"(len)subject predicate object:("<<buffer_len<<")"<<subject<<" "<<predicate<<" "<<object<<".\n";
	//fflush(stdout);
	//char* buffer = new char[buffer_len];
	string statement;
	statement.append(subject).append(" ").append(predicate).append(" ").append(
			object).append(" .\n");
	//cout<<"statement:"<<statement<<"\n";
	//cout<<"object:"<<object<<"(len:"<<object.length()<<"\n";
	//fflush(stdout);
	/*
	 strcpy((char*)buffer,(char*)subject.c_str());
	 strcat((char*)buffer," ");
	 strcat((char*)buffer,(char*)predicate.c_str());
	 strcat((char*)buffer," ");
	 strcat((char*)buffer,(char*)object.c_str());
	 strcat((char*)buffer," .\n");
	 */
	//cout<<"buffer:"<<buffer<<"|\n";
	//fflush(stdout);
	// use '1' last parameter to run without buffering

	//raptor_parse_chunk(rdf_parser, (unsigned char*)buffer, buffer_len, 1);
	raptor_parse_chunk(rdf_parser, (unsigned char*) statement.c_str(),
			statement.length(), 0);

	//delete [] buffer;
}

/** End Serialize
 * @return The expected result
 */
bool RDFSyntaxMediator::end_serialize() {
	raptor_parse_chunk(rdf_parser, NULL, 0, 1); /* no data and is_end = 1 */
	raptor_serialize_end( rdf_serializer);
	raptor_free_serializer(rdf_serializer);
	raptor_free_parser( rdf_parser);
	raptor_finish();
	return true;
}

/** Destructor for RDFSyntaxMediator */
RDFSyntaxMediator::~RDFSyntaxMediator() {
}
