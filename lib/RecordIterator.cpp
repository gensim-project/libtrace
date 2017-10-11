#include "libtrace/RecordFile.h"
#include "libtrace/RecordIterator.h"

using namespace libtrace;

TraceRecord RecordIterator::operator*() { Record r = _file->get(_idx); auto i = *(TraceRecord*)&r; return i; }


bool RecordIterator::operator==(const RecordIterator &other) {
	return _file == other._file && _idx == other._idx;
}
bool RecordIterator::operator!=(const RecordIterator &other) {
	return !(*this == other);
}

