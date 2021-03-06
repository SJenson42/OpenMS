// --------------------------------------------------------------------------
//                   OpenMS -- Open-Source Mass Spectrometry
// --------------------------------------------------------------------------
// Copyright The OpenMS Team -- Eberhard Karls University Tuebingen,
// ETH Zurich, and Freie Universitaet Berlin 2002-2013.
//
// This software is released under a three-clause BSD license:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of any author or any participating institution
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
// For a full list of authors, refer to the file AUTHORS.
// --------------------------------------------------------------------------
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Hannes Roest $
// $Authors: Hannes Roest $
// --------------------------------------------------------------------------

#ifndef OPENMS_INTERFACES_IMSDATACONSUMER_H
#define OPENMS_INTERFACES_IMSDATACONSUMER_H

#include <OpenMS/INTERFACES/DataStructures.h>
#include <OpenMS/KERNEL/MSExperiment.h>
#include <OpenMS/KERNEL/MSSpectrum.h>
#include <OpenMS/KERNEL/MSChromatogram.h>

#include <vector>
#include <string>
#include <iostream>
#include <boost/shared_ptr.hpp>

namespace OpenMS
{
namespace Interfaces
{

    /**
      @brief The interface of a consumer of spectra

      The spectra consumer is able to consume MSSpectrum and MSChromatogram and
      process them. It also expects to be informed about the number of spectra
      and chromatograms that it needs to consume and about the
      ExperimentalSettings _before_ consuming any spectra. Depending on the
      implementation, an exception may occur if the ExperimentalSettings and
      the size of the experiment are not set before consuming any spectra. 

    */
    template <typename MapType = MSExperiment<> >
    class OPENMS_DLLAPI IMSDataConsumer
    {
    public:
      typedef typename MapType::SpectrumType SpectrumType;
      typedef typename MapType::ChromatogramType ChromatogramType;

      // IMSDataConsumer() = 0;
      virtual ~IMSDataConsumer() {};
      virtual void consumeSpectrum(SpectrumType & s) = 0;
      virtual void consumeChromatogram(ChromatogramType &) = 0;

      // for some applications its very important to know about the meta-data
      // of the experiment, such as the number of spectra and chromatograms
      // and the experimental settings.
      virtual void setExpectedSize(Size expectedSpectra, Size expectedChromatograms) = 0;
      virtual void setExperimentalSettings(ExperimentalSettings& exp) = 0;
    };

} //end namespace Interfaces
} //end namespace OpenMS

#endif
