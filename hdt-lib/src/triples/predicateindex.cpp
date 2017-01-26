#include "predicateindex.hpp"
#include "BitmapTriples.hpp"

#include "../util/StopWatch.hpp"

namespace hdt {

PredicateIndex::PredicateIndex(BitmapTriples *triples) : triples(triples) {


}

PredicateIndexWavelet::PredicateIndexWavelet(BitmapTriples *triples) : PredicateIndex(triples) {

}

size_t PredicateIndexArray::calculatePos(size_t predicate) {
	if(predicate<=1) {
		return 0;
	}
	return bitmap->select1(predicate-1)+1;
}

size_t PredicateIndexArray::getAppearance(size_t predicate, size_t appearance) {
	// Warning: Not concurrency friendly
	//if(currpred!=predicate) {
	currpred = predicate;
	currpos=calculatePos(predicate);
	//}

	size_t val = array->get(currpos+appearance-1);
	//cout << "Appearance at pos: " << currpos << " => "<< val<< endl;
	return val;
}

size_t PredicateIndexArray::getNumPredicates() {
    //return predCount->getNumberOfElements();
    return bitmapTriples->predicateCount->getNumberOfElements();
}

size_t PredicateIndexArray::load(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener) {
    size_t count = 0;
    bitmap = new BitSequence375();
    count += bitmap->load(&ptr[count], ptrMax, listener);

    array = new LogSequence2();
    count += array->load(&ptr[count], ptrMax, listener);

#if 0
    cout << "Predicate index:                                                    " << endl;
//    for(size_t i=0;i<array->getNumberOfElements();i++) {
//    	cout << array->get(i) << " " << bitmap->access(i)<< endl;
//    }
    for(size_t i=1;i<=getNumPredicates();i++) {
    	cout << "Predicate "<<i<< " first pos: " << calculatePos(i) << " occurs "<< getNumAppearances(i) << " times. "<<endl;

    	for(size_t j=1; j<=getNumAppearances(i);j++) {
    		size_t pos = getAppearance(i, j);
    		cout << "\t Appearance "<< j<< " at " << pos << " => " << bitmapTriples->arrayY->get(pos) << endl;
    	}
    }
#endif

    return count;
}

void PredicateIndexArray::generate(ProgressListener *listener) {
    // Count predicates


    StopWatch st;
    IntermediateListener iListener(listener);
    iListener.setRange(0,20);
    LogSequence2 *predCount = new LogSequence2(bits(triples->arrayY->getNumberOfElements()));

    size_t maxCount = 0;
    for(size_t i=0;i<triples->arrayY->getNumberOfElements(); i++) {
        // Read value
        size_t val = triples->arrayY->get(i);

        // Grow if necessary
        if(predCount->getNumberOfElements()<val) {
            predCount->resize(val);
        }

        // Increment
        size_t count = predCount->get(val-1)+1;
        maxCount = count>maxCount ? count : maxCount;
        predCount->set(val-1, count);

        NOTIFYCOND3(&iListener, "Counting appearances of predicates", i, triples->arrayY->getNumberOfElements(), 20000);
    }
    predCount->reduceBits();

#if 0
    for(size_t i=0;i<predCount->getNumberOfElements();i++) {
        cout << "Predicate " << i << " appears " << predCount->get(i) << " times." << endl;
    }
#endif

    // Convert predicate count to bitmap
    bitmap = new BitSequence375(triples->arrayY->getNumberOfElements());
    size_t tempCountPred=0;
    iListener.setRange(20,25);
    for(size_t i=0;i<predCount->getNumberOfElements();i++) {
        tempCountPred += predCount->get(i);
        bitmap->set(tempCountPred-1, true);
        NOTIFYCOND3(&iListener, "Creating Predicate bitmap", i, predCount->getNumberOfElements(), 100000);
    }
    if(triples->arrayY->getNumberOfElements())
        bitmap->set(triples->arrayY->getNumberOfElements()-1, true);
    cerr << "Predicate Bitmap in " << st << endl;
    st.reset();

    delete predCount;

    // Create predicate index
    LogSequence2 *array = new LogSequence2(bits(triples->arrayY->getNumberOfElements()), triples->arrayY->getNumberOfElements());
    array->resize(triples->arrayY->getNumberOfElements());

    LogSequence2 *insertArray = new LogSequence2(bits(triples->arrayY->getNumberOfElements()), bitmap->countOnes());
    insertArray->resize(bitmap->countOnes());

    iListener.setRange(25,100);
    for(size_t i=0;i<triples->arrayY->getNumberOfElements(); i++) {
            size_t predicateValue = triples->arrayY->get(i);

            size_t insertBase = predicateValue==1 ? 0 : bitmap->select1(predicateValue-1)+1;
            size_t insertOffset = insertArray->get(predicateValue-1);
            insertArray->set(predicateValue-1, insertOffset+1);

            array->set(insertBase+insertOffset, i);
            NOTIFYCOND3(&iListener, "Generating predicate references", i, triples->arrayY->getNumberOfElements(), 100000);
    }

    this->array = array;

    delete insertArray;

    cerr << "Count predicates in " << st << endl;
}

}
