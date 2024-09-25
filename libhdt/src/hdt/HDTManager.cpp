/*
 * File: HDTFactory.cpp
 * Last modified: $Date$
 * Revision: $Revision: 250 $
 * Last modified by: $Author$
 *
 * Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 * Copyright (C) 2012, Mario Arias, Javier D. Fernandez, Miguel A. Martinez-Prieto
 * All rights reserved.
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
#include <memory>

using namespace hdt;


namespace hdt {

std::unique_ptr<HDT> HDTManager::loadHDT(const char *file, ProgressListener *listener){
	auto h{std::make_unique<BasicHDT>()};
    h->loadFromHDT(file, listener);
	return h;
}

std::unique_ptr<HDT> HDTManager::loadFromSeveralHDTInMemory(
	const std::vector<std::pair<const char*, size_t>>& buffer_list, 
	string baseUri, ProgressListener * listener)
{
	auto h{std::make_unique<BasicHDT>()};
    h->loadFromSeveralHDT(buffer_list, baseUri, listener);
	return h;
}

std::unique_ptr<HDT> HDTManager::mapHDT(const char *file, ProgressListener *listener){
    auto h{std::make_unique<BasicHDT>()};
    h->mapHDT(file, listener);
	return h;
}

std::unique_ptr<HDT> HDTManager::mapIndexedHDT(const char *file, ProgressListener *listener){
    auto h{std::make_unique<BasicHDT>()};
    IntermediateListener iListener(listener);
    iListener.setRange(0,10);
    h->mapHDT(file, &iListener);

    iListener.setRange(10,100);
    h->loadOrCreateIndex(&iListener);
	return h;
}

std::unique_ptr<HDT> HDTManager::loadIndexedHDT(const char *file, ProgressListener *listener){
	auto h{std::make_unique<BasicHDT>()};
    IntermediateListener iListener(listener);
    iListener.setRange(0,50);
	h->loadFromHDT(file, listener);

	iListener.setRange(50,100);
	h->loadOrCreateIndex(listener);
	return h;
}

std::unique_ptr<HDT> HDTManager::loadIndexedHDTFromMemory(const char *hdtBuffer, size_t hdtBufferLen, const char* hdtIndexBuffer, size_t hdtIndexBufferLen, ProgressListener *listener)
{
	auto h{std::make_unique<BasicHDT>()};
    IntermediateListener iListener(listener);
    iListener.setRange(0,50);
	h->loadFromHDT(hdtBuffer, hdtBufferLen, listener);

	iListener.setRange(50,100);
	h->loadOrCreateIndex(hdtIndexBuffer, hdtIndexBufferLen, listener);
	return h;
}

void HDTManager::indexedHDT(HDT* hdt, ProgressListener *listener){
	BasicHDT *bhdt = dynamic_cast<BasicHDT*>(hdt);
	bhdt->loadOrCreateIndex(listener);
}

std::unique_ptr<HDT> HDTManager::generateHDT(const char *rdfFileName, const char *baseURI, RDFNotation rdfNotation, HDTSpecification &hdtFormat, ProgressListener *listener){
	auto hdt{std::make_unique<BasicHDT>(hdtFormat)};
	hdt->loadFromRDF(rdfFileName, baseURI, rdfNotation, listener);
	return hdt;
}

}
