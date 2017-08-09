#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_RAPTOR
#include <raptor2/raptor2.h>
#endif

/* rdfprint.c: print triples from parsing RDF/XML */

#ifdef HAVE_RAPTOR
static void
print_triple(void* user_data, raptor_statement* triple)
{
  raptor_statement_print_as_ntriples(triple, stdout);
  fputc('\n', stdout);
}
#endif

int
main(int argc, char *argv[])
{
#ifdef HAVE_RAPTOR
  raptor_world *world = NULL;
  raptor_parser* rdf_parser = NULL;
  unsigned char *uri_string;
  raptor_uri *uri, *base_uri;

  world = raptor_new_world();

  rdf_parser = raptor_new_parser(world, "ntriples");

  raptor_parser_set_statement_handler(rdf_parser, NULL, print_triple);

  uri_string = raptor_uri_filename_to_uri_string(argv[1]);
  uri = raptor_new_uri(world, uri_string);
  base_uri = raptor_uri_copy(uri);

  raptor_parser_parse_file(rdf_parser, uri, base_uri);

  raptor_free_parser(rdf_parser);

  raptor_free_uri(base_uri);
  raptor_free_uri(uri);
  raptor_free_memory(uri_string);

  raptor_free_world(world);
#endif

  return EXIT_SUCCESS;
}
