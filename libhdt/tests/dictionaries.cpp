#include <iostream>
#include <HDTManager.hpp>

using namespace hdt;

int 
main(int argc, char **argv)
{
	char error = 0;
	try
	{
		HDT *hdt = HDTManager::mapHDT("../data/literals.hdt");
		Dictionary *dict = hdt ->getDictionary();
		TripleID tid;
		IteratorTripleID *iter = hdt ->getTriples() ->search( tid );
		while ( iter -> hasNext() )
		{
			TripleID* id = iter ->next();
			unsigned int global_object_id = id ->getObject();
			if (global_object_id < dict ->getMaxSubjectID())
			{
				cerr << "Object id is less than max subject id" << endl;
				error = 1;
				break; 
			}
			unsigned int local_object_id = dict ->getMapping() == MAPPING2 ? 	
				global_object_id - dict ->getNshared() :
				global_object_id - dict ->getNshared() - dict ->getNsubjects() + 2;
			if ( local_object_id > dict ->getNobjectsLiterals() )
			{
				cerr << "Local Literal id is greater than number of total literals " << endl;
				error = 2;
				break;
			}
		}
		delete iter;
		delete hdt;
	}
	catch (std::exception& e)
	{
		cerr << e.what() << endl;
		error = 1;
	}
	if ( error > 0 )
		return 1;
	return 0;
}
