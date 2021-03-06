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
// $Maintainer: Stephan Aiche$
// $Authors: Marc Sturm $
// --------------------------------------------------------------------------

#include <OpenMS/CONCEPT/ClassTest.h>

///////////////////////////

#include <OpenMS/DATASTRUCTURES/StringList.h>
#include <QStringList>

using namespace OpenMS;
using namespace std;

///////////////////////////

START_TEST(StringList, "$Id$")

/////////////////////////////////////////////////////////////

StringList* ptr = 0;
StringList* nullPointer = 0;
START_SECTION((StringList()))
	ptr = new StringList;
	TEST_NOT_EQUAL(ptr, nullPointer)
END_SECTION

START_SECTION(~StringList())
	delete ptr;
END_SECTION

START_SECTION((static StringList create(const String& list, const char splitter=',')))
	StringList list = StringList::create("yes,no");
	TEST_EQUAL(list.size(),2);
	TEST_STRING_EQUAL(list[0],"yes");
	TEST_STRING_EQUAL(list[1],"no");

	StringList list2 = StringList::create("no");
	TEST_EQUAL(list2.size(),1);
	TEST_STRING_EQUAL(list2[0],"no");

	StringList list3 = StringList::create("");
	TEST_EQUAL(list3.size(),0);
END_SECTION

START_SECTION((static StringList create( const char * const * list, UInt size )))
{
	const char * const yes_no[] = { "yes", "no" };
	StringList list = StringList::create(yes_no,2);
	TEST_EQUAL(list.size(),2);
	TEST_STRING_EQUAL(list[0],"yes");
	TEST_STRING_EQUAL(list[1],"no");

	const char * const yes[] = { "yes" };
	StringList list2 = StringList::create(yes,1);
	TEST_EQUAL(list2.size(),1);
	TEST_STRING_EQUAL(list2[0],"yes");

	StringList list3 = StringList::create(0,0);
	TEST_EQUAL(list3.size(),0);
}
END_SECTION

START_SECTION((StringList(const QStringList &rhs)))
{
  QStringList q_str_list;
  q_str_list << "First Element" << "Second Element" << "Third Element";

  StringList str_list(q_str_list);
  TEST_EQUAL((int)str_list.size(), q_str_list.size())
  ABORT_IF((int)str_list.size() != q_str_list.size())
  for(Size i = 0 ; i < str_list.size() ; ++i)
  {
    TEST_EQUAL(str_list[i], String(q_str_list[(int)i]))
  }
}
END_SECTION

START_SECTION((StringList(const StringList& rhs)))
	StringList list = StringList::create("yes,no");
	StringList list2(list);
	TEST_EQUAL(list2.size(),2);
	TEST_STRING_EQUAL(list2[0],"yes");
	TEST_STRING_EQUAL(list2[1],"no");
END_SECTION

START_SECTION((StringList(const std::vector<String>& rhs)))
	std::vector<String> list;
	list.push_back("yes");
	list.push_back("no");
	StringList list2(list);
	TEST_EQUAL(list2.size(),2);
	TEST_STRING_EQUAL(list2[0],"yes");
	TEST_STRING_EQUAL(list2[1],"no");
END_SECTION

START_SECTION((StringList(const std::vector<std::string>& rhs)))
	std::vector<string> list;
	list.push_back("yes");
	list.push_back("no");
	StringList list2(list);
	TEST_EQUAL(list2.size(),2);
	TEST_STRING_EQUAL(list2[0],"yes");
	TEST_STRING_EQUAL(list2[1],"no");

END_SECTION

START_SECTION((StringList& operator=(const StringList& rhs)))
	StringList list = StringList::create("yes,no");
	StringList list2;
	list2 = list;
	TEST_EQUAL(list2.size(),2);
	TEST_STRING_EQUAL(list2[0],"yes");
	TEST_STRING_EQUAL(list2[1],"no");

END_SECTION

START_SECTION((StringList& operator=(const std::vector<String>& rhs)))
	std::vector<String> list;
	list.push_back("yes");
	list.push_back("no");
	StringList list2;
	list2 = list;
	TEST_EQUAL(list2.size(),2);
	TEST_STRING_EQUAL(list2[0],"yes");
	TEST_STRING_EQUAL(list2[1],"no");

END_SECTION

START_SECTION((StringList& operator=(const std::vector<std::string>& rhs)))
	std::vector<string> list;
	list.push_back("yes");
	list.push_back("no");
	StringList list2;
	list2 = list;
	TEST_EQUAL(list2.size(),2);
	TEST_STRING_EQUAL(list2[0],"yes");
	TEST_STRING_EQUAL(list2[1],"no");

END_SECTION

START_SECTION((template<typename StringType> StringList& operator<<(const StringType& string)))
	StringList list;
	list << "a" << "b" << "c" << "a";
	TEST_EQUAL(list.size(),4);
	TEST_STRING_EQUAL(list[0],"a");
	TEST_STRING_EQUAL(list[1],"b");
	TEST_STRING_EQUAL(list[2],"c");
	TEST_STRING_EQUAL(list[3],"a");
END_SECTION

START_SECTION((bool contains(const String& s) const))
	StringList list = StringList::create("yes,no");
	TEST_EQUAL(list.contains("yes"),true)
	TEST_EQUAL(list.contains("no"),true)
	TEST_EQUAL(list.contains("jup"),false)	
	TEST_EQUAL(list.contains(""),false)
	TEST_EQUAL(list.contains("noe"),false)
END_SECTION

START_SECTION((void toUpper()))
	StringList list = StringList::create("yes,no");
	list.toUpper();
	TEST_EQUAL(list[0],"YES")
	TEST_EQUAL(list[1],"NO")
END_SECTION

START_SECTION((void toLower()))
	StringList list = StringList::create("yES,nO");
	list.toLower();
	TEST_EQUAL(list[0],"yes")
	TEST_EQUAL(list[1],"no")
END_SECTION

START_SECTION((String concatenate(const String &glue="") const ))
	StringList list = StringList::create("1,2,3,4,5");
	TEST_STRING_EQUAL(list.concatenate("g"),"1g2g3g4g5");
	TEST_STRING_EQUAL(list.concatenate(""),"12345");
	
	list.clear();
	TEST_STRING_EQUAL(list.concatenate("g"),"");
	TEST_STRING_EQUAL(list.concatenate(""),"");
	
	//test2 (from StringList)
	StringList tmp;
	TEST_EQUAL(tmp.concatenate(),"")
	tmp.push_back("1\n");
	tmp.push_back("2\n");
	tmp.push_back("3\n");
	TEST_EQUAL(tmp.concatenate(),"1\n2\n3\n")
END_SECTION

StringList tmp_list;
tmp_list.push_back("first_line");
tmp_list.push_back("");
tmp_list.push_back("");
tmp_list.push_back("middle_line");
tmp_list.push_back("");
tmp_list.push_back("  space_line");
tmp_list.push_back("	tab_line");
tmp_list.push_back("back_space_line   ");
tmp_list.push_back("back_tab_line			");
tmp_list.push_back("");
tmp_list.push_back("last_line");

StringList tmp_list2;
tmp_list2.push_back("first_line");
tmp_list2.push_back("");
tmp_list2.push_back("");
tmp_list2.push_back("middle_line");
tmp_list2.push_back("");
tmp_list2.push_back("space_line");
tmp_list2.push_back("tab_line");
tmp_list2.push_back("back_space_line");
tmp_list2.push_back("back_tab_line");
tmp_list2.push_back("");
tmp_list2.push_back("last_line");

START_SECTION((Iterator search(const Iterator& start, const String& text, bool trim=false)))
	StringList list(tmp_list);
	
	TEST_EQUAL(list.search(list.begin(),"first_line") == list.begin(), true)
	TEST_EQUAL(list.search(list.begin(),"middle_line") == (list.begin()+3), true)
	TEST_EQUAL(list.search(list.begin(),"space_line") == list.end(), true)
	TEST_EQUAL(list.search(list.begin(),"tab_line") == list.end(), true)
	TEST_EQUAL(list.search(list.begin(),"last_line") == (list.end()-1), true)
	TEST_EQUAL(list.search(list.begin(),"invented_line") == list.end(), true)
	TEST_EQUAL(list.search(list.begin()+1,"first_line") == list.end(), true)
	TEST_EQUAL(list.search(list.begin()," ") == (list.begin()+5), true)
	TEST_EQUAL(list.search(list.begin(),"\t") == (list.begin()+6), true)
	TEST_EQUAL(list.search(list.begin()+9,"\t") == (list.end()), true)
	
	//trim
	TEST_EQUAL(list.search(list.begin(),"first_line",true) == list.begin(), true)
	TEST_EQUAL(list.search(list.begin(),"space_line",true) == (list.begin()+5), true)
	TEST_EQUAL(list.search(list.begin(),"tab_line",true) == (list.begin()+6), true)
	TEST_EQUAL(list.search(list.begin(),"invented_line",true) == list.end(), true)
	TEST_EQUAL(list.search(list.begin()+1,"first_line",true) == list.end(), true)
	
	//Try it on the same file (but trimmed)
	list = tmp_list2;

	TEST_EQUAL(list.search(list.begin(),"first_line") == list.begin(), true)
	TEST_EQUAL(list.search(list.begin(),"middle_line") == (list.begin()+3), true)
	TEST_EQUAL(list.search(list.begin(),"space_line",true) == (list.begin()+5), true)
	TEST_EQUAL(list.search(list.begin(),"tab_line",true) == (list.begin()+6), true)
	TEST_EQUAL(list.search(list.begin(),"last_line") == (list.end()-1), true)
	TEST_EQUAL(list.search(list.begin(),"invented_line") == list.end(), true)
	TEST_EQUAL(list.search(list.begin()+1,"first_line") == list.end(), true)

	//trim
	TEST_EQUAL(list.search(list.begin(),"first_line",true) == list.begin(), true)
	TEST_EQUAL(list.search(list.begin(),"space_line",true) == (list.begin()+5), true)
	TEST_EQUAL(list.search(list.begin(),"tab_line",true) == (list.begin()+6), true)
	TEST_EQUAL(list.search(list.begin(),"invented_line",true) == list.end(), true)
	TEST_EQUAL(list.search(list.begin()+1,"first_line",true) == list.end(), true)
END_SECTION

START_SECTION((Iterator search(const String& text, bool trim=false)))
	StringList list(tmp_list);
	
	TEST_EQUAL(list.search("first_line") == list.begin(), true)
	TEST_EQUAL(list.search("middle_line") == (list.begin()+3), true)
	TEST_EQUAL(list.search("space_line") == list.end(), true)
	TEST_EQUAL(list.search("tab_line") == list.end(), true)
	TEST_EQUAL(list.search("last_line") == (list.end()-1), true)
	TEST_EQUAL(list.search("invented_line") == list.end(), true)
	TEST_EQUAL(list.search(" ") == (list.begin()+5), true)
	TEST_EQUAL(list.search("\t") == (list.begin()+6), true)
	
	//trim
	TEST_EQUAL(list.search("first_line",true) == list.begin(), true)
	TEST_EQUAL(list.search("space_line",true) == (list.begin()+5), true)
	TEST_EQUAL(list.search("tab_line",true) == (list.begin()+6), true)
	TEST_EQUAL(list.search("invented_line",true) == list.end(), true)
	
	//Try it on the same file (but trimmed)
	list = tmp_list2;

	TEST_EQUAL(list.search("first_line") == list.begin(), true)
	TEST_EQUAL(list.search("middle_line") == (list.begin()+3), true)
	TEST_EQUAL(list.search("space_line",true) == (list.begin()+5), true)
	TEST_EQUAL(list.search("tab_line",true) == (list.begin()+6), true)
	TEST_EQUAL(list.search("last_line") == (list.end()-1), true)
	TEST_EQUAL(list.search("invented_line") == list.end(), true)

	//trim
	TEST_EQUAL(list.search("first_line",true) == list.begin(), true)
	TEST_EQUAL(list.search("space_line",true) == (list.begin()+5), true)
	TEST_EQUAL(list.search("tab_line",true) == (list.begin()+6), true)
	TEST_EQUAL(list.search("invented_line",true) == list.end(), true)
END_SECTION

START_SECTION((Iterator searchSuffix(const Iterator& start, const String& text, bool trim=false)))
	StringList list(tmp_list);
	
	TEST_EQUAL(list.searchSuffix(list.begin(),"invented_line",true) == list.end(), true)
	TEST_EQUAL(list.searchSuffix(list.begin(),"back_space_line",true) == list.begin()+7, true)
	TEST_EQUAL(list.searchSuffix(list.begin(),"back_tab_line",true) == list.begin()+8, true)
	TEST_EQUAL(list.searchSuffix(list.begin()+8,"back_space_line",true) == list.end(), true)
	
	TEST_EQUAL(list.searchSuffix(list.begin(),"invented_line") == list.end(), true)
	TEST_EQUAL(list.searchSuffix(list.begin(),"back_space_line") == list.end(), true)
	TEST_EQUAL(list.searchSuffix(list.begin(),"back_tab_line") == list.end(), true)
END_SECTION

START_SECTION((Iterator searchSuffix(const String& text, bool trim=false)))
	StringList list(tmp_list);
	
	TEST_EQUAL(list.searchSuffix("invented_line",true) == list.end(), true)
	TEST_EQUAL(list.searchSuffix("back_space_line",true) == list.begin()+7, true)
	TEST_EQUAL(list.searchSuffix("back_tab_line",true) == list.begin()+8, true)
	
	TEST_EQUAL(list.searchSuffix("invented_line") == list.end(), true)
	TEST_EQUAL(list.searchSuffix("back_space_line") == list.end(), true)
	TEST_EQUAL(list.searchSuffix("back_tab_line") == list.end(), true)
END_SECTION

START_SECTION((ConstIterator search(const ConstIterator& start, const String& text, bool trim=false) const))
	StringList list(tmp_list);
	
	TEST_EQUAL(list.search(list.begin(),"first_line") == list.begin(), true)
	TEST_EQUAL(list.search(list.begin(),"middle_line") == (list.begin()+3), true)
	TEST_EQUAL(list.search(list.begin(),"space_line") == list.end(), true)
	TEST_EQUAL(list.search(list.begin(),"tab_line") == list.end(), true)
	TEST_EQUAL(list.search(list.begin(),"last_line") == (list.end()-1), true)
	TEST_EQUAL(list.search(list.begin(),"invented_line") == list.end(), true)
	TEST_EQUAL(list.search(list.begin()+1,"first_line") == list.end(), true)
	TEST_EQUAL(list.search(list.begin()," ") == (list.begin()+5), true)
	TEST_EQUAL(list.search(list.begin(),"\t") == (list.begin()+6), true)
	TEST_EQUAL(list.search(list.begin()+9,"\t") == (list.end()), true)
	
	//trim
	TEST_EQUAL(list.search(list.begin(),"first_line",true) == list.begin(), true)
	TEST_EQUAL(list.search(list.begin(),"space_line",true) == (list.begin()+5), true)
	TEST_EQUAL(list.search(list.begin(),"tab_line",true) == (list.begin()+6), true)
	TEST_EQUAL(list.search(list.begin(),"invented_line",true) == list.end(), true)
	TEST_EQUAL(list.search(list.begin()+1,"first_line",true) == list.end(), true)
	
	//Try it on the same file (but trimmed)
	list = tmp_list2;

	TEST_EQUAL(list.search(list.begin(),"first_line") == list.begin(), true)
	TEST_EQUAL(list.search(list.begin(),"middle_line") == (list.begin()+3), true)
	TEST_EQUAL(list.search(list.begin(),"space_line",true) == (list.begin()+5), true)
	TEST_EQUAL(list.search(list.begin(),"tab_line",true) == (list.begin()+6), true)
	TEST_EQUAL(list.search(list.begin(),"last_line") == (list.end()-1), true)
	TEST_EQUAL(list.search(list.begin(),"invented_line") == list.end(), true)
	TEST_EQUAL(list.search(list.begin()+1,"first_line") == list.end(), true)

	//trim
	TEST_EQUAL(list.search(list.begin(),"first_line",true) == list.begin(), true)
	TEST_EQUAL(list.search(list.begin(),"space_line",true) == (list.begin()+5), true)
	TEST_EQUAL(list.search(list.begin(),"tab_line",true) == (list.begin()+6), true)
	TEST_EQUAL(list.search(list.begin(),"invented_line",true) == list.end(), true)
	TEST_EQUAL(list.search(list.begin()+1,"first_line",true) == list.end(), true)
END_SECTION

START_SECTION((ConstIterator search(const String& text, bool trim=false) const))
	StringList list(tmp_list);
	
	TEST_EQUAL(list.search("first_line") == list.begin(), true)
	TEST_EQUAL(list.search("middle_line") == (list.begin()+3), true)
	TEST_EQUAL(list.search("space_line") == list.end(), true)
	TEST_EQUAL(list.search("tab_line") == list.end(), true)
	TEST_EQUAL(list.search("last_line") == (list.end()-1), true)
	TEST_EQUAL(list.search("invented_line") == list.end(), true)
	TEST_EQUAL(list.search(" ") == (list.begin()+5), true)
	TEST_EQUAL(list.search("\t") == (list.begin()+6), true)
	
	//trim
	TEST_EQUAL(list.search("first_line",true) == list.begin(), true)
	TEST_EQUAL(list.search("space_line",true) == (list.begin()+5), true)
	TEST_EQUAL(list.search("tab_line",true) == (list.begin()+6), true)
	TEST_EQUAL(list.search("invented_line",true) == list.end(), true)
	
	//Try it on the same file (but trimmed)
	list = tmp_list2;

	TEST_EQUAL(list.search("first_line") == list.begin(), true)
	TEST_EQUAL(list.search("middle_line") == (list.begin()+3), true)
	TEST_EQUAL(list.search("space_line",true) == (list.begin()+5), true)
	TEST_EQUAL(list.search("tab_line",true) == (list.begin()+6), true)
	TEST_EQUAL(list.search("last_line") == (list.end()-1), true)
	TEST_EQUAL(list.search("invented_line") == list.end(), true)

	//trim
	TEST_EQUAL(list.search("first_line",true) == list.begin(), true)
	TEST_EQUAL(list.search("space_line",true) == (list.begin()+5), true)
	TEST_EQUAL(list.search("tab_line",true) == (list.begin()+6), true)
	TEST_EQUAL(list.search("invented_line",true) == list.end(), true)
END_SECTION

START_SECTION((ConstIterator searchSuffix(const ConstIterator& start, const String& text, bool trim=false) const))
	StringList list(tmp_list);
	
	TEST_EQUAL(list.searchSuffix(list.begin(),"invented_line",true) == list.end(), true)
	TEST_EQUAL(list.searchSuffix(list.begin(),"back_space_line",true) == list.begin()+7, true)
	TEST_EQUAL(list.searchSuffix(list.begin(),"back_tab_line",true) == list.begin()+8, true)
	TEST_EQUAL(list.searchSuffix(list.begin()+8,"back_space_line",true) == list.end(), true)
	
	TEST_EQUAL(list.searchSuffix(list.begin(),"invented_line") == list.end(), true)
	TEST_EQUAL(list.searchSuffix(list.begin(),"back_space_line") == list.end(), true)
	TEST_EQUAL(list.searchSuffix(list.begin(),"back_tab_line") == list.end(), true)
END_SECTION

START_SECTION((ConstIterator searchSuffix(const String& text, bool trim=false) const))
	StringList list(tmp_list);
	
	TEST_EQUAL(list.searchSuffix("invented_line",true) == list.end(), true)
	TEST_EQUAL(list.searchSuffix("back_space_line",true) == list.begin()+7, true)
	TEST_EQUAL(list.searchSuffix("back_tab_line",true) == list.begin()+8, true)
	
	TEST_EQUAL(list.searchSuffix("invented_line") == list.end(), true)
	TEST_EQUAL(list.searchSuffix("back_space_line") == list.end(), true)
	TEST_EQUAL(list.searchSuffix("back_tab_line") == list.end(), true)
END_SECTION

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
END_TEST
