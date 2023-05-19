# SecureFilteredVCFComparison

This code is used to run the experiments of our paper. The C++ code is first used to preprocess the VCF files into inputs that can be used by the mpc code that is compiled by the MP-SPDZ code.

There are two different experiments you can do with this data. If you wish, you can run the experiments using the preprocessed bit map files HG00\*\_4700.txt. First, install the MP-SPDZ package
from their github: https://github.com/data61/MP-SPDZ

Run their commands `make` commands, and make sure you run the scripts
```
Scripts/setup-online.sh
```

and
```
Scripts/setup-ssl.sh
```

Once these have been run, there should now be a folder called `Player-Data`.

In this folder, you will have the keys for each player.

Once you have setup the MP-SPDZ framework and are able to run , you must copy one file to `Player-Data/Input-P0-0` and another into `Player-Data/Input-P1-0`.

Then compile 

The experiment we ran had the following steps:
* Create a directory which has a SNP panel that includes RSIDs.
 * It is better to have Chromosome Position, but we can get that from the RSIDs
* Add the VCFs from Genome in a Bottle to that same directory
 * Download the version that matches the SNP panel version, especially for when RSID is not available in the VCF
* Extract Variants from VCF using vcf_variant_extraction.cpp
 * Make sure to update the cpp file to know where the files are
* Filter the SNPs using snp_filter.cpp
 * Make sure to update the file to know where the files are
 * File includes details on what it expects for the update
* Place resulting files into the MP-SPDZ/Player-Data/ folder as the input files for the respective party
 * Party P0 has file Input-P0-0
 * Party P1 has file Input-P1-0

Before running these steps, it is important to setup the MP-SPDZ framework. Their documentation gives instructions for getting started: https://mp-spdz.readthedocs.io/en/latest/readme.html

