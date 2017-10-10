#include "RecordFile.h"
#include "RecordIterator.h"

TraceRecord RecordIterator::operator*() { Record r = _file->get(_idx); auto i = *(TraceRecord*)&r; return i; }


bool operator==(const RecordIterator &a, const RecordIterator &b) {
	return a._file == b._file && a._idx == b._idx;
}
bool operator!=(const RecordIterator &a, const RecordIterator &b) {
	return !(a == b);
}

