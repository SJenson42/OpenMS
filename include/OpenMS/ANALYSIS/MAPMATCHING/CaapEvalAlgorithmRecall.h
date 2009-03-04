// -*- mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//
// --------------------------------------------------------------------------
//                   OpenMS Mass Spectrometry Framework
// --------------------------------------------------------------------------
//  Copyright (C) 2003-2008 -- Oliver Kohlbacher, Knut Reinert
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// --------------------------------------------------------------------------
// $Maintainer: Katharina Albers, Clemens Groepl $
// --------------------------------------------------------------------------

#ifndef OPENMS_ANALYSIS_MAPMATCHING_CAAPEVALALGORITHMRECALL_H
#define OPENMS_ANALYSIS_MAPMATCHING_CAAPEVALALGORITHMRECALL_H

#include <OpenMS/ANALYSIS/MAPMATCHING/CaapEvalAlgorithm.h>

namespace OpenMS
{
	/**
		@brief Caap evaluation algorithm to obtain a recall value.
		
		It evaluates an input consensus map with respect to a ground truth.

	  @htmlinclude OpenMS_CaapEvalAlgorithmRecall.parameters
	  
		@ingroup CaapEval
	*/
	class OPENMS_DLLAPI CaapEvalAlgorithmRecall
	 : public CaapEvalAlgorithm
	{
		public:
			/// Default constructor
			CaapEvalAlgorithmRecall();

			/// Destructor
			virtual ~CaapEvalAlgorithmRecall();
			
			/**
				@brief Applies the algorithm
			*/
			virtual void evaluate(const ConsensusMap& consensus_map_in, const ConsensusMap& consensus_map_gt, DoubleReal& out);

			/// Creates a new instance of this class (for Factory)
			static CaapEvalAlgorithm* create()
			{
				return new CaapEvalAlgorithmRecall();
			}
			
			/// Returns the product name (for the Factory)
			static String getProductName()
			{
				return "recall";
			}
			
		private:

			/// Copy constructor intentionally not implemented -> private
			CaapEvalAlgorithmRecall(const CaapEvalAlgorithmRecall& );
			/// Assignment operator intentionally not implemented -> private
			CaapEvalAlgorithmRecall& operator=(const CaapEvalAlgorithmRecall& );
			
	};

} // namespace OpenMS

#endif // OPENMS_ANALYSIS_MAPMATCHING_CAAPEVALALGORITHMRECALL_H
