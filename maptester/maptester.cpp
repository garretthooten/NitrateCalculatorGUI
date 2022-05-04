#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

std::vector< std::vector<float> >string_to_float(std::vector< std::vector<std::string> > string_CSV, int starting_value)
{
    std::vector< std::vector<float> > return_vector;
    for(int i = starting_value; i < string_CSV.size(); i++)
    {
        std::vector<float> temp_row;
        for(int j = 0; j < string_CSV[i].size(); j++)
        {
            //std::cout << "converting item " << string_CSV[i][j] << std::endl;
            temp_row.push_back(std::stof(string_CSV[i][j]));
        }
        return_vector.push_back(temp_row);
    }
    return return_vector;
}

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

  std::cout << "\nFiles loaded!" << std::endl;

  std::vector< std::vector<float> > file1Float = string_to_float(parsedCSV1, 6);
  std::vector< std::vector<float> > file2Float = string_to_float(parsedCSV2, 6);
  std::cout << "file1Float size: " << file1Float.size() << "\nFile1Float[0] size: " << file1Float[0].size() << std::endl;
  std::cout << "file2Float size: " << file2Float.size() << "\nFile2Float[0] size: " << file2Float[0].size() << std::endl;

  int file1Null = 0;
  int file2Null = 0;

  float file1Sum = 0.0;
  float file2Sum = 0.0;

  int file1Count = 0;
  int file2Count = 0;

  for(int i = 0; i < file1Float.size(); i++)
  {
    for(int j = 0; j < file1Float[i].size(); j++)
    {
      if(file1Float[i][j] == -9999)
        file1Null++;
      else
      {
        file1Sum += file1Float[i][j];
        file1Count++;
      }

      if(file2Float[i][j] == -9999)
        file2Null++;
      else
      {
        file2Sum += file2Float[i][j];
        file2Count++;
      }
    }
  }

  std::cout << "file1Null: " << file1Null << "\nfile2Null: " << file2Null << std::endl;
  std::cout << "\nfile1Sum: " << file1Sum << "\nfile1Count: " << file1Count << "\nfile1Avg: " << (file1Sum/file1Count) << std::endl;
  std::cout << "\nfile2Sum: " << file2Sum << "\nfile2Count: " << file2Count << "\nfile2Avg: " << (file2Sum/file2Count) << std::endl;

  return 0;
}
