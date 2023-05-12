// This code takes a VCF and generates a CSV file
// This code is a remnant of how we did our preprocessing
// Any other method of preprocessing before running the secure protocols is acceptable
// The only requirement is that the final file has space separated 0 and 1

// The "directory" and "vcf" variables together locate the input file
// THe "target" variable indicates the location to write the resulting csv file to.

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

std::vector<std::string> split(const std::string & operand, const std::string & delimiter)
{
	std::vector<std::string> values;
	size_t start = 0;
	size_t end = operand.find(delimiter);
	while (end != std::string::npos)
	{
		std::string token = operand.substr(start, end - start);
		values.push_back(token);
		start = end + 1;
		end = operand.find(delimiter, start);
	}
	return values;
}

int main()
{
	
	std::string directory = "";
	std::ifstream inputfile;
	std::ofstream outputfile;
	std::string HG = "HG007";
	std::string vcf = HG + "_GRCh38_1_22_v4.2.1_benchmark.vcf";
	inputfile.open(directory + vcf);
	if (inputfile.is_open())
	{
		std::string line;
		int k = 0;
		while (line.find("#CHROM\tPOS\tID\tREF\tALT\tQUAL\tFILTER\tINFO\tFORMAT") == std::string::npos)
		{
			std::getline(inputfile, line);
			k = k + 1;
		}
		std::string target = HG + "_GRCh38.csv";
		outputfile.open(directory + target);
		if (outputfile.is_open())
		{
			std::cout << "Converting file now." << std::endl;

			std::string delimiter = "\t";
			while (!inputfile.eof())
			{
				std::vector<std::string> values = split(line, "\t");
				for (size_t i = 0; i < 7; ++i)
				{
					outputfile << values[i] << ", ";
				}
				outputfile << std::endl;
				std::getline(inputfile, line);
			}
			outputfile.close();
		}
		inputfile.close();
	}
	else
	{
		std::cout << "could not open file" << std::endl;
	}
	return 0;
}
