// g++ -o Mc2root Mc2root.cxx `root-config --cflags --libs`
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>

#include "TFile.h"
#include "TTree.h"

int main(int argc, char** argv)
{
	if(argc != 3 && argc != 4) {
		std::cout<<"usage: "<<argv[0]<<" <input file> <output file> <option debug flag>"<<std::endl;
		return 1;
	}

	bool debug = false;
	if(argc == 4) debug = true;

	// some flags that are hard coded for now, but could be made to be read from command line
	bool calibrated = false; // controls whether calibrated data (energy) has been written
	bool writeWaveform = false; // controls whether we write waveforms

	std::string inputFileName = argv[1];

	// open output root file
	TFile output(argv[2], "recreate");
	if(!output.IsOpen()) {
		std::cerr<<"Failed to open output root file "<<argv[2]<<std::endl;
	}

	// create tree
	TTree* tree = new TTree("dt5780","data from dt5780");

	// create and add leafs
	uint16_t board;
	tree->Branch("board", &board);
	uint16_t channel;
	tree->Branch("channel", &channel);
	uint16_t charge;
	tree->Branch("charge", &charge);
	ULong64_t timestamp;
	tree->Branch("timestamp", &timestamp);
	double energy;
	if(calibrated) {
		tree->Branch("energy", &energy);
	}
	uint32_t flags;
	tree->Branch("flags", &flags);
	uint32_t nofSamples;
	std::vector<uint16_t> waveform;
	if(writeWaveform) {
		tree->Branch("waveform", &waveform);
	}

	// open input file
	std::ifstream input(argv[1], std::ios::binary);

	if(!input.is_open()) {
		std::cerr<<"Failed to open '"<<argv[1]<<"'!"<<std::endl;
		return 1;
	}
	std::cout<<"Opened '"<<argv[1]<<"'!"<<std::endl;

	// get size of input file
	input.seekg(0, input.end);
	size_t fileSize = input.tellg();
	input.seekg(0, input.beg);

	// create buffer and read file into it (might want to check how much memory we create here?)
	std::vector<uint16_t> buffer(fileSize/sizeof(uint16_t));
	input.read(reinterpret_cast<char*>(buffer.data()), fileSize);

	// loop over buffer
	size_t pos = 0;
	size_t entry = 0;
	uint64_t tmp = 0;
	while(pos < buffer.size()) {
		// first 16 bit word is board
		board = buffer[pos++];
		// second 16 bit word is channel
		channel = buffer[pos++];
		// next four 16 bit words make up the timestamp
		timestamp = (static_cast<uint64_t>(buffer[pos+3])<<48)|(static_cast<uint64_t>(buffer[pos+2])<<32)|(static_cast<uint64_t>(buffer[pos+1])<<16)|(buffer[pos]);
		pos += 4;
		// next 16 bit word is charge (we assume this is always written!)
		charge = buffer[pos++];
		// if calibrated energy is written, the next four 16 bit words make up the (floating point) energy
		if(calibrated) {
			// we first need to assemble the 64bit word, then reinterpret it as a double
			tmp = (static_cast<uint64_t>(buffer[pos+3])<<48)|(static_cast<uint64_t>(buffer[pos+2])<<32)|(static_cast<uint64_t>(buffer[pos+1])<<16)|(buffer[pos]);
			energy = *reinterpret_cast<double*>(&tmp);
			pos += 4;
		}
		// next two 16 bit words make up the flags
		flags = (static_cast<uint32_t>(buffer[pos+1])<<16)|(buffer[pos]);
		pos += 2;
		// next two 16 bit words make up the number of waveform samples
		nofSamples = (static_cast<uint32_t>(buffer[pos+1])<<16)|(buffer[pos]);
		pos += 2;
		// read all 16 bit waveform samples
		waveform.clear();
		for(int s = 0; s < nofSamples; ++s) {
			waveform.push_back(buffer[pos++]);
		}
		tree->Fill();
		++entry;
		if(entry%1000 == 0) {
			std::cout<<(100*pos)/buffer.size()<<"% done, "<<pos<<"/"<<buffer.size()<<" words read\r"<<std::flush;
		}
	} // end of buffer loop

	std::cout<<"100% done, "<<pos<<"/"<<buffer.size()<<" words read = "<<entry<<" entries"<<std::endl;

	tree->Write();
	output.Close();

	return 0;
}
