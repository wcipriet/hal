/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 * Copyright (C) 2012-2019 by UCSC Computational Genomics Lab
 *
 * Released under the MIT license, see LICENSE.txt
 */
#include "halMafScanReference.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace hal;

MafScanReference::MafScanReference() : MafScanner() {
}

MafScanReference::~MafScanReference() {
}

std::string MafScanReference::getRefName(const std::string &mafPath) {
    MafScanner::scan(mafPath, set<string>());
    return _name;
}

void MafScanReference::aLine() {
}

void MafScanReference::sLine() {
    Row &row = _block[_rows - 1];
    // this is the first pass.  so we do a quick sanity check
    if (row._sequenceName.find('.') == string::npos || row._sequenceName.find('.') == 0) {
        throw hal_exception("illegal sequence name found: " + row._sequenceName +
                            ".  Sequence names must be in genomeName.sequenceName format.");
    }

    _name = genomeName(row._sequenceName);
    _mafFile.seekg(0, ios_base::end);
}

void MafScanReference::end() {
}
