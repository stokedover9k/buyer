#include "parser.h"

BuyerParser::BuyerParser() : data()
{  }

BuyerParser::BuyerParser(std::istream& file) : data()
{
  readFile(file);
}

BuyerParser::BuyerParser(const char* filename) : data()
{
  std::ifstream file(filename);
  readFile(file);
  file.close();
}

void BuyerParser::readFile(std::istream& file)
{
  CSVRow row;
  while( file >> row )
    data.push_back(row);
}

std::string const& BuyerParser::getCell(const Cell& c) {
  return data[c.second-1][c.first-'a'];
}

float BuyerParser::getBudget(void) {
  return std::atof( getCell(Cell('b',4)).c_str()+3 );
}

std::vector<float> BuyerParser::getBrands(BuyerParser::InputType t) {
  std::vector<float> v;
  Cell c;
  size_t off = 0;  //offset
  switch(t)
    {
    case BuyerParser::PRICE:  c = Cell('c',  3);  off = 3;  break;
    case BuyerParser::TRUST:  c = Cell('l', 15);  off = 0;  break;
    case BuyerParser::FIT:    c = Cell('u', 15);  off = 0;  break;
    default: throw
	"BuyerParser::getBrands(BuyerParser::InputType): unknown type requested";
    }
  for( int i = 0; i < N_BRANDS; i++ ) {
    v.push_back( std::atof(getCell(c).c_str()+off) );
    c.first++;
  }
  return v;
}

///////////////////////////////////////////////////////////////////////////////

std::string const& CSVRow::operator[](std::size_t index) const
{ return m_data[index]; }

std::size_t CSVRow::size(void)
{ return m_data.size(); }

void CSVRow::readNextRow(std::istream& str)
{
  std::string line;
  std::getline(str,line);
    
  std::stringstream lineStream(line);
  std::string cell;

  m_data.clear();
  while(std::getline(lineStream,cell,',')) {
    m_data.push_back(cell);
  }
}

std::istream& operator>>(std::istream& str, CSVRow& data)
{
  data.readNextRow(str);
  return str;
}
