// This file is part of TeleSculptor, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/TeleSculptor/blob/master/LICENSE for details.

/**
 * File: QueryResults.cpp
 * Date: March, November 2011
 * Author: Dorian Galvez-Lopez
 * Description: structure to store results of database queries
 * License: see the LICENSE_DBoW2.txt file
 *
 */

#include <iostream>
#include <fstream>
#include "QueryResults.h"

using namespace std;

namespace DBoW2
{

// ---------------------------------------------------------------------------

ostream & operator<<(ostream& os, const Result& ret )
{
  os << "<EntryId: " << ret.Id << ", Score: " << ret.Score << ">";
  return os;
}

// ---------------------------------------------------------------------------

ostream & operator<<(ostream& os, const QueryResults& ret )
{
  if(ret.size() == 1)
    os << "1 result:" << endl;
  else
    os << ret.size() << " results:" << endl;

  QueryResults::const_iterator rit;
  for(rit = ret.begin(); rit != ret.end(); ++rit)
  {
    os << *rit;
    if(rit + 1 != ret.end()) os << endl;
  }
  return os;
}

// ---------------------------------------------------------------------------

void QueryResults::saveM(const std::string &filename) const
{
  fstream f(filename.c_str(), ios::out);

  QueryResults::const_iterator qit;
  for(qit = begin(); qit != end(); ++qit)
  {
    f << qit->Id << " " << qit->Score << endl;
  }

  f.close();
}

// ---------------------------------------------------------------------------

} // namespace DBoW2

