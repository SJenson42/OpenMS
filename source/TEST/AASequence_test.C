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
//

#include <OpenMS/CONCEPT/ClassTest.h>

///////////////////////////

#include <OpenMS/CHEMISTRY/AASequence.h>
#include <OpenMS/CHEMISTRY/ResidueDB.h>
#include <iostream>

using namespace OpenMS;
using namespace std;

///////////////////////////

START_TEST(AASequence, "$Id$")

/////////////////////////////////////////////////////////////

AASequence* ptr = 0;
AASequence* nullPointer = 0;
START_SECTION(AASequence())
	ptr = new AASequence();
	TEST_NOT_EQUAL(ptr, nullPointer)
END_SECTION

START_SECTION(~AASequence())
	delete ptr;
END_SECTION

START_SECTION(AASequence(const AASequence& rhs))
	AASequence seq;
	seq = String("AAA");
	AASequence seq2(seq);
	TEST_EQUAL(seq, seq2)
END_SECTION

START_SECTION(AASequence(const String& rhs))
  AASequence seq("CNARCKNCNCNARCDRE");
  TEST_EQUAL(seq.isModified(), false)
  TEST_EQUAL(seq.isValid(), true)
  TEST_EQUAL(seq.hasNTerminalModification(),false);
  TEST_EQUAL(seq.hasCTerminalModification(),false);
  TEST_EQUAL(seq.getResidue((SignedSize)4).getModification(),"")

  AASequence seq2;
  seq2 = String("CNARCKNCNCNARCDRE");
	TEST_EQUAL(seq, seq2);

  // test complex term-mods
  AASequence seq3("VPQVSTPTLVEVSRSLGK(Label:18O(2))");
  TEST_EQUAL(seq3.isModified(), true)
  TEST_EQUAL(seq3.isValid(), true)
  TEST_EQUAL(seq3.hasNTerminalModification(),false);
  TEST_EQUAL(seq3.hasCTerminalModification(),true);
  TEST_EQUAL(seq3.getResidue((SignedSize)4).getModification(),"")
  TEST_EQUAL(seq3.getCTerminalModification(), "Label:18O(2)")
  AASequence seq4;
  seq4 = "VPQVSTPTLVEVSRSLGK(Label:18O(2))";
  TEST_EQUAL(seq3,seq4)

  AASequence seq5("(ICPL:2H(4))CNARCNCNCN");
  TEST_EQUAL(seq5.isValid(), true)
  TEST_EQUAL(seq5.hasNTerminalModification(),true);
  TEST_EQUAL(seq5.isModified(),true);
  TEST_EQUAL(seq5.getNTerminalModification(),"ICPL:2H(4)")

  AASequence seq6("CNARCK(Label:13C(6)15N(2))NCNCN");
  TEST_EQUAL(seq6.isValid(), true)
  TEST_EQUAL(seq6.hasNTerminalModification(),false);
  TEST_EQUAL(seq6.hasCTerminalModification(),false);
  TEST_EQUAL(seq6.isModified(),true);
  TEST_EQUAL(seq6.getResidue((SignedSize)5).getModification(),"Label:13C(6)15N(2)")
  TEST_EQUAL(seq6.getResidue((SignedSize)4).getModification(),"")

  AASequence seq7("CNARCKNCNCNARCDRE(Amidated)");
  TEST_EQUAL(seq7.isValid(), true)
  TEST_EQUAL(seq7.hasNTerminalModification(),false);
  TEST_EQUAL(seq7.hasCTerminalModification(),true);
  TEST_EQUAL(seq7.isModified(),true);
  TEST_EQUAL(seq7.getCTerminalModification(),"Amidated")
END_SECTION

START_SECTION(AASequence& operator = (const AASequence& rhs))
	AASequence seq("AAA");
	AASequence seq2;
	seq2 = String("AAA");
	TEST_EQUAL(seq, seq2)
END_SECTION

START_SECTION(([EXTRA]Test modifications with brackets))
	AASequence seq1("ANLVFK(Label:13C(6)15N(2))EIEK(Label:2H(4))");
	TEST_EQUAL(seq1.isValid(), true)
	TEST_EQUAL(seq1.hasNTerminalModification(), false)
	TEST_EQUAL(seq1.hasCTerminalModification(), false)
	TEST_EQUAL(seq1.isModified(), true)
	AASequence seq2("ANLVFK(Label:13C(6)15N(2))EIEK(Label:2H(4))(Amidated)");
	TEST_EQUAL(seq2.isValid(), true)
	TEST_EQUAL(seq2.hasNTerminalModification(), false)
	TEST_EQUAL(seq2.hasCTerminalModification(), true)
	TEST_EQUAL(seq2.isModified(), true)  
END_SECTION

START_SECTION(bool operator == (const char* rhs) const)
  AASequence seq1("(Acetyl)DFPIANGER");
  AASequence seq2("DFPIANGER");
  TEST_EQUAL(seq2 == "DFPIANGER", true)
  TEST_EQUAL(seq1 == "(Acetyl)DFPIANGER", true)

  AASequence seq3("DFPIANGER(ADP-Ribosyl)");
  AASequence seq4("DFPIANGER(Amidated)");
  TEST_EQUAL(seq3 == "DFPIANGER", false)
  TEST_EQUAL(seq3 == "DFPIANGER(ADP-Ribosyl)", true)
  TEST_EQUAL(seq4 == "DFPIANGER(Amidated)", true)
  TEST_EQUAL(seq4 == "DFPIANGER", false)

  AASequence seq5("DFBIANGER");
  TEST_EQUAL(seq5 == "DFPIANGER", false)
  TEST_EQUAL(seq5 == "DFBIANGER", true)	
END_SECTION

START_SECTION(bool operator == (const String& rhs) const)
  AASequence seq1("(Acetyl)DFPIANGER");
  AASequence seq2("DFPIANGER");
  TEST_EQUAL(seq2 == String("DFPIANGER"), true)
  TEST_EQUAL(seq1 == String("(Acetyl)DFPIANGER"), true)

  AASequence seq3("DFPIANGER(ADP-Ribosyl)");
  AASequence seq4("DFPIANGER(Amidated)");
  TEST_EQUAL(seq3 == String("DFPIANGER"), false)
  TEST_EQUAL(seq3 == String("DFPIANGER(ADP-Ribosyl)"), true)
  TEST_EQUAL(seq4 == String("DFPIANGER(Amidated)"), true)
  TEST_EQUAL(seq4 == String("DFPIANGER"), false)

  AASequence seq5("DFBIANGER");
  TEST_EQUAL(seq5 == String("DFPIANGER"), false)
  TEST_EQUAL(seq5 == String("DFBIANGER"), true)
END_SECTION

START_SECTION(bool operator == (const AASequence& rhs) const)
  AASequence seq1("(Acetyl)DFPIANGER");
  AASequence seq2("DFPIANGER");
  TEST_EQUAL(seq2 == AASequence("DFPIANGER"), true)
  TEST_EQUAL(seq1 == AASequence("(Acetyl)DFPIANGER"), true)

  AASequence seq3("DFPIANGER(ADP-Ribosyl)");
  AASequence seq4("DFPIANGER(Amidated)");
  TEST_EQUAL(seq3 == AASequence("DFPIANGER"), false)
  TEST_EQUAL(seq3 == AASequence("DFPIANGER(ADP-Ribosyl)"), true)
  TEST_EQUAL(seq4 == AASequence("DFPIANGER(Amidated)"), true)
  TEST_EQUAL(seq4 == AASequence("DFPIANGER"), false)

  AASequence seq5("DFBIANGER");
  TEST_EQUAL(seq5 == AASequence("DFPIANGER"), false)
  TEST_EQUAL(seq5 == AASequence("DFBIANGER"), true)
END_SECTION

START_SECTION((const Residue& getResidue(SignedSize index) const))
	AASequence seq(String("ACDEF"));
	SignedSize sint(2);
  TEST_EQUAL(seq.getResidue(sint).getOneLetterCode(), "D")
	TEST_EXCEPTION(Exception::IndexUnderflow, seq.getResidue((SignedSize)-3))
	TEST_EXCEPTION(Exception::IndexOverflow, seq.getResidue((SignedSize)1000))
END_SECTION

START_SECTION(const Residue& getResidue(Size index) const)
	AASequence seq("ACDEF");
	Size unsignedint(2);
	TEST_EQUAL(seq.getResidue(unsignedint).getOneLetterCode(), "D")
	TEST_EXCEPTION(Exception::IndexOverflow, seq.getResidue((Size)1000))
END_SECTION

START_SECTION((EmpiricalFormula getFormula(Residue::ResidueType type = Residue::Full, Int charge=0) const))
	AASequence seq("ACDEF");
	TEST_EQUAL(seq.getFormula(), EmpiricalFormula("O10SH33N5C24"))
	TEST_EQUAL(seq.getFormula(Residue::Full, 1), EmpiricalFormula("O10SH33N5C24+"))
	TEST_EQUAL(seq.getFormula(Residue::BIon, 0), EmpiricalFormula("O9SH31N5C24"))
END_SECTION

START_SECTION((DoubleReal getAverageWeight(Residue::ResidueType type = Residue::Full, Int charge=0) const))
	AASequence seq("DFPIANGER");
	TOLERANCE_ABSOLUTE(0.01)
	TEST_REAL_SIMILAR(seq.getAverageWeight(), double(1018.08088))
	TEST_REAL_SIMILAR(seq.getAverageWeight(Residue::YIon, 1), double(1019.09))
END_SECTION

START_SECTION((DoubleReal getMonoWeight(Residue::ResidueType type = Residue::Full, Int charge=0) const))
  AASequence seq("DFPIANGER");
	TOLERANCE_ABSOLUTE(0.01)
	TEST_REAL_SIMILAR(seq.getMonoWeight(), double(1017.48796))
	TEST_REAL_SIMILAR(seq.getMonoWeight(Residue::YIon, 1), double(1018.5))

	// test N-term modification
	AASequence seq2("(NIC)DFPIANGER");
	TEST_REAL_SIMILAR(seq2.getMonoWeight(), double(1122.51));

	// test old OpenMS NIC definition
	AASequence seq2a("(MOD:09998)DFPIANGER");
	TEST_EQUAL(seq2 == seq2a, true)

	// test heavy modification
	AASequence seq3("(dNIC)DFPIANGER");
	TEST_REAL_SIMILAR(seq3.getMonoWeight(), double(1126.536019));

	// test old OpenMS dNIC definition
	AASequence seq3a("(MOD:09999)DFPIANGER");
	TEST_EQUAL(seq3 == seq3a, true)
	

END_SECTION

START_SECTION(const Residue& operator [] (SignedSize index) const)
  AASequence seq("DFPIANGER");
	SignedSize index = 0;
	TEST_EQUAL(seq[index].getOneLetterCode(), "D")
	index = -1;
	TEST_EXCEPTION(Exception::IndexUnderflow, seq[index])
	index = 20;
	TEST_EXCEPTION(Exception::IndexOverflow, seq[index])
END_SECTION

START_SECTION(const Residue& operator [] (Size index) const)
	AASequence seq("DFPIANGER");
  Size index = 0;
  TEST_EQUAL(seq[index].getOneLetterCode(), "D")
  index = 20;
  TEST_EXCEPTION(Exception::IndexOverflow, seq[index])
END_SECTION

START_SECTION(AASequence operator + (const AASequence& peptide) const)
  AASequence seq1("DFPIANGER"), seq2("DFP"), seq3("IANGER");
	TEST_EQUAL(seq1, seq2 + seq3);
END_SECTION

START_SECTION(AASequence operator + (const String& peptide) const)
  AASequence seq1("DFPIANGER"), seq2("DFP"); 
	String seq3("IANGER"), seq4("BLUBB");
	TEST_EQUAL(seq1, seq2 + seq3)
END_SECTION

START_SECTION(AASequence operator + (const Residue* residue) const)
  AASequence seq1("DFPIANGER");
	AASequence seq2("DFPIANGE");
  TEST_EQUAL(seq1, seq2 + ResidueDB::getInstance()->getResidue("R"))
END_SECTION

START_SECTION(AASequence& operator += (const AASequence&))
  AASequence seq1("DFPIANGER"), seq2("DFP"), seq3("IANGER");
	seq2 += seq3;
	TEST_EQUAL(seq1, seq2)
END_SECTION

START_SECTION(AASequence& operator += (const String&))
  AASequence seq1("DFPIANGER"), seq2("DFP");
	String seq3("IANGER"), seq4("BLUBB");
	seq2 += seq3;
	TEST_EQUAL(seq1, seq2)
END_SECTION

START_SECTION(AASequence& operator += (const Residue* residue))
  AASequence seq1("DFPIANGER");
  AASequence seq2("DFPIANGE");
  seq2 += ResidueDB::getInstance()->getResidue("R");
  TEST_EQUAL(seq1, seq2)
END_SECTION

START_SECTION(Size size() const)
  AASequence seq1("DFPIANGER");
	TEST_EQUAL(seq1.size(), 9)
END_SECTION

START_SECTION(AASequence getPrefix(Size index) const)
  AASequence seq1("DFPIANGER"), seq2("DFP"), seq3("DFPIANGER"), seq4("(TMT6plex)DFPIANGER"), seq5("DFPIANGER(Label:18O(2))"), seq6("DFPIANGERR(Label:18O(2))");
	TEST_EQUAL(seq2, seq1.getPrefix(3));
	TEST_EQUAL(seq3, seq1.getPrefix(9));
  TEST_NOT_EQUAL(seq4.getPrefix(3), seq1.getPrefix(3))
  TEST_NOT_EQUAL(seq5.getPrefix(9), seq1.getPrefix(9))
  TEST_EQUAL(seq6.getPrefix(9), seq1.getPrefix(9))
	TEST_EXCEPTION(Exception::IndexOverflow, seq1.getPrefix(10))
END_SECTION

START_SECTION(AASequence getSuffix(Size index) const)
  AASequence seq1("DFPIANGER"), seq2("GER"), seq3("DFPIANGER"), seq4("DFPIANGER(Label:18O(2))"), seq5("(TMT6plex)DFPIANGER"), seq6("(TMT6plex)DDFPIANGER");
	TEST_EQUAL(seq2, seq1.getSuffix(3));
	TEST_EQUAL(seq3, seq1.getSuffix(9));
  TEST_NOT_EQUAL(seq4.getSuffix(3), seq1.getSuffix(3))
  TEST_NOT_EQUAL(seq5.getSuffix(9), seq1.getSuffix(9))
  TEST_EQUAL(seq6.getSuffix(9), seq1.getSuffix(9))
	TEST_EXCEPTION(Exception::IndexOverflow, seq1.getSuffix(10))
END_SECTION

START_SECTION(AASequence getSubsequence(Size index, UInt number) const)
  AASequence seq1("DFPIANGER"), seq2("IAN"), seq3("DFPIANGER");
	TEST_EQUAL(seq2, seq1.getSubsequence(3, 3))
	TEST_EQUAL(seq3, seq1.getSubsequence(0, 9))
	TEST_EXCEPTION(Exception::IndexOverflow, seq1.getSubsequence(0, 10))
END_SECTION

START_SECTION(bool has(const Residue& residue) const)
  AASequence seq("DFPIANGER");
	TEST_EQUAL(seq.has(seq[(Size)0]), true)
	Residue res;
	TEST_NOT_EQUAL(seq.has(res), true)
END_SECTION

START_SECTION(bool has(const String& name) const)
  AASequence seq("DFPIANGER");
	TEST_EQUAL(seq.has("D"), true)
	TEST_EQUAL(seq.has("N"), true)
	TEST_NOT_EQUAL(seq.has("Q"), true)
END_SECTION

START_SECTION(bool hasSubsequence(const AASequence& peptide) const)
  AASequence seq1("DFPIANGER"), seq2("IANG"), seq3("AIN");
	TEST_EQUAL(seq1.hasSubsequence(seq2), true)
	TEST_EQUAL(seq1.hasSubsequence(seq3), false)
END_SECTION

START_SECTION(bool hasSubsequence(const String& peptide) const)
  AASequence seq1("DFPIANGER");
	String seq2("IANG"), seq3("AIN");
	TEST_EQUAL(seq1.hasSubsequence(seq2), true)
	TEST_EQUAL(seq1.hasSubsequence(seq3), false)
END_SECTION

START_SECTION(bool hasPrefix(const AASequence& peptide) const)
  AASequence seq1("DFPIANGER"), seq2("DFP"), seq3("AIN"), seq4("(TMT6plex)DFP"), seq5("DFPIANGER(Label:18O(2))"), seq6("DFP(Label:18O(2))");
	TEST_EQUAL(seq1.hasPrefix(seq2), true)
	TEST_EQUAL(seq1.hasPrefix(seq3), false)
  TEST_EQUAL(seq1.hasPrefix(seq4), false)
  TEST_EQUAL(seq1.hasPrefix(seq5), false)
  TEST_EQUAL(seq1.hasPrefix(seq6), true)
END_SECTION

START_SECTION(bool hasPrefix(const String& peptide) const)
  AASequence seq1("DFPIANGER");
  String seq2("DFP"), seq3("AIN"), seq4("(TMT6plex)DFP"), seq5("DFPIANGER(Label:18O(2))"), seq6("DFP(Label:18O(2))");;
	TEST_EQUAL(seq1.hasPrefix(seq2), true)
	TEST_EQUAL(seq1.hasPrefix(seq3), false)
  TEST_EQUAL(seq1.hasPrefix(seq4), false)
  TEST_EQUAL(seq1.hasPrefix(seq5), false)
  TEST_EQUAL(seq1.hasPrefix(seq6), true)
END_SECTION

START_SECTION(bool hasSuffix(const AASequence& peptide) const)
  AASequence seq1("DFPIANGER"), seq2("GER"), seq3("AIN"), seq4("GER(Label:18O(2))"), seq5("(TMT6plex)DFPIANGER"), seq6("(TMT6plex)GER");
  TEST_EQUAL(seq1.hasSuffix(seq2), true)
  TEST_EQUAL(seq1.hasSuffix(seq3), false) 
  TEST_EQUAL(seq1.hasSuffix(seq4), false)
  TEST_EQUAL(seq1.hasSuffix(seq5), false)
  TEST_EQUAL(seq1.hasSuffix(seq6), true)
END_SECTION

START_SECTION(bool hasSuffix(const String& peptide) const)
  AASequence seq1("DFPIANGER");
  String seq2("GER"), seq3("AIN"), seq4("GER(Label:18O(2))"), seq5("(TMT6plex)DFPIANGER"), seq6("(TMT6plex)GER");
  TEST_EQUAL(seq1.hasSuffix(seq2), true)
  TEST_EQUAL(seq1.hasSuffix(seq3), false)
  TEST_EQUAL(seq1.hasSuffix(seq4), false)
  TEST_EQUAL(seq1.hasSuffix(seq5), false)
  TEST_EQUAL(seq1.hasSuffix(seq6), true)
END_SECTION

START_SECTION(ConstIterator begin() const)
	String result[] = { "D", "F", "P", "I", "A", "N", "G", "E", "R" };
	AASequence seq("DFPIANGER");
	Size i = 0;
	for (AASequence::ConstIterator it = seq.begin(); it != seq.end(); ++it, ++i)
	{
		TEST_EQUAL((*it).getOneLetterCode(), result[i])
	}
END_SECTION

START_SECTION(ConstIterator end() const)
	NOT_TESTABLE
END_SECTION

START_SECTION(Iterator begin())
	String result[] = { "D", "F", "P", "I", "A", "N", "G", "E", "R" };
  AASequence seq("DFPIANGER");
  Size i = 0;
  for (AASequence::ConstIterator it = seq.begin(); it != seq.end(); ++it, ++i)
  {
    TEST_EQUAL((*it).getOneLetterCode(), result[i])
  }
END_SECTION
		  
START_SECTION(Iterator end())
	NOT_TESTABLE
END_SECTION

//START_SECTION(friend std::ostream& operator << (std::ostream& os, const AASequence& peptide))
//  // TODO
//END_SECTION

//START_SECTION(friend std::istream& operator >> (std::istream& is, const AASequence& peptide))
//  // TODO
//END_SECTION

START_SECTION(String toString() const)
	AASequence seq1("DFPIANGER");
	AASequence seq2("(MOD:00051)DFPIANGER");
	AASequence seq3("DFPIAN(Deamidated)GER");

	TEST_EQUAL(seq1.isValid(), true)
	TEST_EQUAL(seq2.isValid(), true)
	TEST_EQUAL(seq3.isValid(), true)

	TEST_STRING_EQUAL(seq1.toString(), "DFPIANGER")
	TEST_STRING_EQUAL(seq2.toString(), "(MOD:00051)DFPIANGER")
	TEST_STRING_EQUAL(seq3.toString(), "DFPIAN(Deamidated)GER")
END_SECTION

START_SECTION(String toUnmodifiedString() const)
	AASequence seq1("DFPIANGER");
  AASequence seq2("(MOD:00051)DFPIANGER");
  AASequence seq3("DFPIAN(Deamidated)GER");

  TEST_EQUAL(seq1.isValid(), true)
  TEST_EQUAL(seq2.isValid(), true)
  TEST_EQUAL(seq3.isValid(), true)

  TEST_STRING_EQUAL(seq1.toUnmodifiedString(), "DFPIANGER")
  TEST_STRING_EQUAL(seq2.toUnmodifiedString(), "DFPIANGER")
  TEST_STRING_EQUAL(seq3.toUnmodifiedString(), "DFPIANGER")
END_SECTION

START_SECTION(AASequence(const char *rhs))
	AASequence seq1('C');
	AASequence seq2('A');
	TEST_STRING_EQUAL(seq1.toString(), "C")
	TEST_STRING_EQUAL(seq2.toUnmodifiedString(), "A")
	AASequence seq3("CA");
	TEST_STRING_EQUAL((seq1 + seq2).toString(), seq3.toString())
END_SECTION

START_SECTION(void setModification(Size index, const String &modification))
	AASequence seq1("ACDEFNK");
	seq1.setModification(5, "Deamidated");
	TEST_STRING_EQUAL(seq1[(Size)5].getModification(), "Deamidated")
END_SECTION

START_SECTION(void setNTerminalModification(const String &modification))
	AASequence seq1("DFPIANGER");
	AASequence seq2("(MOD:00051)DFPIANGER");
	TEST_EQUAL(seq1 == seq2, false)
	seq1.setNTerminalModification("MOD:00051");
	TEST_EQUAL(seq1 == seq2, true)
	
	AASequence seq3("DABCDEF");
	AASequence seq4("(MOD:00051)DABCDEF");
	TEST_EQUAL(seq3 == seq4, false)
	TEST_EQUAL(seq3.isValid(), seq4.isValid())
	seq3.setNTerminalModification("MOD:00051");
	TEST_EQUAL(seq3.isModified(), true)
	TEST_EQUAL(seq4.isModified(), true)
	TEST_EQUAL(seq3 == seq4, true)
END_SECTION


START_SECTION(const String& getNTerminalModification() const)
	AASequence seq1("(MOD:00051)DFPIANGER");
	TEST_EQUAL(seq1.getNTerminalModification(), "MOD:00051")

	AASequence seq2("DFPIANGER");
	TEST_EQUAL(seq2.getNTerminalModification(), "")

END_SECTION


START_SECTION(void setCTerminalModification(const String &modification))
	AASequence seq1("DFPIANGER");
	AASequence seq2("DFPIANGER(Amidated)");

	TEST_EQUAL(seq1 == seq2, false)
	seq1.setCTerminalModification("Amidated");
	TEST_EQUAL(seq1 == seq2, true)

	AASequence seq3("DABCDER");
	AASequence seq4("DABCDER(Amidated)");
	TEST_EQUAL(seq3 == seq4, false)
	TEST_EQUAL(seq3.isValid(), seq4.isValid())
	seq3.setCTerminalModification("Amidated");
	TEST_EQUAL(seq3.isModified(), true)
	TEST_EQUAL(seq4.isModified(), true)
	TEST_EQUAL(seq3 == seq4, true)

	AASequence seq5("DABCDER(MOD:00177)");
	AASequence seq6("DABCDER(MOD:00177)(Amidated)");
	TEST_EQUAL(seq5.isModified(), true)
	TEST_EQUAL(seq6.isModified(), true)
	seq5.setCTerminalModification("Amidated");	
	TEST_EQUAL(seq5 == seq6, true)

	AASequence seq7("DFPIANGER(MOD:00177)");
	AASequence seq8("DFPIANGER(MOD:00177)(Amidated)");
	TEST_EQUAL(seq7.isModified(), true)
	TEST_EQUAL(seq8.isModified(), true)
	seq7.setCTerminalModification("Amidated");
	TEST_EQUAL(seq5 == seq6, true)
END_SECTION

START_SECTION(const String& getCTerminalModification() const)
  AASequence seq1("DFPIANGER(Amidated)");
  TEST_EQUAL(seq1.getCTerminalModification(), "Amidated")

  AASequence seq2("DFPIANGER");
  TEST_EQUAL(seq2.getCTerminalModification(), "")	
END_SECTION

START_SECTION(bool setStringSequence(const String &sequence))
	AASequence seq1("DFPIANGER");
	AASequence seq2("(MOD:00051)DFPIAK");

	AASequence seq3 = seq1;

	TEST_EQUAL(seq1 == seq3, true)
	
	seq3.setStringSequence("(MOD:00051)DFPIAK");
	TEST_EQUAL(seq2 == seq3, true)

	seq3.setStringSequence("DFPIANGER");
	TEST_EQUAL(seq1 == seq3, true)	
END_SECTION

START_SECTION(AASequence operator + (const char *rhs) const)
  AASequence seq1("DFPIANGER"), seq2("DFP");
	TEST_EQUAL(seq1, seq2 + "IANGER");
END_SECTION

START_SECTION(AASequence& operator += (const char *rhs))
	AASequence seq1("DFPIANGER"), seq2("DFP");
	seq2 += "IANGER";
	TEST_EQUAL(seq1, seq2)
END_SECTION


START_SECTION(bool isValid() const)
	AASequence seq1("(MOD:00051)DABCDEF");
	AASequence seq2("DABCDEF");
	AASequence seq3("(MOD:00051)DFPIANGER");
	AASequence seq4("DFPIANGER");

	TEST_EQUAL(seq1.isValid(), true)
	TEST_EQUAL(seq2.isValid(), true)
	TEST_EQUAL(seq3.isValid(), true)
	TEST_EQUAL(seq4.isValid(), true)


	AASequence seq5("blDABCDEF");
	AASequence seq6("a");
	TEST_EQUAL(seq5.isValid(), false)
	TEST_EQUAL(seq6.isValid(), false)
END_SECTION

START_SECTION(bool hasNTerminalModification() const)
	AASequence seq1("(MOD:00051)DABCDEF");
	AASequence seq2("DABCDEF");

	TEST_EQUAL(seq1.hasNTerminalModification(), true)
	TEST_EQUAL(seq2.hasNTerminalModification(), false)
	
	AASequence seq3("(MOD:00051)DFPIANGER");
	AASequence seq4("DFPIANGER");
	TEST_EQUAL(seq3.hasNTerminalModification(), true)
	TEST_EQUAL(seq4.hasNTerminalModification(), false)
END_SECTION
 
START_SECTION(bool hasCTerminalModification() const)
	AASequence seq1("DFPIANGER(Amidated)");
	AASequence seq2("DFPIANGER");
	TEST_EQUAL(seq1.hasCTerminalModification(), true)
	TEST_EQUAL(seq2.hasCTerminalModification(), false)
	seq1.setCTerminalModification("");
	TEST_EQUAL(seq1.hasCTerminalModification(), false)
END_SECTION

START_SECTION(bool isModified() const)
	AASequence seq1("DFPIANGER");
	TEST_EQUAL(seq1.isModified(), false);
	AASequence seq2(seq1);
	seq2.setNTerminalModification("MOD:09999");
	TEST_EQUAL(seq2.isModified(), true)

	AASequence seq3(seq1);
	seq3.setCTerminalModification("Amidated");
	TEST_EQUAL(seq3.isModified(), true);

	AASequence seq4("DFPIANGER(MOD:00177)");
	TEST_EQUAL(seq4.isModified(), true);
END_SECTION

START_SECTION(bool isModified(Size index) const)
	AASequence seq4("DFPIAN(MOD:00565)GER");
	TEST_EQUAL(seq4.isModified(5), true)
	TEST_EQUAL(seq4.isModified(4), false)
END_SECTION

START_SECTION(bool operator<(const AASequence &rhs) const)
	AASequence seq1("DFPIANGER");
	AASequence seq2("DFBIANGER");
	TEST_EQUAL(seq2 < seq1, true)
	TEST_EQUAL(seq1 < seq2, false)
	AASequence seq3("DFPIANGFR");
	TEST_EQUAL(seq3 < seq1, false)
END_SECTION
 
START_SECTION(bool operator!=(const AASequence& rhs) const)
  AASequence seq1("(MOD:00051)DFPIANGER");
  AASequence seq2("DFPIANGER");
  TEST_EQUAL(seq2 != AASequence("DFPIANGER"), false)
  TEST_EQUAL(seq1 != AASequence("(MOD:00051)DFPIANGER"), false)

  AASequence seq3("DFPIANGER(MOD:00177)");
  AASequence seq4("DFPIANGER(Amidated)");
  TEST_EQUAL(seq3 != AASequence("DFPIANGER"), true)
  TEST_EQUAL(seq3 != AASequence("DFPIANGER(MOD:00177)"), false)
  TEST_EQUAL(seq4 != AASequence("DFPIANGER(Amidated)"), false)
  TEST_EQUAL(seq4 != AASequence("DFPIANGER"), true)

  AASequence seq5("DFBIANGER");
  TEST_EQUAL(seq5 != AASequence("DFPIANGER"), true)
  TEST_EQUAL(seq5 != AASequence("DFBIANGER"), false)
END_SECTION

START_SECTION(bool operator!=(const String& rhs) const)
  AASequence seq1("(MOD:00051)DFPIANGER");
  AASequence seq2("DFPIANGER");
  TEST_EQUAL(seq2 != String("DFPIANGER"), false)
  TEST_EQUAL(seq1 != String("(MOD:00051)DFPIANGER"), false)

  AASequence seq3("DFPIANGER(MOD:00177)");
  AASequence seq4("DFPIANGER(Amidated)");
  TEST_EQUAL(seq3 != String("DFPIANGER"), true)
  TEST_EQUAL(seq3 != String("DFPIANGER(MOD:00177)"), false)
  TEST_EQUAL(seq4 != String("DFPIANGER(Amidated)"), false)
  TEST_EQUAL(seq4 != String("DFPIANGER"), true)

	AASequence seq5("DFBIANGER");
	TEST_EQUAL(seq5 != String("DFPIANGER"), true)
	TEST_EQUAL(seq5 != String("DFBIANGER"), false)
END_SECTION

START_SECTION(bool operator!=(const char *rhs) const)
	AASequence seq1("(MOD:00051)DFPIANGER");
	AASequence seq2("DFPIANGER");
	TEST_EQUAL(seq2 != "DFPIANGER", false)
	TEST_EQUAL(seq1 != "(MOD:00051)DFPIANGER", false)

	AASequence seq3("DFPIANGER(MOD:00177)");
	AASequence seq4("DFPIANGER(Amidated)");
	TEST_EQUAL(seq3 != "DFPIANGER", true)
	TEST_EQUAL(seq3 != "DFPIANGER(MOD:00177)", false)
	TEST_EQUAL(seq4 != "DFPIANGER(Amidated)", false)
	TEST_EQUAL(seq4 != "DFPIANGER", true)

	AASequence seq5("DFBIANGER");
	TEST_EQUAL(seq5 != "DFPIANGER", true)
	TEST_EQUAL(seq5 != "DFBIANGER", false)
END_SECTION

START_SECTION(Size getNumberOf(const String &residue) const)
	AASequence seq("DFPIANGERDFPIANGER");
	TEST_EQUAL(seq.getNumberOf("Ala"), 2)
	TEST_EQUAL(seq.getNumberOf("D"), 2)
END_SECTION

START_SECTION(void getAAFrequencies(Map<String, Size>& frequency_table) const)
	AASequence a("THREEAAAWITHYYY");

	Map<String, Size> table;
	a.getAAFrequencies(table);

	TEST_EQUAL(table["T"]==2, true);
	TEST_EQUAL(table["H"]==2, true);
	TEST_EQUAL(table["R"]==1, true);
	TEST_EQUAL(table["E"]==2, true);
	TEST_EQUAL(table["A"]==3, true);
	TEST_EQUAL(table["W"]==1, true);
	TEST_EQUAL(table["I"]==1, true);
	TEST_EQUAL(table["Y"]==3, true);

	TEST_EQUAL(table.size()==8, true);

END_SECTION


START_SECTION([EXTRA] Tag in peptides)
	String I_weight = String(ResidueDB::getInstance()->getResidue("I")->getMonoWeight(Residue::Internal));
  AASequence aa1("DFPIANGER");
  AASequence aa2("DPF[" + I_weight + "]ANGER");
  AASequence aa3("[" + I_weight + "]DFPANGER");
  AASequence aa4("DFPANGER[" + I_weight + "]");
	TEST_REAL_SIMILAR(aa1.getMonoWeight(), 1017.487958568)
	TEST_EQUAL(aa2.isModified(), false)
	TEST_EQUAL(aa3.hasNTerminalModification(), false)
	TEST_EQUAL(aa4.hasCTerminalModification(), false)
	TEST_REAL_SIMILAR(aa2.getMonoWeight(), 1017.487958568)
	TEST_REAL_SIMILAR(aa3.getMonoWeight(), 1017.487958568)
	TEST_REAL_SIMILAR(aa4.getMonoWeight(), 1017.487958568)

END_SECTION

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
END_TEST
