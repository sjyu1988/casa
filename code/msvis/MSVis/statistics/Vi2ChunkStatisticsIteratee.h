// -*- mode: c++ -*-
#ifndef VI2_CHUNK_STATISTICS_ITERATEE_H_
#define VI2_CHUNK_STATISTICS_ITERATEE_H_

#include <msvis/MSVis/VisBuffer2.h>
#include <casacore/scimath/Mathematics/StatisticsAlgorithm.h>
#include <unordered_map>

namespace casa {

// This class is used to encapsulate the action(s) taken on a sequence of
// casacore::StatisticsAlgorithm instances that are generated by the
// Vi2ChunkDataProvider::foreachDataset() method, which can be used to compute
// statistics for each dataset of possibly merged casacore::MS chunks provided by
// a VisibilityIterator2 instance. See the Vi2ChunkDataProvider.h file for an
// outline of how this class may be used.

template <class DataIterator, class WeightsIterator, class MaskIterator>
class Vi2ChunkStatisticsIteratee {

	typedef typename DataIterator::AccumType AccumType;

public:

	// This method will be called by Vi2ChunkDataProvider::foreachDataset() for
	// each dataset composed of one or more chunks provided by a
	// VisibilityIterator2 instance, with the casacore::StatisticsAlgorithm
	// instance initialized with a data provider for the dataset. The pointer to
	// the VisBuffer2 instance is provided in case this method needs to get some
	// metadata for the current chunk.
	virtual void nextDataset(
		casacore::StatisticsAlgorithm<AccumType,DataIterator,MaskIterator,WeightsIterator> &stats,
		const std::unordered_map<int,std::string> *columnValues) = 0;
};

} // namespace casa

#endif // VI2_CHUNK_STATISTICS_ITERATEE_H_
