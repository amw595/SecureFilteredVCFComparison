# SecureFilteredVCFComparison

This code is used to run the experiments of our paper. The C++ code is first used to preprocess the VCF files into inputs that can be used by the mpc code that is compiled by the MP-SPDZ code.

You can run this experiment locally (much quicker and easier to verify), or you can run over a network, which requires a little more work but can ensure computation is done legitimately.

While we include the cpp files which were used to filter the VCF retrieved from https://github.com/genome-in-a-bottle/giab_latest_release the code must be edited to be used as is. This is because you have to specify details such as the directory location, file names, and output file destinations. Alternatively, we include some example preprocessed files which you can use to run the experiments: `HG00\*\_4700.txt`. These files contain 4763 binary digits in text format. They can be read as is by the `mpc` code included in this repository. Follow the steps below to run the experiments using the included preprocessed files:

First, install the MP-SPDZ package from their github: https://github.com/data61/MP-SPDZ. We recommend using Ubuntu for the operating system to use the MP-SPDZ framework. The authors of MP-SPDZ include a list of packages to install in the Ubuntu environment, which we will also include here for convenience:
```
sudo apt-get install automake build-essential clang cmake git libboost-dev libboost-thread-dev libgmp libntl-dev libsodium-dev libssl-dev libtool python3
```

Once you have been able to run one of their scripts, you should do the following:

To allow player interaction, they need keys which can be setup by the following commands:
```
$ Scripts/setup-online.sh
```

and
```
$ Scripts/setup-ssl.sh
```

These commands create a directory called `Player-Data` and put keys the players use in the protocols into this directory. This directory will also be used to store the data the players wish to input to the computation.

Next, copy `jaccard_similarity.mpc`, `set_diff.mpc`, and `symmetric_diff.mpc` to `Programs/Source` in the `MP-SPDZ` directory.
Once these have been copied into the `Programs/Source` directory, run
```
$ ./compile.py jaccard_similarity
$ ./compile.py set_diff
$ ./compile.py symmetric_diff
```

Now copy one of the `HG00*_4700.txt` files to `Player-Data/Input-P0-0` and another `HG00*_4700.txt` file into `Player-Data/Input-P1-0`. This allows the framework to use the data as input from party P0 and party P1 respectively. 

Before running the scripts to execute the mpc code, you must compile the executables they rely on:
* `$ make shamir` compiles the executables used by the `shamir.sh` and `mal-shamir.sh` scripts.
* `$ make arithmetic` compiles the executables used by the `semi.sh` and `mascot.sh`.

Now you can run any of the following scripts:
* `$ Scripts/shamir.sh jaccard_similarity`
* `$ Scripts/mal-shamir.sh jaccard_similarity`
* `$ Scripts/semi.sh jaccard_similarity`
* `$ Scripts/mascot.sh jaccard_similarity`

If you wish, you can replace `jaccard_similarity` with `symmetric_diff` or `set_diff` as well.

Next, in order to run over multiple machines, the following must happen:

You need at least three separate machines. On each machine, make sure that you can do the above steps. Then you must decide a host machine. The host machine will also be player `P0`. In a text file include on each new line the ip address of each machine and the port to access the machine from. Using example IP-addresses, an example file may look like
```
192.0.2.0:5000
192.0.2.1:5001
192.0.2.1:5002
```

Where you would replace the ip addresses here with addresses used by your machines. We will refer to this file as `sample_ip_file.txt` and you will replace with your own file and file name.

Now, if every machine can run the above steps locally, then the only thing left to do here, is to make sure every machine here shares the same keys. A simple way to do that for experimental purposes only would be to have one machine create the keys locally and distribute them to the other machines. Using the above machines as examples we give a quick overview on the update to running the code over the `Scripts/...` method above. First note that only machine `192.0.2.0` needs `Player-Data/Input-P0-0` file populated, and `192.0.2.1` needs `Player-Data/Input-P1-0` file populated.

The machine with the IP address 192.0.2.0 will run the command
```
./shamir-party.x 0 jaccard_similarity --ip-file-name sample_ip_file.txt -N 3
```

The machine with the IP address 192.0.2.1 will run the command
```
./shamir-party.x 1 jaccard_similarity --ip-file-name sample_ip_file.txt -N 3
```

The machine with the IP address 192.0.2.2 will run the command
```
./shamir-party.x 2 jaccard_similarity --ip-file-name sample_ip_file.txt -N 3
```

The first argument in the bash command is the executable. The second is the id marker for the party (machine) used in the protocol. The third is the indicator that you will provide an ip-file and the last is indicating how many parties will participate. Until each party participates the program will wait until a timeout. Do not forget that some protocols like `shamir-party.x` will require a minimum of 3 parties while others like `mascot-party.x` and `semi-party.x` only need 2.

You can replace `./shamir-party.x` with the following to replicate our experiments
* `./malicious-shamir-party.x` corresponds with `Scripts/mal-shamir.sh`
* `./semi-party.x` corresponds with `Scripts/semi.sh`
* `./mascot-party.x` corresponds with `Scripts/mascot.sh`

The only other thing you would need to change is the number of players using the `-N` argument as in the examples above.


To generate the input files `HG00*_4700.txt`:
* Create a directory which has a SNP panel that includes RSIDs.
 * It is better to have Chromosome Position, but we can get that from the RSIDs
* Add the VCFs from Genome in a Bottle to that same directory
 * Download the version that matches the SNP panel version, especially for when RSID is not available in the VCF
* Extract Variants from VCF using vcf_variant_extraction.cpp
 * Make sure to update the cpp file to know where the files are
* Filter the SNPs using snp_filter.cpp
 * Make sure to update the file to know where the files are
 * File includes details on what it expects for the update
 * 
