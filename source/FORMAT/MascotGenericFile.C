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
// $Maintainer: Andreas Bertsch $
// $Authors: Andreas Bertsch $
// --------------------------------------------------------------------------

#include <OpenMS/FORMAT/MascotGenericFile.h>
#include <OpenMS/CHEMISTRY/ModificationsDB.h>
#include <QFileInfo>
#include <QtCore/QRegExp>

using namespace std;

namespace OpenMS
{

  MascotGenericFile::MascotGenericFile() :
    ProgressLogger(),
    DefaultParamHandler("MascotGenericFile")
  {
    defaults_.setValue("database", "MSDB", "Name of the sequence database");
    defaults_.setValue("search_type", "MIS", "Name of the search type for the query", StringList::create("advanced"));
    defaults_.setValidStrings("search_type", StringList::create("MIS,SQ,PMF"));
    defaults_.setValue("enzyme", "Trypsin", "The enzyme descriptor to the enzyme used for digestion. (Trypsin is default, None would be best for peptide input or unspecific digestion, for more please refer to your mascot server).");
    defaults_.setValue("instrument", "Default", "Instrument definition which specifies the fragmentation rules");
    defaults_.setValue("missed_cleavages", 1, "Number of missed cleavages allowed for the enzyme");
    defaults_.setMinInt("missed_cleavages", 0);
    defaults_.setValue("precursor_mass_tolerance", 3.0, "Tolerance of the precursor peaks");
    defaults_.setMinFloat("precursor_mass_tolerance", 0.0);
    defaults_.setValue("precursor_error_units", "Da", "Units of the precursor mass tolerance");
    defaults_.setValidStrings("precursor_error_units", StringList::create("%,ppm,mmu,Da"));
    defaults_.setValue("fragment_mass_tolerance", 0.3, "Tolerance of the peaks in the fragment spectrum");
    defaults_.setMinFloat("fragment_mass_tolerance", 0.0);
    defaults_.setValue("fragment_error_units", "Da", "Units of the fragment peaks tolerance");
    defaults_.setValidStrings("fragment_error_units", StringList::create("mmu,Da"));
    defaults_.setValue("charges", "1,2,3", "Allowed charge states, given as a comma separated list of integers");
    defaults_.setValue("taxonomy", "All entries", "Taxonomy specification of the sequences");
    vector<String> all_mods;
    ModificationsDB::getInstance()->getAllSearchModifications(all_mods);
    defaults_.setValue("fixed_modifications", StringList::create(""), "List of fixed modifications, according to UniMod definitions.");
    defaults_.setValidStrings("fixed_modifications", all_mods);
    defaults_.setValue("variable_modifications", StringList::create(""), "Variable modifications given as UniMod definitions.");
    defaults_.setValidStrings("variable_modifications", all_mods);

    defaults_.setValue("mass_type", "monoisotopic", "Defines the mass type, either monoisotopic or average");
    defaults_.setValidStrings("mass_type", StringList::create("monoisotopic,average"));
    defaults_.setValue("number_of_hits", 0, "Number of hits which should be returned, if 0 AUTO mode is enabled.");
    defaults_.setMinInt("number_of_hits", 0);
    defaults_.setValue("skip_spectrum_charges", "false", "Sometimes precursor charges are given for each spectrum but are wrong, setting this to 'true' does not write any charge information to the spectrum, the general charge information is however kept.");
    defaults_.setValidStrings("skip_spectrum_charges", StringList::create("true,false"));

    defaults_.setValue("search_title", "OpenMS_search", "Sets the title of the search.", StringList::create("advanced"));
    defaults_.setValue("username", "OpenMS", "Sets the username which is mentioned in the results file.", StringList::create("advanced"));
    defaults_.setValue("email", "", "Sets the email which is mentioned in the results file. Note: Some server require that a proper email is provided.");

    // the next section should not be shown to TOPP users
    Param p;
    p.setValue("format", "Mascot generic", "Sets the format type of the peak list, this should not be changed unless you write the header only.", StringList::create("advanced"));
    p.setValidStrings("format", StringList::create("Mascot generic,mzData (.XML),mzML (.mzML)")); // Mascot's HTTP interface supports more, but we don't :)
    p.setValue("boundary", "GZWgAaYKjHFeUaLOLEIOMq", "MIME boundary for parameter header (if using HTTP format)", StringList::create("advanced"));
    p.setValue("HTTP_format", "false", "Write header with MIME boundaries instead of simple key-value pairs. For HTTP submission only.", StringList::create("advanced"));
    p.setValidStrings("HTTP_format", StringList::create("true,false"));
    p.setValue("content", "all", "Use parameter header + the peak lists with BEGIN IONS... or only one of them.", StringList::create("advanced"));
    p.setValidStrings("content", StringList::create("all,peaklist_only,header_only"));
    defaults_.insert("internal:", p);

    defaultsToParam_();
  }

  MascotGenericFile::~MascotGenericFile()
  {

  }

  void MascotGenericFile::store(const String& filename, const PeakMap& experiment)
  {
    if (!File::writable(filename))
    {
      throw Exception::FileNotWritable(__FILE__, __LINE__, __PRETTY_FUNCTION__, filename);
    }
    ofstream os(filename.c_str());
    store(os, filename, experiment);
    os.close();
  }

  void MascotGenericFile::store(ostream& os, const String& filename, const PeakMap& experiment)
  {
    if (param_.getValue("internal:content") != "peaklist_only")
      writeHeader_(os);
    if (param_.getValue("internal:content") != "header_only")
      writeMSExperiment_(os, filename, experiment);
  }

  void MascotGenericFile::writeParameterHeader_(const String& name, ostream& os)
  {
    if (param_.getValue("internal:HTTP_format") == "true")
    {
      os << "--" << param_.getValue("internal:boundary") << "\n" << "Content-Disposition: form-data; name=\"" << name << "\"" << "\n\n";
    }
    else
    {
      os << name << "=";
    }
  }

  void MascotGenericFile::writeHeader_(ostream& os)
  {
    // search title
    if (param_.getValue("search_title") != "")
    {
      writeParameterHeader_("COM", os);
      os << param_.getValue("search_title") << "\n";
    }

    // user name
    writeParameterHeader_("USERNAME", os);
    os << param_.getValue("username") << "\n";

    // email
    if (!param_.getValue("email").toString().empty())
    {
      writeParameterHeader_("USEREMAIL", os);
      os << param_.getValue("email") << "\n";
    }

    // format
    writeParameterHeader_("FORMAT", os);    // make sure this stays within the first 5 lines of the file, since we use it to recognize our own MGF files in case their file suffix is not MGF
    os << param_.getValue("internal:format") << "\n";

    // precursor mass tolerance unit : Da
    writeParameterHeader_("TOLU", os);
    os << param_.getValue("precursor_error_units") << "\n";

    // ion mass tolerance unit : Da
    writeParameterHeader_("ITOLU", os);
    os << param_.getValue("fragment_error_units") << "\n";

    // format version
    writeParameterHeader_("FORMVER", os);
    os << "1.01" << "\n";

    //db name
    writeParameterHeader_("DB", os);
    os << param_.getValue("database") << "\n";

    // search type
    writeParameterHeader_("SEARCH", os);
    os << param_.getValue("search_type") << "\n";

    // number of peptide candidates in the list
    writeParameterHeader_("REPORT", os);
    UInt num_hits((UInt)param_.getValue("number_of_hits"));
    if (num_hits != 0)
    {
      os << param_.getValue("number_of_hits") << "\n";
    }
    else
    {
      os << "AUTO" << "\n";
    }

    //cleavage enzyme
    writeParameterHeader_("CLE", os);
    os << param_.getValue("enzyme") << "\n";

    //average/monoisotopic
    writeParameterHeader_("MASS", os);
    os << param_.getValue("mass_type") << "\n";

    // @TODO remove "Deamidated (NQ)" special cases below when a general
    // solution for specificity groups in UniMod is implemented (ticket #387) 

    //fixed modifications
    StringList fixed_mods((StringList)param_.getValue("fixed_modifications"));
    for (StringList::const_iterator it = fixed_mods.begin(); it != fixed_mods.end(); ++it)
    {
      if ((*it == "Deamidated (N)") || (*it == "Deamidated (Q)")) continue;
      writeParameterHeader_("MODS", os);
      os << *it << "\n";
    }
    if (fixed_mods.contains("Deamidated (N)") || 
        fixed_mods.contains("Deamidated (Q)"))
    {
      writeParameterHeader_("MODS", os);
      os << "Deamidated (NQ)" << "\n";
    }

    //variable modifications
    StringList var_mods((StringList)param_.getValue("variable_modifications"));
    for (StringList::const_iterator it = var_mods.begin(); it != var_mods.end(); ++it)
    {
      if ((*it == "Deamidated (N)") || (*it == "Deamidated (Q)")) continue;
      writeParameterHeader_("IT_MODS", os);
      os << *it << "\n";
    }
    if (var_mods.contains("Deamidated (N)") || 
        var_mods.contains("Deamidated (Q)"))
    {
      writeParameterHeader_("IT_MODS", os);
      os << "Deamidated (NQ)" << "\n";
    }

    //instrument
    writeParameterHeader_("INSTRUMENT", os);
    os << param_.getValue("instrument") << "\n";

    //missed cleavages
    writeParameterHeader_("PFA", os);
    os << param_.getValue("missed_cleavages") << "\n";

    //precursor mass tolerance
    writeParameterHeader_("TOL", os);
    os << param_.getValue("precursor_mass_tolerance") << "\n";

    //ion mass tolerance_
    writeParameterHeader_("ITOL", os);
    os << param_.getValue("fragment_mass_tolerance") << "\n";

    //taxonomy
    writeParameterHeader_("TAXONOMY", os);
    os << param_.getValue("taxonomy") << "\n";

    //charge
    writeParameterHeader_("CHARGE", os);
    os << param_.getValue("charges") << "\n";
  }

  void MascotGenericFile::writeSpectrum_(ostream& os, const PeakSpectrum& spec, const String& filename)
  {
    Precursor precursor;
    if (spec.getPrecursors().size() > 0)
    {
      precursor = spec.getPrecursors()[0];
    }
    if (spec.getPrecursors().size() > 1)
    {
      cerr << "Warning: The spectrum written to Mascot file has more than one precursor. The first precursor is used!\n";
    }
    if (spec.size() >= 10000)
    {
      throw Exception::InvalidValue(__FILE__, __LINE__, __PRETTY_FUNCTION__, "Spectrum to be written as MGF has more than 10.000 peaks which is"
                                                                             " the maximum upper limit. Only centroided data is allowed. This is most likely raw data.",
                                    String(spec.size()));
    }
    DoubleReal mz(precursor.getMZ()), rt(spec.getRT());

    if (mz == 0)
    {
      //retention time
      cout << "No precursor m/z information for spectrum with rt: " << rt << " present, skipping spectrum!\n";
    }
    else
    {
      os << "\n";
      os << "BEGIN IONS\n";
      os << "TITLE=" << precisionWrapper(mz) << "_" << precisionWrapper(rt) << "_" << spec.getNativeID() << "_" << filename << "\n";
      os << "PEPMASS=" << precisionWrapper(mz) <<  "\n";
      os << "RTINSECONDS=" << precisionWrapper(rt) << "\n";

      bool skip_spectrum_charges(param_.getValue("skip_spectrum_charges").toBool());

      int charge(precursor.getCharge());

      if (charge != 0)
      {
        if (!skip_spectrum_charges)
        {
          os << "CHARGE=" << charge << "\n";
        }
      }

      os << "\n";

      for (PeakSpectrum::const_iterator it = spec.begin(); it != spec.end(); ++it)
      {
        os << precisionWrapper(it->getMZ()) << " " << precisionWrapper(it->getIntensity()) << "\n";
      }
      os << "END IONS\n";
    }
  }

  std::pair<String, String> MascotGenericFile::getHTTPPeakListEnclosure(const String& filename) const
  {
    std::pair<String, String> r;
    r.first = String("--" + String(param_.getValue("internal:boundary")) + "\n" + "Content-Disposition: form-data; name=\"FILE\"; filename=\"" + filename + "\"\n\n");
    r.second = String("\n\n--" + String(param_.getValue("internal:boundary")) + "--\n");
    return r;
  }

  void MascotGenericFile::writeMSExperiment_(ostream& os, const String& filename, const PeakMap& experiment)
  {

    std::pair<String, String> enc = getHTTPPeakListEnclosure(filename);
    if (param_.getValue("internal:HTTP_format").toBool())
    {
      os << enc.first;
    }

    QFileInfo fileinfo(filename.c_str());
    QString filtered_filename = fileinfo.completeBaseName();
    filtered_filename.remove(QRegExp("[^a-zA-Z0-9]"));
    this->startProgress(0, experiment.size(), "storing mascot generic file");
    for (Size i = 0; i < experiment.size(); i++)
    {
      this->setProgress(i);
      if (experiment[i].getMSLevel() == 2)
      {
        writeSpectrum_(os, experiment[i], filtered_filename);
      }
      else if (experiment[i].getMSLevel() == 0)
      {
        LOG_WARN << "MascotGenericFile: MSLevel is set to 0, ignoring this spectrum!" << "\n";
      }
    }
    // close file
    if (param_.getValue("internal:HTTP_format").toBool())
    {
      os << enc.second;
    }
    this->endProgress();
  }

  bool MascotGenericFile::getNextSpectrum_(istream& is, vector<pair<String, String> >& spectrum, UInt& charge, DoubleReal& precursor_mz, DoubleReal& precursor_int, DoubleReal& rt, String& title, Size& line_number)
  {
    bool ok(false);
    spectrum.clear();
    charge = 0;
    precursor_mz = 0;
    precursor_int = 0;
    rt = -1;
    title = "";

    String line;
    // seek to next peak list block
    while (getline(is, line, '\n'))
    {
      ++line_number;

      line.trim(); // remove whitespaces, line-endings etc

      // found peak list block?
      if (line == "BEGIN IONS")
      {
        ok = false;
        while (getline(is, line, '\n'))
        {
          ++line_number;
          line.trim(); // remove whitespaces, line-endings etc

          if (line.empty()) continue;

          if (isdigit(line[0]))
          { // actual data .. this comes first, since its the most common case
            vector<String> split;
            do
            {
              if (line.empty())
              {
                continue;
              }

              //line.substitute('\t', ' ');
              line.split(' ', split);
              if (split.size() >= 2)
              {
                spectrum.push_back(make_pair(split[0], split[1])); // conversion to double is the expensive part, so we defer conversion to MT environment
                // @improvement add meta info e.g. charge, name... (Andreas)
              }
              else
              {
                throw Exception::ParseError(__FILE__, __LINE__, __PRETTY_FUNCTION__, "the line (" + line + ") should contain m/z and intensity value separated by whitespace!", "");
              }
            }
            while (getline(is, line, '\n') && ++line_number && line.trim() != "END IONS"); // line.trim() is important here!

            if (line == "END IONS")
            {
              // found spectrum
              return true;
            }
            else
            {
              throw Exception::ParseError(__FILE__, __LINE__, __PRETTY_FUNCTION__, "Reached end of file. Found \"BEGIN IONS\" but not the corresponding \"END IONS\"!", "");
            }
          }
          else if (line.hasPrefix("PEPMASS"))
          { // parse precursor position
            String tmp = line.substr(8);
            tmp.substitute('\t', ' ');
            vector<String> split;
            tmp.split(' ', split);
            if (split.size() == 1)
            {
              precursor_mz = split[0].trim().toDouble();
            }
            else if (split.size() == 2)
            {
              precursor_mz = split[0].trim().toDouble();
              precursor_int = split[1].trim().toDouble();
            }
            else
            {
              throw Exception::ParseError(__FILE__, __LINE__, __PRETTY_FUNCTION__, "Cannot parse PEPMASS: '" + line + "' in line " + String(line_number) + " (expected 1 or 2 entries, but " + String(split.size()) + " were present!", "");
            }
          }
          else if (line.hasPrefix("CHARGE"))
          {
            String tmp = line.substr(7);
            tmp.remove('+');
            charge = tmp.toInt();
          }
          else if (line.hasPrefix("RTINSECONDS"))
          {
            String tmp = line.substr(12);
            rt = tmp.toDouble();
          }
          else if (line.hasPrefix("TITLE"))
          {
            // test if we have a line like "TITLE= Cmpd 1, +MSn(595.3), 10.9 min"
            if (line.hasSubstring("min"))
            {
              try
              {
                vector<String> split;
                line.split(',', split);
                if (!split.empty())
                {
                  for (Size i = 0; i != split.size(); ++i)
                  {
                    if (split[i].hasSubstring("min"))
                    {
                      vector<String> split2;
                      split[i].trim().split(' ', split2);
                      if (!split2.empty())
                      {
                        rt = split2[0].trim().toDouble() * 60.0;
                      }
                    }
                  }
                }
              }
              catch (Exception::BaseException& /*e*/)
              {
                // just do nothing and write the whole title to spec
                vector<String> split;
                line.split('=', split);
                if (split.size() >= 2)
                {
                  title = split[1];
                }
              }
            }
            else // just write the title as metainfo to the spectrum
            {
              vector<String> split;
              line.split('=', split);
              if (split.size() == 2)
              {
                title = split[1];
              }
              // TODO concatenate the other parts if the title contains additional '=' chars
            }
          }
        }
      }
    }

    return ok;
  }

} // namespace OpenMS
