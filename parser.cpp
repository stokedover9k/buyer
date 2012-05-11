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
    case BuyerParser::FASH:   c = Cell('c', 15);  off = 0;  break;
    default: throw
	"BuyerParser::getBrands(BuyerParser::InputType): unknown type requested";
    }
  for( int i = 0; i < N_BRANDS; i++ ) {
    v.push_back( std::atof(getCell(c).c_str()+off) );
    c.first++;
  }
  return v;
}

float BuyerParser::getModelParam(BuyerParser::InputType t, size_t i, size_t j) {
  Cell c;

  switch(t)
    {
    case BuyerParser::rho:     c = Cell('b', 5);  break;
    case BuyerParser::RHO:     c = Cell('b', 6);  break;
    case BuyerParser::lambda:  c = Cell('b', 9);  break;
    case BuyerParser::LAMBDA:  c = Cell('b',10);  break;
    case BuyerParser::phi:     c = Cell('t',21);  break;
    case BuyerParser::PHI:     c = Cell('t',22);  break;
    case BuyerParser::tau:     c = Cell('k',21);  break;
    case BuyerParser::TAU:     c = Cell('k',22);  break;
    case BuyerParser::gamma:   c = Cell('b',21);  break;
    case BuyerParser::GAMMA:   c = Cell('b',22);  break;
    case BuyerParser::DELTA:   c = Cell('b',40);  break;
    case BuyerParser::alpha:  
      throw "BuyerParser::getModelParam(): implementation missing for alpha param";
    case BuyerParser::ALPHA:  
      throw "BuyerParser::getModelParam(): implementation missing for ALPHA param";
    default:
      throw "BuyerParser::getModelParam(): unknown param requested";
    }
  
  float v = std::atof( getCell(c).c_str() );

  return v;
}

void BuyerParser::getAdCampaign(std::vector<std::vector<float> >& v) {
  v.clear();
  Cell start_c('c',41);

  for( int b=0; b<N_BRANDS; b++ ) {
    v.push_back(std::vector<float>(N_TIME_INTERVALS));
    Cell c = Cell(start_c.first + b, start_c.second);

    for( int t=0; t<N_TIME_INTERVALS; t++ ) {
      v[b][t] = std::atof( getCell(c).c_str() );
      c.second++;
    }
  }
}

void BuyerParser::getInteractions(std::vector<std::vector<std::pair<float,float> > >& v, 
				  size_t x) {
  v.clear();
  Cell st_a('k',42);
  Cell st_A('p',42);

  for( int i=0; i<x; i++ ) {
    v.push_back(std::vector<std::pair<float,float> >(x));
    for( int j=0; j<x; j++ ) {
      std::pair<float,float> p( std::atof( getCell(Cell(st_a.first+j, 
							st_a.second+i)).c_str() ),
				std::atof( getCell(Cell(st_A.first+j, 
							st_A.second+i)).c_str() )
				);
      v[i][j] = p;
    }
  }
}

void BuyerParser::getWeakInteractions(std::vector<std::vector<std::pair<float,float> > >& v, size_t agents, size_t ties) {
  v.clear();
  Cell st_b('k', 54);
  Cell st_B('p', 54);
  
  for( int i=0; i<agents; i++ ) {
    v.push_back(std::vector<std::pair<float,float> >(ties));
    
    for( int k=0; k<ties; k++ ) {
      std::pair<float,float> p( std::atof( getCell( Cell(st_b.first+i,
							 st_b.second+k) ).c_str() ),
				std::atof( getCell( Cell(st_B.first+i,
							 st_B.second+k) ).c_str() ) );
      v[i][k] = p;
    }
  }
}

void BuyerParser::getWeakTiesPreferences(std::vector<std::vector<float> >& ties,
					 size_t ties_count) {
  ties.clear();
  Cell start_c('c', 54);

  for( int k=0; k<ties_count; k++ ) {
    ties.push_back(std::vector<float>(N_BRANDS));
    Cell c = Cell(start_c.first, start_c.second+k);
  
    for( int b=0; b<N_BRANDS; b++ ) {
      ties[k][b] = std::atof( getCell(c).c_str() );
      c.first++;
    }
  }
}

std::string BuyerParser::getBrandName(size_t b_ind) {
  const std::string& s = getCell( Cell('c' + b_ind, 1) );
  return s.substr(1, s.length()-2);
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
