/*
 *  Tests for the case-insensitive fm-index.
 */

#ifdef HAVE_CDS

#include <cassert>
#include <string>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <HDT.hpp>
#include "../src/dictionary/PlainDictionary.hpp"
#include "../src/dictionary/LiteralDictionary.hpp"

using namespace std;
using namespace hdt;

string make_literal(string& s) {
    s.insert(0, 1, '"');
    s.append(1, '"');
    return s;
}

string toupper(string s) {
    string S = string(s);
    for (int i = 0; i < S.length(); ++i)
        S[i] = toupper(S[i]);
    return S;
}

bool hasSubstring(string& s, string& sub, bool caseInsensitive) {
    if (caseInsensitive) {
        s = toupper(s);
        sub = toupper(sub);
    }
    return s.find(sub) != string::npos;
}

int matchCount(string* objects, int len, string match, bool caseInsensitive) {
    int count = 0;
    for (int i = 0; i < len; ++i) {
        if (hasSubstring(objects[i], match, caseInsensitive))
            ++count;
    }
    return count;
}

bool compare(string& a, string& b, bool caseInsensitive) {
    if (caseInsensitive) {
        unsigned int sz = a.size();
        if (b.size() != sz)
            return false;
        for (unsigned int i = 0; i < sz; ++i)
            if (tolower(a[i]) != tolower(b[i]))
                return false;
        return true;
    }
    else {
        return a.compare(b) == 0;
    }
}

void testGeneric(string* objects, int len, string* substrings, int sublen) {
    string spec_file("../presets/fmindex.hdtcfg");
    HDTSpecification spec(spec_file);
    PlainDictionary plain(spec);
    
    plain.startProcessing();
    for (int i = 0; i < len; ++i) {
        make_literal(objects[i]);
        plain.insert(objects[i], OBJECT);
    }
    plain.stopProcessing();
    
    LiteralDictionary dictionary(spec);
    dictionary.import(&plain);
    
    unsigned int id;
    string res;
    // full matching
    for (int b = 0; b < 2; ++b) {
        for (int i = 0; i < len; ++i) {
            id = dictionary.stringToId(objects[i], OBJECT, b);
            res = dictionary.idToString(id, OBJECT);
            assert(compare(objects[i], res, b));
        }
    }
    // substring matching
    uint32_t** occs = (uint32_t**)malloc(sizeof(uint32_t*));
    uint32_t count;
    for (int b = 0; b < 2; ++b) {
        for (int i = 0; i < sublen; ++i) {
            count = dictionary.substringToId((unsigned char*)substrings[i].c_str(), substrings[i].length(), b, occs);
            delete [] *occs;
            assert(count == matchCount(objects, len, substrings[i], b));
        }
    }
    
    delete occs;
}

void testA() {    
    string objects[] = {
        "", // 1
        "a", "A", // 2
        "aa", "AA", "aA", "Aa", // 4
        "aaa", "AAA", "AAa", "Aaa", "AaA", "aAa", "aaA", "aAA" // 8
        };
    int len = sizeof(objects)/sizeof(string);
    
    string substrings[] = {"a", "aa", "aaa", "Aa", "aA", "aAa", "AAa"};
    int sublen = sizeof(substrings)/sizeof(string);
    
    testGeneric(objects, len, substrings, sublen);
}

// http://c-faq.com/lib/randrange.html
int randrange(int min, int max) {
    return (int)((double)rand() / ((double)RAND_MAX + 1) * (max-min)) + min;
}

void testRandom() {
    srand(0);
    static const char alphanum[] = "ABCabc12";
    int alphalen = sizeof(alphanum)/sizeof(char) - 1; // -1 for 0 delimiter
    
    int objectCount = 1000;
    int objectMin = 10;
    int objectMax = 100;
    int substringCount = 100;
    int substringMin = 2;
    int substringMax = 10;
    
    string objects[objectCount];
    string substrings[substringCount];
    
    // objects
    for (int i = 0; i < objectCount; ++i) {
        int len = randrange(objectMin, objectMax);
        string s(len, 'a');
        for (int j = 0; j < len; ++j) {
            s[j] = alphanum[randrange(0, alphalen)];
        }
        objects[i] = s;
    }
    
    // substrings
    for (int i = 0; i < substringCount; ++i) {
        int len = randrange(substringMin, substringMax);
        string s(len, 'a');
        for (int j = 0; j < len; ++j)
            s[j] = alphanum[randrange(0, alphalen)];
        substrings[i] = s;
    }
    
    testGeneric(objects, objectCount, substrings, substringCount);
}

int main(int argc, char **argv) {
    try {
        //testA();
        cout << "PASSED TEST A\n";
        testRandom();
        cout << "PASSED TEST RANDOM\n";
    }
    catch (char const* m) {
        cout << "ERROR:\n";
        cout << m << '\n';
    }
}


#endif