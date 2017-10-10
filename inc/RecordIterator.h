#ifndef RECORDITERATOR_H
#define RECORDITERATOR_H

#include <cstdint>
#include "records.h"

class TraceRecord;
class RecordFile;
class RecordIterator;

class RecordIterator
{
public:
	RecordIterator(RecordFile *file, uint64_t start_idx) : _file(file), _idx(start_idx) {}

	friend bool operator==(const RecordIterator &a, const RecordIterator &b);

	TraceRecord operator*();
	
	// Preincrement operator
	RecordIterator operator++() { _idx++; return *this; }
	
	// Postincrement operator
	RecordIterator operator++(int) { auto temp = RecordIterator(_file, _idx); _idx++; return temp; }
	
	uint64_t index() { return _idx; }
private:
	uint64_t _idx;
	RecordFile *_file;
};

bool operator==(const RecordIterator &a, const RecordIterator &b);
bool operator!=(const RecordIterator &a, const RecordIterator &b);
#endif
