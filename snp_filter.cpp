// This file takes a SNP panel, and filters a list of VCFS
// One major limitation is that this file does not process a traditional VCF
// Instead, it processes a CSV file as extracted from a VCF using vcf_variant_extraction.cpp
// 
// This file also assumes that we are using Genome in a Bottle HG002 to HG007 and processes those
// The important variables are 
//	* directory: the directory where all the files can be found
//  * snp_panel: the filename of the SNP panel
//  * snp_delimiter: the delimiter of the SNP panel file (sometimes the delimiter was tab, sometimes it was comma)
//  * filename_ending: In our case each filename had the same ending, the filename_ending variable is used for that
// 
// 
// This process is unnecessary for our method, but it is the way we did preprocessing
// The output is a file with space separated integer values 0 and 1. The file will be called "HG00i_bit_array.txt" where i = 2, 3, ..., 7
// The file will be located in the same directory as the input files.

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <chrono>

std::vector<std::string> split(const std::string & operand, const std::string & delimiter)
{
	std::vector<std::string> values;
	size_t start = 0;
	size_t end = operand.find(delimiter);
	while (end != std::string::npos)
	{
		std::string token;
		if (end < operand.size())
			token = operand.substr(start, end - start);
		values.push_back(token);
		start = end + 1;
		if (start < operand.size())
			end = operand.find(delimiter, start);
		else
			end = std::string::npos;
	}
	return values;
}

std::string strip(const std::string& operand)
{
	if (operand.size() == 0) return operand;
	size_t start = -1;
	while (isspace(operand[++start]) && start + 1 < operand.size());
	size_t end = operand.size();
	while (end > start && isspace(operand[--end]));
	return operand.substr(start, end - start + 1);
}

size_t get_index(std::ifstream & file, const std::string& name, const std::string& delimiter)
{
	std::string line;
	std::getline(file, line);
	if (line.find(name) != std::string::npos)
	{
		std::vector<std::string> values = split(line, delimiter);
		size_t index = 0;
		for (auto value : values)
		{
			if (value.find(name) != std::string::npos)
			{
				return index;
			}
			index += 1;
		}
	}
	return -1;
}

std::unordered_map<std::string, int> get_snps(std::ifstream & file, size_t index, const std::string& delimiter)
{
	std::unordered_map<std::string, int> snps;
	int k = 0;
	while (!file.eof())
	{
		std::string line;
		std::getline(file, line);
		std::vector<std::string> values = split(line, delimiter);
		if (values.size() > index)
			snps[strip(values[index])] = k;
		k += 1;
	}
	return snps;
}

std::string combine_strings(const std::string & s1, const std::string & s2)
{
	return s1 + ", " + s2;
}

std::unordered_map<std::string, int> get_snps(std::ifstream& file, size_t index1, size_t index2, const std::string& delimiter)
{
	std::unordered_map<std::string, int> snps;
	int k = 0;
	while (!file.eof())
	{
		std::string line;
		std::getline(file, line);
		std::vector<std::string> values = split(line, delimiter);
		if (values.size() > index1 && values.size() > index2)
		{
			std::string key = combine_strings(strip(values[index1]), strip(values[index2]));
			snps[key] = k;
		}
		k += 1;
	}
	return snps;
}

std::vector<int> build_bit_array(std::ifstream& file, const std::unordered_map<std::string, int>& snp_panel, std::vector<std::string>(*fileprocessor)(std::ifstream &), std::string(*indexer)(const std::vector<std::string> &), const std::string& delimiter)
{
	std::vector<int> bit_array(snp_panel.size(), 0);

	int matches = 0;
	while (!file.eof())
	{
		std::vector<std::string> lines = fileprocessor(file);
		for (std::string line : lines)
		{
			std::vector<std::string> values = split(line, delimiter);
			std::string token = indexer(values);
			std::cout << token << std::endl;
			if (snp_panel.find(token) != snp_panel.end())
			{
				int bit_ind = snp_panel.at(token);
				if (bit_ind < bit_array.size())
				{
					bit_array[bit_ind] = 1;
					matches += 1;
				}
			}

		}
	}
	return bit_array;
}

std::vector<int> build_bit_array(std::ifstream& file, const std::unordered_map<std::string, int>& snp_panel, std::string(*fileprocessor) (std::ifstream&), std::string(*indexer)(const std::vector<std::string>&), char delimiter)
{
	std::vector<int> bit_array;
	for (int i = 0; i < snp_panel.size(); ++i)
	{
		bit_array.push_back(0);
	}
	int matches = 0;
	std::vector<std::string> values;
	std::string value;
	while (!file.eof())
	{
		std::string data = fileprocessor(file);
		std::string cv;
		size_t index = 0;
		while (index < data.size())
		{
			while (index < data.size() && data[index] != '\n')
			{
				if (data[index] == delimiter)
				{
					values.push_back(strip(value));
					value = "";
				}
				else if (data[index] >= 0 && data[index] <= 256 && !isspace(data[index]))
				{
					value += data[index];
				}
				index++;
			}
			if (data[index] == '\n')
			{
				if (value != "")
				{
					values.push_back(value);
					value = "";
				}
				std::string token = indexer(values);
				//std::cout << token << std::endl;
				if (snp_panel.find(token) != snp_panel.end())
				{
					int bit_ind = snp_panel.at(token);
					if (bit_ind < bit_array.size())
					{
						bit_array[bit_ind] = 1;
						matches++;
					}
				}
				values = std::vector<std::string>();
			}
			index++;
		}
	}
	std::cout << matches << std::endl;
	return bit_array;
}


inline
std::vector<std::string> single_line_read(std::ifstream& file)
{
	std::string line;
	std::getline(file, line);
	std::vector<std::string> result({ line });
	return result;
}

inline
std::vector<std::string> read_blocks(std::ifstream& file)
{
	std::vector<std::string> result;
	char* block = new char[1024 * 1024];

	file.read(block, 1024 * 1024);
	
	std::string s;
	for (size_t i = 0; i < 1024 * 1024; ++i)
	{
		if (block[i] == '\n')
		{
			result.push_back(s);
			s = "";
		}
		else
		{
			s += block[i];
		}

	}
	delete[] block;
	return result;
}

inline
std::string read_blocks_string(std::ifstream& file)
{
	const size_t BLOCK_SIZE = 1024 * 256;
	char * buffer = new char[BLOCK_SIZE];
	file.read(buffer, BLOCK_SIZE);
	std::string result = buffer;
	delete[] buffer;
	return result;
}

inline
std::string get_chrom_pos(const std::vector<std::string> & values)
{
	if (values.size() > 1)
		return combine_strings(values[0], values[1]);
	return "";
}

inline
std::string get_chrom_pos_rm_chr(const std::vector<std::string>& values)
{
	if (values.size() > 1)
		return combine_strings(values[0].substr(3), values[1]);
}

inline
std::string get_id(const std::vector<std::string> & values)
{
	if (values.size() > 2)
		return values[2];
	return "";
}



int dot_product(const std::vector<int>& a, const std::vector<int>& b)
{
	int c = 0;
	for (int i = 0; i < a.size(); ++i)
	{
		c += a[i] * b[i];
	}
	return c;
}

int snp_share(const std::vector<int>& a, const std::vector<int>& b)
{
	int c = 0;
	for (int i = 0; i < a.size(); ++i)
	{
		c += 1 - (a[i] + b[i] - 2 * a[i] * b[i]);
	}
	return c;
}

int main()
{
	std::string directory = "";
	std::string snp_panel = "";
	std::string snp_delimiter = "\t";

	std::string filename_ending = "_GRCh38.csv";

	std::ifstream snp_panel_file;
	std::ifstream source;
	std::ofstream target;

	std::cout << "Making SNP Panel" << std::endl;
	// prepare snp panel
	snp_panel_file.open(directory + snp_panel);

	size_t id_index_panel = get_index(snp_panel_file, "ID", snp_delimiter);
	size_t chrom_index_panel = 2;
	size_t pos_index_panel = 3;
	std::unordered_map<std::string, int> panel_snp_ids = get_snps(snp_panel_file, chrom_index_panel, pos_index_panel, snp_delimiter);
	std::cout << panel_snp_ids.size() << std::endl;


	snp_panel_file.close();

	for (int i = 2; i <= 7; ++i)
	{

		std::string HG = "HG00" + std::to_string(i);
		std::cout << "Processing " << HG << std::endl;

		source.open(directory + HG + filename_ending);

		size_t id_index = get_index(source, "ID", ",");
		size_t chrom_index = 0;
		size_t pos_index = 1;
		time_t before_build = time(NULL);
		auto start_time = std::chrono::high_resolution_clock::now();
		std::vector<int> a = build_bit_array(source, panel_snp_ids, read_blocks_string, get_chrom_pos_rm_chr, ',');

		auto end_time = std::chrono::high_resolution_clock::now();
		time_t after_build = time(NULL);
		double seconds = difftime(after_build, before_build);
		std::cout << "Construction took " << seconds << "seconds" << std::endl;
		std::cout << "Chrono measured " << (end_time - start_time)/std::chrono::milliseconds(1) << std::endl;
		source.close();

		std::cout << "saving bit array" << std::endl;
		target.open(directory + HG + "_bit_array.txt", std::ios::trunc);

		for (int i = 0; i < a.size(); ++i)
		{
			target << std::to_string(a[i]) << ' ';
		}
		target << std::endl;
		target.close();
	}

	int wait;
	std::cin >> wait;
	return 0;
}
