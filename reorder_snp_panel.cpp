// This code takes two disjoint panels and combines them into one panel in a specified order.
// It takes a file with RSIDs and treats that as the correct order.
// It then takes two panel files, and combines them into one matching the RSID ordering to the RSID file.


#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

std::vector<std::string> split(const std::string & s, const std::string & by)
{
	std::vector<std::string> values;
	int start = 0;
	int end = s.find(by);
	while (end != std::string::npos)
	{
		values.push_back(s.substr(start, end - start));
		start = end + 1;
		end = s.find(by, start);
	}
	if (start < s.size())
	{
		values.push_back(s.substr(start));
	}
	return values;
}

int main()
{
	std::string directory = "";
	std::string rsid_filename = "";
	std::string reordered_panel_name = "";


	std::string panelA_filename = "";
	std::string panelB_filename = "";

	std::ifstream master;
	master.open(directory + rsid_filename);

	if (master.is_open())
	{
		std::cout << "Success" << std::endl;
		std::vector<std::string> rsid_ordering;
		while (!master.eof())
		{
			std::string line;
			std::getline(master, line);
			rsid_ordering.push_back(line);
		}
		master.close();

		std::ifstream panelA;
		std::unordered_map<std::string, std::string> snps;
		panelA.open(directory + panelA_filename);

		if (panelA.is_open())
		{
			std::string line;
			std::getline(panelA, line);
			snps["Header"] = line;
			while (!panelA.eof())
			{
				std::getline(panelA, line);
				std::string id = split(line, "\t")[1];
				snps[id] = line;
			}
			panelA.close();
		}

		std::ifstream panelB;
		panelB.open(directory + panelB_filename);
		if (panelB.is_open())
		{
			std::string line;
			std::getline(panelB, line);
			while (!panelB.eof())
			{
				std::getline(panelB, line);
				std::string id = split(line, "\t")[1];
				snps.operator[](id) = line;
			}
			panelB.close();
		}

		std::ofstream new_panel;
		new_panel.open(directory + reordered_panel_name);
		new_panel << snps["Header"] << std::endl;
		for (std::string id : rsid_ordering)
		{
			if (snps.find(id) != snps.end())
			{
				new_panel << snps[id] << std::endl;
			}
			else
			{
				std::cout << "Warning!!! Could not find id! " << id << std::endl;
			}
		}
	}
	else
	{
		std::cout << "Could not find the file" << std::endl;
	}
}
