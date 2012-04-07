/**
   NOTE: CSVRow class taken from
   http://stackoverflow.com/questions/1120140/csv-parser-in-c
   by Loki Astari
 */

#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <utility>
#include <cstdlib>

#include "input-params.h"

class CSVRow;
typedef std::pair<char,size_t> Cell;

class BuyerParser
{
 public:
  enum InputType {
    PRICE,
    TRUST,
    FIT,
  };

  BuyerParser();
  BuyerParser( std::istream& file );
  BuyerParser( const char* filename );
  
  void readFile( std::istream& file );

  std::string const& getCell(const Cell&);

  float              getBudget(void);
  std::vector<float> getBrands(InputType);

 private:
  std::vector<CSVRow> data;
};

///////////////////////////////////////////////////////////////////////////////

class CSVRow
{
 public:

  std::string const& operator[](std::size_t index) const;
  std::size_t size(void);
  void readNextRow(std::istream& str);

 private:
  std::vector<std::string> m_data;
};

std::istream& operator>>(std::istream& str, CSVRow& data);



  
