// -*- mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//
// --------------------------------------------------------------------------
//                   OpenMS Mass Spectrometry Framework
// --------------------------------------------------------------------------
//  Copyright (C) 2003-2011 -- Oliver Kohlbacher, Knut Reinert
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// --------------------------------------------------------------------------
// $Maintainer: Hannes Roest $
// $Authors: Hannes Roest $
// --------------------------------------------------------------------------


#include <OpenMS/CONCEPT/ClassTest.h>
#include <OpenMS/FORMAT/TraMLFile.h>

#define private public
#include <OpenMS/ANALYSIS/OPENSWATH/ChromatogramExtractor.h>

using namespace OpenMS;
using namespace std;

START_TEST(ChromatogramExtractor, "$Id$")

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

START_SECTION((void extractChromatograms(const ExperimentType & input, ExperimentType & output, OpenMS::TargetedExperiment & transition_exp, double & extract_window, bool ppm)))
{
  double extract_window = 0.05;
  PeakMap exp;
  PeakMap out_exp;
  TargetedExperiment transitions;
  MzMLFile().load(OPENMS_GET_TEST_DATA_PATH("ChromatogramExtractor_input.mzML"), exp);
  TraMLFile().load(OPENMS_GET_TEST_DATA_PATH("ChromatogramExtractor_input.TraML"), transitions);

  TEST_EQUAL(transitions.getProteins().size(), 1)

  TEST_EQUAL(transitions.getPeptides().size(), 2)
  TEST_EQUAL(transitions.getPeptides()[0].sequence, "PEPTIDEA")
  TEST_EQUAL(transitions.getPeptides()[1].sequence, "PEPTIDEB")

  TargetedExperiment::Peptide firstpeptide = transitions.getPeptides()[0];
  TEST_EQUAL(firstpeptide.rts.size(), 1);
  TEST_EQUAL(firstpeptide.rts[0].getCVTerms().count("MS:1000896"), 1);
  TEST_EQUAL(firstpeptide.rts[0].getCVTerms()["MS:1000896"].size(), 1);

  OpenMS::DataValue v = firstpeptide.rts[0].getCVTerms()["MS:1000896"][0].getValue();
  if(v.valueType() == 0) {  //data value is a string, e.g. "1042.42" and needs to be converted to double
    TEST_EQUAL( (String(v)).toDouble(), 44);
  }
  else { 
    TEST_EQUAL( (double)v, 44);
  }

  TEST_EQUAL(transitions.getTransitions().size(), 3)
  TEST_EQUAL(transitions.getTransitions()[0].getPrecursorMZ(), 500)
  TEST_EQUAL(transitions.getTransitions()[0].getProductMZ(), 628.45)
  TEST_EQUAL(transitions.getTransitions()[0].getLibraryIntensity(), 1)

  TEST_EQUAL(transitions.getTransitions()[1].getPrecursorMZ(), 500)
  TEST_EQUAL(transitions.getTransitions()[1].getProductMZ(), 654.38)
  TEST_EQUAL(transitions.getTransitions()[1].getLibraryIntensity(), 2)

  TEST_EQUAL(transitions.getTransitions()[2].getPrecursorMZ(), 501)
  TEST_EQUAL(transitions.getTransitions()[2].getProductMZ(), 618.31)
  TEST_EQUAL(transitions.getTransitions()[2].getLibraryIntensity(), 10000)

  ///////////////////////////////////////////////////////////////////////////
  ChromatogramExtractor extractor;
  TransformationDescription trafo;
#ifdef USE_SP_INTERFACE
  SpectrumChromatogramInterface::SpectrumInterface* experiment = getSpectrumInterfaceOpenMSPtr(exp);
  extractor.extractChromatograms(*experiment, out_exp, transitions, extract_window, false, trafo, -1, "tophat");
  delete experiment;
#else
  extractor.extractChromatograms(exp, out_exp, transitions, extract_window, false, trafo, -1, "tophat");
#endif

  TEST_EQUAL(out_exp.size(), 0)
  TEST_EQUAL(out_exp.getChromatograms().size(), 3)

  MSChromatogram<ChromatogramPeak> chrom = out_exp.getChromatograms()[0];

  TEST_EQUAL(chrom.size(), 59);
  // we sort/reorder 
  int firstchromat  = 1;
  int secondchromat = 2;
  int thirdchromat  = 0;

  double max_value = -1; double foundat = -1;
  chrom = out_exp.getChromatograms()[firstchromat];
  for(MSChromatogram<ChromatogramPeak>::iterator it = chrom.begin(); it != chrom.end(); it++)
  {
    if(it->getIntensity() > max_value)
    {
      max_value = it->getIntensity();
      foundat = it->getRT();
    }
  }
  TEST_REAL_SIMILAR(max_value, 169.792);
  TEST_REAL_SIMILAR(foundat, 3120.26);

  max_value = -1; foundat = -1;
  chrom = out_exp.getChromatograms()[secondchromat];
  for(MSChromatogram<ChromatogramPeak>::iterator it = chrom.begin(); it != chrom.end(); it++)
  {
    if(it->getIntensity() > max_value)
    {
      max_value = it->getIntensity();
      foundat = it->getRT();
    }
  }

  TEST_REAL_SIMILAR(max_value, 577.33);
  TEST_REAL_SIMILAR(foundat, 3120.26);

  max_value = -1; foundat = -1;
  chrom = out_exp.getChromatograms()[thirdchromat];
  for(MSChromatogram<ChromatogramPeak>::iterator it = chrom.begin(); it != chrom.end(); it++)
  {
    if(it->getIntensity() > max_value)
    {
      max_value = it->getIntensity();
      foundat = it->getRT();
    }
  }

  TEST_REAL_SIMILAR(max_value, 35.593);
  TEST_REAL_SIMILAR(foundat, 3055.16);


}
END_SECTION

//  mz_a = [400+0.01*i for i in range(20)]
//  int_a = [0 + i*100.0 for i in range(10)] + [900 - i*100.0 for i in range(10)]
static const double mz_arr[] = {
  400.0 ,
  400.01,
  400.02,
  400.03,
  400.04,
  400.05,
  400.06,
  400.07,
  400.08,
  400.09,
  400.1 ,
  400.11,
  400.12,
  400.13,
  400.14,
  400.15,
  400.16,
  400.17,
  400.18,
  400.19,
  450.0,
  500.0,
};
static const double int_arr[] = {
  0.0  , 
  100.0,
  200.0,
  300.0,
  400.0,
  500.0,
  600.0,
  700.0,
  800.0,
  900.0,
  900.0,
  800.0,
  700.0,
  600.0,
  500.0,
  400.0,
  300.0,
  200.0,
  100.0,
  0.0, 
  10.0, 
  10.0, 
};

START_SECTION((void extract_value_tophat(const SpectrumType & input, const double & mz, Size & peak_idx, double & integrated_intensity, const double & extract_window, const bool ppm)))
{
  std::vector<double> mz (mz_arr, mz_arr + sizeof(mz_arr) / sizeof(mz_arr[0]) );
  std::vector<double> intensities (int_arr, int_arr + sizeof(int_arr) / sizeof(int_arr[0]) );

  // conver the data into a spectrum
  MSSpectrum<Peak1D> spectrum;
  for(Size i=0; i<mz.size(); ++i)
  {
    Peak1D peak;
    peak.setMZ(mz[i]);
    peak.setIntensity(intensities[i]);
    spectrum.push_back(peak);
  }

  Size peak_idx = 0;
  double integrated_intensity = 0;
  double extract_window = 0.2; // +/- 0.1

  // If we use monotonically increasing m/z values then everything should work fine
  ChromatogramExtractor extractor;
  extractor.extract_value_tophat(spectrum, 399.91, peak_idx, integrated_intensity, extract_window, false);
  TEST_REAL_SIMILAR( integrated_intensity,100.0);
  extractor.extract_value_tophat(spectrum, 400.0, peak_idx, integrated_intensity, extract_window, false);
  // print(sum([0 + i*100.0 for i in range(10)]) )
  TEST_REAL_SIMILAR( integrated_intensity,4500.0);
  extractor.extract_value_tophat(spectrum, 400.05, peak_idx, integrated_intensity, extract_window, false);
  //print(sum([0 + i*100.0 for i in range(10)]) + sum([900 - i*100.0 for i in range(6)])  )
  TEST_REAL_SIMILAR( integrated_intensity,8400.0);
  extractor.extract_value_tophat(spectrum, 400.1, peak_idx, integrated_intensity, extract_window, false);
  //print(sum([0 + i*100.0 for i in range(10)]) + sum([900 - i*100.0 for i in range(10)])  )
  TEST_REAL_SIMILAR( integrated_intensity,9000.0);
  TEST_EQUAL((int)integrated_intensity,9000);
  extractor.extract_value_tophat(spectrum, 400.28, peak_idx, integrated_intensity, extract_window, false);
  TEST_REAL_SIMILAR( integrated_intensity,100.0);
  extractor.extract_value_tophat(spectrum, 500.0, peak_idx, integrated_intensity, extract_window, false);
  TEST_REAL_SIMILAR( integrated_intensity, 10.0);

  // this is to document the situation of using m/z values that are not monotonically increasing:
  //  --> it might not give the correct result (9000) if we try to extract 400.1 AFTER 500.0 
  extractor.extract_value_tophat(spectrum, 400.1, peak_idx, integrated_intensity, extract_window, false);
  TEST_NOT_EQUAL((int)integrated_intensity,9000);

  /// use ppm extraction windows
  //
  peak_idx = 0;
  integrated_intensity = 0;
  extract_window = 500; // 500 ppm == 0.2 Da @ 400 m/z

  extractor.extract_value_tophat(spectrum, 399.91, peak_idx, integrated_intensity, extract_window, true);
  TEST_REAL_SIMILAR( integrated_intensity,0.0);  // below 400, 500ppm is below 0.2 Da...
  extractor.extract_value_tophat(spectrum, 399.92, peak_idx, integrated_intensity, extract_window, true);
  TEST_REAL_SIMILAR( integrated_intensity,100.0); 
  extractor.extract_value_tophat(spectrum, 400.0, peak_idx, integrated_intensity, extract_window, true);
  TEST_REAL_SIMILAR( integrated_intensity,4500.0);
  extractor.extract_value_tophat(spectrum, 400.05, peak_idx, integrated_intensity, extract_window, true);
  TEST_REAL_SIMILAR( integrated_intensity,8400.0);
  extractor.extract_value_tophat(spectrum, 400.1, peak_idx, integrated_intensity, extract_window, true);
  TEST_REAL_SIMILAR( integrated_intensity,9000.0);

}
END_SECTION

START_SECTION((void extract_value_bartlett(const SpectrumType & input, const double & mz, Size & peak_idx, double & integrated_intensity, const double & extract_window, const bool ppm)))
{
  std::vector<double> mz (mz_arr, mz_arr + sizeof(mz_arr) / sizeof(mz_arr[0]) );
  std::vector<double> intensities (int_arr, int_arr + sizeof(int_arr) / sizeof(int_arr[0]) );

  // conver the data into a spectrum
  MSSpectrum<Peak1D> spectrum;
  for(Size i=0; i<mz.size(); ++i)
  {
    Peak1D peak;
    peak.setMZ(mz[i]);
    peak.setIntensity(intensities[i]);
    spectrum.push_back(peak);
  }

  Size peak_idx = 0;
  double integrated_intensity = 0;
  double extract_window = 0.2; // +/- 0.1

  /*
   * Python code to replicate (use mz_a and int_a from above):
   *
  win = 0.1
  center = 400.1
  #win = center * 250 *  1.0e-6 # for ppm
  data = [ (m,i) for m,i in zip(mz_a, int_a) if m >= center - win and m <= center  + win]
  triangle(data, center, win)

  def triangle(data, center, win):
    s = 0
    for d in data:
      weight =  1 - abs(d[0] - center) / win;
      print weight, d[1]
      s += weight * d[1]
    return s
  */

  // If we use monotonically increasing m/z values then everything should work fine
  ChromatogramExtractor extractor;
  extractor.extract_value_bartlett(spectrum, 399.91, peak_idx, integrated_intensity, extract_window, false);
  TEST_REAL_SIMILAR( integrated_intensity,0.0);
  extractor.extract_value_bartlett(spectrum, 400.0, peak_idx, integrated_intensity, extract_window, false);
  TEST_REAL_SIMILAR( integrated_intensity,1650.0);
  extractor.extract_value_bartlett(spectrum, 400.05, peak_idx, integrated_intensity, extract_window, false);
  TEST_REAL_SIMILAR( integrated_intensity,4650.0);
  extractor.extract_value_bartlett(spectrum, 400.1, peak_idx, integrated_intensity, extract_window, false);
  TEST_REAL_SIMILAR( integrated_intensity,6150.0);
  extractor.extract_value_bartlett(spectrum, 400.28, peak_idx, integrated_intensity, extract_window, false);
  TEST_REAL_SIMILAR( integrated_intensity,0.0);
  extractor.extract_value_bartlett(spectrum, 500.0, peak_idx, integrated_intensity, extract_window, false);
  TEST_REAL_SIMILAR( integrated_intensity, 10.0);

  // this is to document the situation of using m/z values that are not monotonically increasing:
  //  --> it might not give the correct result (9000) if we try to extract 400.1 AFTER 500.0 
  extractor.extract_value_bartlett(spectrum, 400.1, peak_idx, integrated_intensity, extract_window, false);
  TEST_NOT_EQUAL((int)integrated_intensity,9000);

  /// use ppm extraction windows
  //
  peak_idx = 0;
  integrated_intensity = 0;
  extract_window = 500; // 500 ppm == 0.2 Da @ 400 m/z

  extractor.extract_value_bartlett(spectrum, 399.91, peak_idx, integrated_intensity, extract_window, true);
  TEST_REAL_SIMILAR( integrated_intensity,0.0);  // below 400, 500ppm is below 0.2 Da...
  extractor.extract_value_bartlett(spectrum, 399.92, peak_idx, integrated_intensity, extract_window, true);
  TEST_REAL_SIMILAR( integrated_intensity,9.98199639930487); 
  extractor.extract_value_bartlett(spectrum, 400.0, peak_idx, integrated_intensity, extract_window, true);
  TEST_REAL_SIMILAR( integrated_intensity,1650.0);
  extractor.extract_value_bartlett(spectrum, 400.05, peak_idx, integrated_intensity, extract_window, true);
  TEST_REAL_SIMILAR( integrated_intensity,4650.4687);
  extractor.extract_value_bartlett(spectrum, 400.1, peak_idx, integrated_intensity, extract_window, true);
  TEST_REAL_SIMILAR( integrated_intensity,6150.7123219188725);

}
END_SECTION

START_SECTION(void extract_value_tophat(const std::vector<double>::const_iterator & mz_start, std::vector<double>::const_iterator & mz_it,
    const std::vector<double>::const_iterator & mz_end, std::vector<double>::const_iterator & int_it,
    const double & mz, double & integrated_intensity, double & extract_window, bool ppm))
{ 
  std::vector<double> mz (mz_arr, mz_arr + sizeof(mz_arr) / sizeof(mz_arr[0]) );
  std::vector<double> intensities (int_arr, int_arr + sizeof(int_arr) / sizeof(int_arr[0]) );

  // conver the data into a spectrum
  MSSpectrum<Peak1D> spectrum;
  for(Size i=0; i<mz.size(); ++i)
  {
    Peak1D peak;
    peak.setMZ(mz[i]);
    peak.setIntensity(intensities[i]);
    spectrum.push_back(peak);
  }

  std::vector<double>::const_iterator mz_start = mz.begin();
  std::vector<double>::const_iterator mz_it_end = mz.end();
  std::vector<double>::const_iterator mz_it = mz.begin();
  std::vector<double>::const_iterator int_it = intensities.begin();

  double integrated_intensity = 0;
  double extract_window = 0.2; // +/- 0.1

  // If we use monotonically increasing m/z values then everything should work fine
  ChromatogramExtractor extractor;
  extractor.extract_value_tophat(mz_start, mz_it, mz_it_end, int_it, 399.91, integrated_intensity, extract_window, false);
  TEST_REAL_SIMILAR( integrated_intensity,100.0);
  extractor.extract_value_tophat(mz_start, mz_it, mz_it_end, int_it, 400.0, integrated_intensity, extract_window, false);
  // print(sum([0 + i*100.0 for i in range(10)]) )
  TEST_REAL_SIMILAR( integrated_intensity,4500.0);
  extractor.extract_value_tophat(mz_start, mz_it, mz_it_end, int_it, 400.05,  integrated_intensity, extract_window, false);
  //print(sum([0 + i*100.0 for i in range(10)]) + sum([900 - i*100.0 for i in range(6)])  )
  TEST_REAL_SIMILAR( integrated_intensity,8400.0);
  extractor.extract_value_tophat(mz_start, mz_it, mz_it_end, int_it, 400.1, integrated_intensity, extract_window, false);
  //print(sum([0 + i*100.0 for i in range(10)]) + sum([900 - i*100.0 for i in range(10)])  )
  TEST_REAL_SIMILAR( integrated_intensity,9000.0);
  TEST_EQUAL((int)integrated_intensity,9000);
  extractor.extract_value_tophat(mz_start, mz_it, mz_it_end, int_it, 400.28, integrated_intensity, extract_window, false);
  TEST_REAL_SIMILAR( integrated_intensity,100.0);
  extractor.extract_value_tophat(mz_start, mz_it, mz_it_end, int_it, 500.0, integrated_intensity, extract_window, false);
  TEST_REAL_SIMILAR( integrated_intensity, 10.0);

  // this is to document the situation of using m/z values that are not monotonically increasing:
  //  --> it might not give the correct result (9000) if we try to extract 400.1 AFTER 500.0 
  extractor.extract_value_tophat(mz_start, mz_it, mz_it_end, int_it, 400.1, integrated_intensity, extract_window, false);
  TEST_NOT_EQUAL((int)integrated_intensity,9000);

  /// use ppm extraction windows
  //

  mz_it = mz.begin();
  int_it = intensities.begin();
  integrated_intensity = 0;
  extract_window = 500; // 500 ppm == 0.2 Da @ 400 m/z

  extractor.extract_value_tophat(mz_start, mz_it, mz_it_end, int_it, 399.91, integrated_intensity, extract_window, true);
  TEST_REAL_SIMILAR( integrated_intensity,0.0);  // below 400, 500ppm is below 0.2 Da...
  extractor.extract_value_tophat(mz_start, mz_it, mz_it_end, int_it, 399.92, integrated_intensity, extract_window, true);
  TEST_REAL_SIMILAR( integrated_intensity,100.0); 
  extractor.extract_value_tophat(mz_start, mz_it, mz_it_end, int_it, 400.0, integrated_intensity, extract_window, true);
  TEST_REAL_SIMILAR( integrated_intensity,4500.0);
  extractor.extract_value_tophat(mz_start, mz_it, mz_it_end, int_it, 400.05, integrated_intensity, extract_window, true);
  TEST_REAL_SIMILAR( integrated_intensity,8400.0);
  extractor.extract_value_tophat(mz_start, mz_it, mz_it_end, int_it, 400.1, integrated_intensity, extract_window, true);
  TEST_REAL_SIMILAR( integrated_intensity,9000.0);

}
END_SECTION

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
END_TEST


