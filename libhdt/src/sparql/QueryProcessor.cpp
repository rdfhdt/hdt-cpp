/*
 * QueryProcessor.cpp
 *
 *  Created on: 11/08/2012
 *      Author: mck
 */
#include <stdexcept>
#include <list>

#include "QueryProcessor.hpp"

namespace hdt {

QueryProcessor::QueryProcessor(HDT *hdt) : hdt(hdt) {

}

QueryProcessor::~QueryProcessor() {

}


bool varbind_cmp(VarBindingInterface* a, VarBindingInterface* b) {
	return a->estimatedNumResults() < b->estimatedNumResults();
}

set<string> getCommonVars(VarBindingInterface& a, VarBindingInterface& b) {
	set<string> output, varsa;
	// Insert all A variables into a set.
    for (size_t i = 0; i < a.getNumVars(); i++) {
		const char* varname = a.getVarName(i);
		varsa.insert(varname);
	}
	// Do intersection, going through B, searching in A.
    for (size_t i = 0; i < b.getNumVars(); i++) {
		string var(b.getVarName(i));
		if (varsa.find(var) != varsa.end()) {
			output.insert(var);
		}
	}

	return output;
}



VarBindingString* QueryProcessor::searchJoin(vector<TripleString>& patterns, set<string>& vars) {
	// try {
		if (patterns.size() == 0) {
			return new EmptyVarBingingString();
		}

		set < string > neededVars;
		map < string, TripleComponentRole > varRole;

		// Gather all appearing vars.
		for (vector<TripleString>::iterator itPat = patterns.begin();
				itPat != patterns.end(); ++itPat) {
			TripleString &triple = *itPat;

			if (IS_VARIABLE(triple.getSubject())) {
				neededVars.insert(triple.getSubject());
				varRole.insert(
						pair<string, TripleComponentRole>(triple.getSubject(),
								SUBJECT));
			}

			if (IS_VARIABLE(triple.getPredicate())) {
				neededVars.insert(triple.getPredicate());
				varRole.insert(
						pair<string, TripleComponentRole>(triple.getPredicate(),
								PREDICATE));
			}

			if (IS_VARIABLE(triple.getObject())) {
				neededVars.insert(triple.getObject());
				varRole.insert(
						pair<string, TripleComponentRole>(triple.getObject(),
								OBJECT));
			}
		}

#if 0
		// Show them
		for(map<string, TripleComponentRole>::iterator itN = varRole.begin(); itN!=varRole.end(); ++itN) {
			cout << "Needed var: " << itN->first << " as " << itN->second << endl;
		}
#endif

		// Create Pattern searchers
		vector<VarBindingInterface *> bindings;

		for (vector<TripleString>::iterator itPat = patterns.begin();
				itPat != patterns.end(); ++itPat) {
			TripleID triplePatID;
            hdt->getDictionary()->tripleStringtoTripleID(*itPat, triplePatID);
			//cout << "PatternString: " << *itPat << " PatternID: " << triplePatID << endl;

			vector<unsigned char> vars; // each position means: 1 Subject, 2 Predicate, 3 Object.
			vector < string > varnames; // If not bound, return null.

			// Fill array indicating which vars we are going to extract from the triple pattern.
			if (neededVars.find(itPat->getSubject()) != neededVars.end()) {
				varnames.push_back(itPat->getSubject());
				vars.push_back(1);
			}
			if (neededVars.find(itPat->getPredicate()) != neededVars.end()) {
				varnames.push_back(itPat->getPredicate());
				vars.push_back(2);
			}
			if (neededVars.find(itPat->getObject()) != neededVars.end()) {
				varnames.push_back(itPat->getObject());
				vars.push_back(3);
			}

			VarBindingInterface *bind = new TriplePatternBinding(hdt->getTriples(), triplePatID, vars, varnames);
			cout << "Pattern: " << *itPat << " Results: "
					<< bind->estimatedNumResults() << endl;
			bindings.push_back(bind);
		}

		// Sort by increasing num results.
		std::sort(bindings.begin(), bindings.end(), varbind_cmp);

		// Remove unused variables.

		// Construct left-recursive tree by bottom-up composition.
		list<VarBindingInterface*> items;

        for (size_t i = 0; i < bindings.size(); i++) {
			items.push_back(bindings[i]);
		}

		VarBindingInterface *root = *(items.begin());
		items.remove(root);

		set < string > bindedVars;

		while (items.size() > 0) {
			VarBindingInterface *left = root;
			for (list<VarBindingInterface *>::iterator it = items.begin();
					it != items.end(); ++it) {
				VarBindingInterface *right = *it;
				set < string > commonVars = getCommonVars(*left, *right);
				if (commonVars.size() > 0) {
					string joinVar = *(commonVars.begin());

					if (left->estimatedNumResults() > 200000
							&& left->isOrdered(	left->getVarIndex(joinVar.c_str()) )
							&& right->isOrdered( right->getVarIndex(joinVar.c_str()))) {
						root = new MergeJoinBinding((char *) joinVar.c_str(), left, right);
					} else {
						// TODO: Heuristic to use presort+mergejoin when the number of left results is big.
						root = new IndexJoinBinding((char *) joinVar.c_str(), left, right);
					}
					items.remove(right);
					break;
				} else {

				}
			}
			// IF no match found??
		}

		return new BasicVarBindingString(varRole, new VarFilterBinding(root, vars), hdt->getDictionary());
	// } catch (char *e) {
	// 	cout << "Exception: " << e << endl;
	// 	throw std::runtime_error("Exception");
	// }

	throw std::runtime_error("Could not find query tree");
}

}
