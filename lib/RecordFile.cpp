#include "RecordFile.h"

RecordIterator RecordFile::begin() { return RecordIterator(this, 0); }
RecordIterator RecordFile::end() { return RecordIterator(this, _count); }
