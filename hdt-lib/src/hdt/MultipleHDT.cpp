/*
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
 * Contacting the authors:
 *   Mario Arias:               mario.arias@gmail.com
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *
 */

#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <time.h>
#include <HDTVocabulary.hpp>
#include <RDFParser.hpp>
#include <HDTManager.hpp>

#include "../util/StopWatch.hpp"
#include "../util/fileUtil.hpp"

#include "ControlInformation.hpp"
#include "HDTFactory.hpp"
#include "MultipleHDT.hpp"
#include "../header/PlainHeader.hpp"
#include "../dictionary/PlainDictionary.hpp"
#include "../dictionary/KyotoDictionary.hpp"
#include "../dictionary/FourSectionDictionary.hpp"

#ifdef HAVE_CDS
#include "../dictionary/LiteralDictionary.hpp"
#endif

#include "../triples/TriplesList.hpp"
#include "../triples/TriplesKyoto.hpp"

#ifndef WIN32
#include "../triples/TripleListDisk.hpp"
#endif

#include "../triples/PlainTriples.hpp"
#include "../triples/CompactTriples.hpp"
#include "../triples/BitmapTriples.hpp"
#include "../triples/TripleOrderConvert.hpp"

#include "../third/gzstream.h"

#include "TripleIDStringIterator.hpp"

using namespace std;

namespace hdt {


IteratorTripleString* MultipleHDT::search(const char* subject,
		const char* predicate, const char* object,
		vector<const char *> contexts) {
	TripleString ts(subject, predicate, object);
	try {
		vector<IteratorTripleString*> ret;
		if (contexts.size()>0) {
			for (int i = 0; i < contexts.size(); i++) {
				ContextEntryIt foundcontext = HDTMultiple.find(contexts[i]);
				cout<<"contexts[i]:"<<contexts[i]<<endl;
				if (foundcontext != HDTMultiple.end()) { //found
					ret.push_back(
							foundcontext->second->search(subject, predicate,
									object));
				}
			}

		} else {
			// iterate all
			for (ContextEntryIt hdt_it = HDTMultiple.begin();
					hdt_it != HDTMultiple.end(); ++hdt_it) {
				IteratorTripleString* it = hdt_it->second->search(subject, predicate, object);
				if (it->hasNext()){
					ret.push_back(it);
				}
			}
		}
		VectorIteratorTripleString *it = new VectorIteratorTripleString(ret);
		TripleString *triple = it->next();
		return it;
	} catch (std::exception& e) {
		cerr << "Exception: " << e.what() << endl;
	}
	return new IteratorTripleString();
}

void MultipleHDT::mapHDTs(vector<string> files, ProgressListener *listener) {
	try {

		IntermediateListener iListener(listener);

		iListener.setRange(0, 90);
		for (int i = 0; i < files.size(); i++) {
			//cout << "Loading " << files[i] << endl;
			// test if the context is already loaded
			ContextEntryIt foundcontext = HDTMultiple.find(files[i].c_str());
			if (foundcontext == HDTMultiple.end()) {
				// Did not exist, insert
				HDTMultiple[files[i].c_str()] = HDTManager::mapIndexedHDT(
						files[i].c_str());
			}
		}
	} catch (std::exception& e) {
		throw;
	}
}
}
