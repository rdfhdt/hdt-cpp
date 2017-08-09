/*
 * File: HDTFactory.cpp
 * Last modified: $Date: 2012-08-21 21:15:01 +0100 (mar, 21 ago 2012) $
 * Revision: $Revision: 250 $
 * Last modified by: $Author: mario.arias $
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
 * Contacting the authors:
 *   Mario Arias:               mario.arias@gmail.com
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *
 */

#include <HDT.hpp>
#include <HDTVocabulary.hpp>
#include <HDTManager.hpp>
#include "BasicHDT.hpp"

using namespace hdt;


namespace hdt {

HDT *HDTManager::loadHDT(const char *file, ProgressListener *listener){
	BasicHDT *h = new BasicHDT();
    h->loadFromHDT(file, listener);
	return h;
}

HDT *HDTManager::mapHDT(const char *file, ProgressListener *listener){
    BasicHDT *h = new BasicHDT();
    h->mapHDT(file, listener);
	return h;
}

HDT *HDTManager::mapIndexedHDT(const char *file, ProgressListener *listener){
    BasicHDT *h = new BasicHDT();
    IntermediateListener iListener(listener);
    iListener.setRange(0,10);
    h->mapHDT(file, &iListener);

    iListener.setRange(10,100);
    h->loadOrCreateIndex(&iListener);
	return h;
}

HDT *HDTManager::loadIndexedHDT(const char *file, ProgressListener *listener){
	BasicHDT *h = new BasicHDT();
    IntermediateListener iListener(listener);
    iListener.setRange(0,50);
	h->loadFromHDT(file, listener);

	iListener.setRange(50,100);
	h->loadOrCreateIndex(listener);
	return h;
}

HDT *HDTManager::indexedHDT(HDT *hdt, ProgressListener *listener){
	BasicHDT *bhdt = dynamic_cast<BasicHDT*>(hdt);
	bhdt->loadOrCreateIndex(listener);
	return bhdt;
}

HDT *HDTManager::generateHDT(const char *rdfFileName, const char *baseURI, RDFNotation rdfNotation, HDTSpecification &hdtFormat, ProgressListener *listener){
	BasicHDT *hdt = new BasicHDT(hdtFormat);
	hdt->loadFromRDF(rdfFileName, baseURI, rdfNotation, listener);
	return hdt;
}

}
