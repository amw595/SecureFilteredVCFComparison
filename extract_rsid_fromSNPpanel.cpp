// This code extracts RSIDs from the file specified.
// One caveat to this code is that is assumes that "rs" always precedes and RSID.
// If the file does not have "rs" or if "rs" is found anywhere outside of an RSID this will cause issues.

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

int main()
{
    std::string panel_filename = "";
    std::string rsid_filename = "";

    std::ifstream snp_panel;
    snp_panel.open(panel_filename);
    if (snp_panel.is_open())
    {
        std::vector<std::string> ids;
        std::cout << "Success" << std::endl;
        int lineno = 0;
        while (!snp_panel.eof())
        {
            std::string line;
            std::getline(snp_panel, line);
            std::string rsid;
            size_t k = line.find("rs");
            if (k != std::string::npos)
            {
                while (k < line.size() && line[k] != ' ')
                {
                    rsid += line[k];
                    k += 1;
                }
                ids.push_back(rsid);
            }
        }
        snp_panel.close();
        std::ofstream rsid_file;
        rsid_file.open(rsid_filename);
        if (rsid_file.is_open())
        {
            for (auto rsid : ids)
            {
                rsid_file << rsid << std::endl;
            }
        }
        else
        {
            std::cout << "Failed to write" << std::endl;
        }
    }

}
