#include "predicateindex.hpp"
#include "BitmapTriples.hpp"

#include "../util/StopWatch.hpp"

namespace hdt {

PredicateIndex::PredicateIndex(BitmapTriples *triples) : triples(triples) {


}

PredicateIndexWavelet::PredicateIndexWavelet(BitmapTriples *triples) : PredicateIndex(triples) {

}

void PredicateIndexArray::generate(ProgressListener *listener) {
    // Count predicates


    StopWatch st;
    IntermediateListener iListener(listener);
    iListener.setRange(90,100);
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

        //NOTIFYCOND3(&iListener, "Counting appearances of predicates", i, array->getNumberOfElements(), 20000);
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
    for(size_t i=0;i<predCount->getNumberOfElements();i++) {
        tempCountPred += predCount->get(i);
        bitmap->set(tempCountPred-1, true);
        NOTIFYCOND3(&iListener, "Creating bitmap", i, predCount->getNumberOfElements(), 10000);
    }
    bitmap->set(triples->arrayY->getNumberOfElements()-1, true);
    cout << "Predicate Bitmap in " << st << endl;
    st.reset();

    delete predCount;

    // Create predicate index
    LogSequence2 *array = new LogSequence2(bits(triples->arrayY->getNumberOfElements()), triples->arrayY->getNumberOfElements());
    array->resize(triples->arrayY->getNumberOfElements());

    LogSequence2 *insertArray = new LogSequence2(bits(triples->arrayY->getNumberOfElements()), bitmap->countOnes());
    insertArray->resize(bitmap->countOnes());

    for(size_t i=0;i<triples->arrayY->getNumberOfElements(); i++) {
            size_t predicateValue = triples->arrayY->get(i);

            size_t insertBase = predicateValue==1 ? 0 : bitmap->select1(predicateValue-1)+1;
            size_t insertOffset = insertArray->get(predicateValue-1);
            insertArray->set(predicateValue-1, insertOffset+1);

            array->set(insertBase+insertOffset, i);
            //NOTIFYCOND3(&iListener, "Generating predicate references", i, triples->arrayY->getNumberOfElements(), 20000);
    }

    this->array = array;

    delete insertArray;

    cout << "Count predicates in " << st << endl;
}

}
