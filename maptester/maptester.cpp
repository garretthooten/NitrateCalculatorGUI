#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

int main(int argc, char* argv[])
{
  if(argc < 3)
  {
    std::cerr << "Usage: <path/to/file1> <path/to/file2>" << std::endl;
    return 1;
  }
  std::string file1 = argv[1];
  std::string file2 = argv[2];

  std::ifstream file1Data(file1.c_str());
  std::ifstream file2Data(file2.c_str());

  std::vector< std::vector<std::string> > parsedCSV1;
  std::vector< std::vector<std::string> > parsedCSV2;

  if(file1Data.is_open())
  {
    std::string line;
    while(getline(file1Data, line))
    {
      std::stringstream linestream(line);
      std::string cell;
      std::vector<std::string> parsedRow;
      while(getline(linestream, cell, ','))
      {
        parsedRow.push_back(cell);
      }
      parsedCSV1.push_back(parsedRow);
    }
  }
  file1Data.close();

  if(file2Data.is_open())
  {
    std::string line;
    while(getline(file2Data, line))
    {
      std::stringstream linestream(line);
      std::string cell;
      std::vector<std::string> parsedRow;
      while(getline(linestream, cell, ','))
      {
        parsedRow.push_back(cell);
      }
      parsedCSV2.push_back(parsedRow);
    }
  }
  file2Data.close();

  int file1Null = 0;
  int file2Null = 0;

  for(int i = 0; i < parsedCSV1.size(); i++)
  {
    for(int j = 0; j < parsedCSV1[i].size(); j++)
    {
      if(parsedCSV1[i][j] == "-9999")
        file1Null++;
      if(parsedCSV2[i][j] == "-9999")
        file2Null++;
    }
  }

  std::cout << "file1Null: " << file1Null << "\nfile2Null: " << file2Null << std::endl;

  return 0;
}
