#ifndef __TableParser_h_
#define __TableParser_h_

#include <string>
#include <vector>

struct Table2D 
{
  typedef std::string Value;
  typedef std::vector<Value> Values;
  
  struct Row
  {
    std::string myTitle;
    std::string myUnit;
    Values myValues;
  };
  
  std::string myTitle;
  std::vector<std::string> myColumnUnits;
  std::vector<std::string> myColumnTitles;
  
  typedef std::vector<Row> Rows;
  Rows myRows;
  
  bool Check();
};

std::vector<std::string> GetTableNames(const char* fname, const char* separator,
				       const bool firstStringAsTitles);
Table2D GetTable(const char* fname, const char* separator, const int tableNb,
		 const bool firstStringAsTitles);

#endif //__TableParser_h_
