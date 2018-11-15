/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.txt
 */

#ifndef _HALSTORAGEDRIVER_H
#define _HALSTORAGEDRIVER_H
#include "halCommon.h"

namespace hal {
    /**
     * Class for access a genome's DNA sequence. It handles buffering data and only goes
     * to the storage layer when the buffer needs filled, inlining of base access.
     * There can be multiple object active independently on a given genome.  Assumes
     * that DNA is nibble-encode and handles encoding and decoding
     */
    class DNAStorage {
        public:
        /* Destructor */
        virtual ~DNAStorage() = 0;

        /* get a base at the specified index. */
        inline char getBase(hal_index_t index) const {
            hal_index_t relIndex = access(index);
            return dnaUnpack(relIndex, _buffer[relIndex / 2]);
        }

        /* set a base at the specified index. */
        inline void setBase(hal_index_t index, char base) {
            hal_index_t relIndex = access(index);
            _buffer[relIndex / 2] = dnaPack(base, relIndex, _buffer[relIndex / 2]);
            _dirty = true;
        }
        
        protected:
        /* constructor */
        DNAStorage(hal_index_t startIndex,
                   hal_index_t endIndex,
                   char* buffer) :
            _startIndex(startIndex),
            _endIndex(endIndex),
            _buffer(buffer),
            _dirty(false) {
        }

        
        /* refresh the buffer if needed and return relative index */
        inline hal_index_t access(hal_index_t index) const {
            if ((index < _startIndex) or (index >= _endIndex)) {
                fetch(index);
            }
            return index - _startIndex;
        }
        
        /* refreshed the buffer, const since it is abstracted as a cache */
        virtual void fetch(hal_index_t index) const = 0;

        /* Cached buffer.  Index will always be even (first nibble) */
        mutable hal_index_t _startIndex;
        mutable hal_index_t _endIndex;
        mutable char* _buffer;
        mutable bool _dirty;
    };
}
#endif
