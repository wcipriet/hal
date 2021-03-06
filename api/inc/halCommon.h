/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 * Copyright (C) 2012-2019 by UCSC Computational Genomics Lab
 *
 * Released under the MIT license, see LICENSE.txt
 */

#ifndef _HALCOMMON_H
#define _HALCOMMON_H

#include "halDefs.h"
#include <cassert>
#include <locale>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace hal {

    /* size to use for UDC blocks */
    static const unsigned UDC_BLOCK_SIZE = 8 * 1024;

    extern const char to_upper_map[128];

    /** fast conversion of a character to upper case */
    inline char fastUpper(char c) {
        return to_upper_map[unsigned(c)];
    }

    inline bool compatibleWithVersion(const std::string &version) {
        // assume versions are strings tho we treat as floats for now.
        return int(std::stod(HAL_VERSION)) == int(std::stod(version));
    }

    /** C++ style strtok-type function.  Can't remember why I wrote it */
    std::vector<std::string> chopString(const std::string &inString, const std::string &separator);

    /** convert a string to an integer */
    hal_index_t strToInt(const std::string &str);
    
    /** Get the DNA reverse complement of a character.
     * If the input is not a nucleotide, then just return it as is
     * (ie no error checking) */
    inline char reverseComplement(char c) {
        switch (c) {
        case 'A':
            return 'T';
        case 'a':
            return 't';
        case 'C':
            return 'G';
        case 'c':
            return 'g';
        case 'G':
            return 'C';
        case 'g':
            return 'c';
        case 'T':
            return 'A';
        case 't':
            return 'a';
        default:
            break;
        }
        return c;
    }

    /** Get the reversed complement of a string (in place) */
    void reverseComplement(std::string &s);

    /** Reverse the gaps in the string (gap i -> len-1-i) which
     * is not done above (does not reverse dna) */
    void reverseGaps(std::string &s);

    /** Check if a DNA character is a valid base (or n-chracter) */
    inline bool isNucleotide(char c) {
        bool result = false;
        switch (c) {
        case 'A':
        case 'a':
        case 'C':
        case 'c':
        case 'G':
        case 'g':
        case 'T':
        case 't':
        case 'N':
        case 'n':
            result = true;
        default:
            break;
        }
        return result;
    }

    inline bool isTransition(char c1, char c2) {
        assert(isNucleotide(c1) && isNucleotide(c2));
        char x = fastUpper((char)c1);
        char y = fastUpper((char)c2);
        switch (x) {
        case 'A':
            return y == 'G';
        case 'C':
            return y == 'T';
        case 'G':
            return y == 'A';
        case 'T':
            return y == 'C';
        default:
            break;
        }
        return false;
    }

    inline bool isSubstitution(char c1, char c2) {
        return fastUpper(c1) != fastUpper(c2);
    }

    inline bool isTransversion(char c1, char c2) {
        char x = fastUpper(c1);
        char y = fastUpper(c2);
        return ((x != y) && (x != 'N') && (y != 'N') && !isTransition(c1, c2));
    }

    inline bool isMissingData(char c) {
        return c == 'n' || c == 'N';
    }

    inline bool isMasked(char c) {
        return c >= 'a';
    }

    /** test if 3rd codon position is 4-fold degenerate given first 2 positions */
    inline bool isFourfoldDegenerate(char c1, char c2) {
        char x1 = fastUpper(c1);
        char x2 = fastUpper(c2);
        if (x2 == 'T' || x2 == 'G') {
            return x1 == 'C' || x1 == 'G';
        } else if (x2 == 'C') {
            return x1 == 'A' || x1 == 'C' || x1 == 'G' || x1 == 'T';
        }
        return false;
    }

    /** Count the mutations between two DNA strings */
    inline hal_size_t hammingDistance(const std::string &s1, const std::string &s2) {
        assert(s1.length() == s2.length());
        hal_size_t dist = 0;
        for (size_t i = 0; i < s1.length(); ++i) {
            if (isSubstitution(s1[i], s2[i]) == true) {
                ++dist;
            }
        }
        return dist;
    }

    const Genome *getLowestCommonAncestor(const std::set<const Genome *> &inputSet);

    /* Given a set of genomes (input set) find all genomes in the spanning
     * tree including the inptuts (root should be the root of the alignment) */
    void getGenomesInSpanningTree(const std::set<const Genome *> &inputSet, std::set<const Genome *> &outputSet);

    /* Given a node (root), return it and all genomes (including internal nodes)
     * below it in the tree */
    void getGenomesInSubTree(const Genome *root, std::set<const Genome *> &outputSet);

    /* Get all leaf genomes in the alignment, in no particular order. */
    std::vector<const Genome *> getLeafGenomes(const Alignment *alignment);

    /* is file a URL that requires UDC? */
    bool isUrl(const std::string alignmentPath);

    /* get the file size from the OS */
    size_t getFileStatSize(int fd);

    /* map of character to encoding for both upper and lower case */
    extern const uint8_t dnaPackMap[256];

    /* map of 4-bit encoding to character */
    extern const char dnaUnpackMap[16];

    /** Unpack a DNA character.  It stored two characters per byte (one per
     *  nibble). Bit 1 is set for capital letter bits 2,3,4 determine character
     *  in the order (a,c,g,t,n).
    */
    inline char dnaUnpack(hal_index_t index, unsigned char packedChar) {
        uint8_t code = (index & 1) ? (packedChar & 0x0F) : (packedChar >> 4);
        return dnaUnpackMap[code];
    }

    /** Pack a DNA character */
    inline unsigned char dnaPack(char unpackedChar, hal_index_t index, unsigned char packedChar) {
        uint8_t code = dnaPackMap[uint8_t(unpackedChar)];
        return (index & 1) ? ((packedChar & 0xF0) | code) : ((packedChar & 0x0F) | (code << 4));
    }
}

#endif
// Local Variables:
// mode: c++
// End:
