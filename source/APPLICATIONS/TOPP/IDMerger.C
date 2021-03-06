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
// $Maintainer: Nico Pfeifer $
// $Authors: $
// --------------------------------------------------------------------------

#include <OpenMS/FORMAT/IdXMLFile.h>
#include <OpenMS/METADATA/PeptideIdentification.h>

#include <OpenMS/APPLICATIONS/TOPPBase.h>

using namespace OpenMS;
using namespace std;

//-------------------------------------------------------------
//Doxygen docu
//-------------------------------------------------------------

/**
  @page TOPP_IDMerger IDMerger

  @brief Merges several idXML files into one idXML file.
 
  <center>
  <table>
  <tr>
  <td ALIGN = "center" BGCOLOR="#EBEBEB"> potential predecessor tools </td>
  <td VALIGN="middle" ROWSPAN=3> \f$ \longrightarrow \f$ IDMerger \f$ \longrightarrow \f$</td>
  <td ALIGN = "center" BGCOLOR="#EBEBEB"> potential successor tools </td>
  </tr>
  <tr>
  <td VALIGN="middle" ALIGN = "center" ROWSPAN=1> @ref TOPP_MascotAdapter (or other ID engines) </td>
  <td VALIGN="middle" ALIGN = "center" ROWSPAN=1> @ref TOPP_ConsensusID </td>
  </tr>
  <tr>
  <td VALIGN="middle" ALIGN = "center" ROWSPAN=1> @ref TOPP_IDFileConverter </td>
  <td VALIGN="middle" ALIGN = "center" ROWSPAN=1> @ref TOPP_IDMapper </td>
  </tr>
  </table>
  </center>
 
  The peptide hits and protein hits of the input files will be written into the single output file. In general, the number of idXML files that can be merged into one file is not limited.

  The combination of search engine and processing date/time should be unique for every identification run over all input files. If this is not the case, the date/time of a conflicting run will be increased in steps of seconds until the combination is unique.

  With the @p pepxml_protxml option, results from corresponding PeptideProphet and ProteinProphet runs can be combined. In this case, exactly two idXML files are expected as input: one containing data from a pepXML file, and the other containing data from a protXML file that was created based on the pepXML (meaningful results can only be obtained for matching files!). pepXML or protXML can be converted to idXML with the @ref TOPP_IDFileConverter tool.

  <B>The command line parameters of this tool are:</B>
  @verbinclude TOPP_IDMerger.cli
  <B>INI file documentation of this tool:</B>
  @htmlinclude TOPP_IDMerger.html
*/

// We do not want this class to show up in the docu:
/// @cond TOPPCLASSES

class TOPPIDMerger :
  public TOPPBase
{
public:
  TOPPIDMerger() :
    TOPPBase("IDMerger", "Merges several protein/peptide identification files into one file.")
  {

  }

protected:
  void mergePepXMLProtXML_(StringList filenames, vector<ProteinIdentification> &
                           proteins, vector<PeptideIdentification> & peptides)
  {
    IdXMLFile idxml;
    idxml.load(filenames[0], proteins, peptides);
    vector<ProteinIdentification> pepxml_proteins, protxml_proteins;
    vector<PeptideIdentification> pepxml_peptides, protxml_peptides;

    if (proteins[0].getProteinGroups().empty()) // first idXML contains data from the pepXML
    {
      proteins.swap(pepxml_proteins);
      peptides.swap(pepxml_peptides);
      idxml.load(filenames[1], protxml_proteins, protxml_peptides);
      if (protxml_proteins[0].getProteinGroups().empty())
      {
        throw Exception::InvalidParameter(__FILE__, __LINE__, __PRETTY_FUNCTION__, "None of the input files seems to be derived from a protXML file (information about protein groups is missing).");
      }
    }
    else      // first idXML contains data from the protXML
    {
      proteins.swap(protxml_proteins);
      peptides.swap(protxml_peptides);
      idxml.load(filenames[1], pepxml_proteins, pepxml_peptides);
    }

    if ((protxml_peptides.size() > 1) || (protxml_proteins.size() > 1))
    {
      throw Exception::InvalidParameter(__FILE__, __LINE__, __PRETTY_FUNCTION__, "The idXML derived from a protXML file should contain only one 'ProteinIdentification' and one 'PeptideIdentification' instance.");
    }

    // peptide information comes from the pepXML (additional information in
    // the protXML - adapted peptide hit score, "is_unique", "is_contributing"
    // - is not transferred):
    peptides.swap(pepxml_peptides);

    // prepare scores and coverage values of protein hits from the protXML:
    map<String, pair<DoubleReal, DoubleReal> > hit_values;
    ProteinIdentification & protein = protxml_proteins[0];
    for (vector<ProteinHit>::iterator hit_it = protein.getHits().begin();
         hit_it != protein.getHits().end(); ++hit_it)
    {
      hit_values[hit_it->getAccession()] = make_pair(hit_it->getScore(),
                                                     hit_it->getCoverage());
    }

    // merge protein information:
    proteins.swap(pepxml_proteins);
    for (vector<ProteinIdentification>::iterator prot_it = proteins.begin();
         prot_it != proteins.end(); ++prot_it)
    {
      prot_it->getProteinGroups() = protein.getProteinGroups();
      prot_it->getIndistinguishableProteins() =
        protein.getIndistinguishableProteins();
      // TODO: since a protXML file can integrate data from several protein
      // identification runs, the protein groups/indistinguishable proteins
      // that we write to one identification run could contain references to
      // proteins that are not observed in this run, but in others; also, some
      // protein hits without enough evidence may not occur in the protXML
      // (thus also not in the protein groups) - clean this up?

      prot_it->setScoreType(protein.getScoreType());
      prot_it->setHigherScoreBetter(protein.isHigherScoreBetter());
      prot_it->setSignificanceThreshold(protein.getSignificanceThreshold());

      for (vector<ProteinHit>::iterator hit_it = prot_it->getHits().begin();
           hit_it != prot_it->getHits().end(); ++hit_it)
      {
        map<String, pair<DoubleReal, DoubleReal> >::const_iterator pos =
          hit_values.find(hit_it->getAccession());
        if (pos == hit_values.end())
        {
          hit_it->setScore(-1);
        }
        else
        {
          hit_it->setScore(pos->second.first);
          hit_it->setCoverage(pos->second.second);
        }
      }
    }
  }

  void generateNewId_(const set<String> & used_ids, const String & search_engine,
                      DateTime & date_time, String & new_id)
  {
    do
    {
      date_time = date_time.addSecs(1);
      new_id = search_engine + "_" + date_time.toString(Qt::ISODate);
    }
    while (used_ids.find(new_id) != used_ids.end());
  }

  void registerOptionsAndFlags_()
  {
    registerInputFileList_("in", "<files>", StringList(), "two or more input files separated by blanks");
    setValidFormats_("in", StringList::create("idXML"));
    registerOutputFile_("out", "<file>", "", "output file ");
    setValidFormats_("out", StringList::create("idXML"));
    registerFlag_("annotate_file_origin", "Store the original filename in each protein/peptide identification (MetaValue: file_origin).");
    registerFlag_("pepxml_protxml", "Merge idXML files derived from a pepXML and corresponding protXML file.\nExactly two input files are expected in this case.");

  }

  ExitCodes main_(int, const char **)
  {
    //-------------------------------------------------------------
    // parameter handling
    //-------------------------------------------------------------

    StringList file_names = getStringList_("in");
    String out = getStringOption_("out");

    if (file_names.size() < 1)
    {
      // this also allows exactly 1 file, because it might be usefull for
      // a TOPPAS pipeline containing an IDMerger, to run only with one file
      writeLog_("Less than one filename given. Aborting!");
      printUsage_();
      return ILLEGAL_PARAMETERS;
    }

    bool pepxml_protxml = getFlag_("pepxml_protxml");
    if (pepxml_protxml && (file_names.size() != 2))
    {
      writeLog_("Exactly two filenames expected for option 'pepxml_protxml'. Aborting!");
      printUsage_();
      return ILLEGAL_PARAMETERS;
    }

    //-------------------------------------------------------------
    // calculations
    //-------------------------------------------------------------

    vector<ProteinIdentification> proteins;
    vector<PeptideIdentification> peptides;

    if (pepxml_protxml)
    {
      mergePepXMLProtXML_(file_names, proteins, peptides);
    }
    else
    {
      bool annotate_file_origin =  getFlag_("annotate_file_origin");
      set<String> used_ids;
      for (StringList::Iterator file_it = file_names.begin();
           file_it != file_names.end(); ++file_it)
      {
        vector<ProteinIdentification> additional_proteins;
        vector<PeptideIdentification> additional_peptides;
        IdXMLFile().load(*file_it, additional_proteins, additional_peptides);

        // set MetaValue file_origin if flag is set
        if (annotate_file_origin)
        {
          for (vector<ProteinIdentification>::iterator prot_it =
                 additional_proteins.begin(); prot_it !=
               additional_proteins.end(); ++prot_it)
          {
            prot_it->setMetaValue("file_origin", DataValue(*file_it));
          }

          for (vector<PeptideIdentification>::iterator pep_it =
                 additional_peptides.begin(); pep_it !=
               additional_peptides.end(); ++pep_it)
          {
            pep_it->setMetaValue("file_origin", DataValue(*file_it));
          }
        }

        for (vector<ProteinIdentification>::iterator prot_it =
               additional_proteins.begin(); prot_it !=
             additional_proteins.end(); ++prot_it)
        {
          String id = prot_it->getIdentifier();
          if (used_ids.find(id) != used_ids.end())           // ID used previously
          {
            writeLog_("Warning: The identifier '" + id + "' was used before!");
            // generate a new ID:
            DateTime date_time = prot_it->getDateTime();
            String new_id;
            generateNewId_(used_ids, prot_it->getSearchEngine(), date_time,
                           new_id);
            writeLog_("New identifier '" + new_id +
                      "' generated as replacement.");
            // update fields:
            prot_it->setIdentifier(new_id);
            prot_it->setDateTime(date_time);
            for (vector<PeptideIdentification>::iterator pep_it =
                   additional_peptides.begin(); pep_it !=
                 additional_peptides.end(); ++pep_it)
            {
              if (pep_it->getIdentifier() == id) pep_it->setIdentifier(new_id);
            }
            used_ids.insert(new_id);
          }
          else used_ids.insert(id);
        }

        proteins.insert(proteins.end(), additional_proteins.begin(),
                        additional_proteins.end());
        peptides.insert(peptides.end(), additional_peptides.begin(),
                        additional_peptides.end());
      }
    }

    //-------------------------------------------------------------
    // writing output
    //-------------------------------------------------------------

    IdXMLFile().store(out, proteins, peptides);

    return EXECUTION_OK;
  }

};


int main(int argc, const char ** argv)
{
  TOPPIDMerger tool;
  return tool.main(argc, argv);
}

/// @endcond
