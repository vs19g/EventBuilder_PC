#include <iostream>
#include <string>
#include "TTree.h"
#include "TFile.h"

void PruneTree(int runNumber)
{
  std::string input_filename = "/Users/theefizzicist/Documents/Projects/18Ne-a-p/WorkingData/trees/run_"+std::to_string(runNumber)+"_18F.root";
  std::string output_filename = "/Users/theefizzicist/Documents/Projects/18Ne-a-p/WorkingData/trees/run_"+std::to_string(runNumber)+"_18F_pruned.root";
  
  std::cout<<"Pruning data from "<<input_filename<<std::endl;

  // ***** Input File *****
  TFile *inputfile = TFile::Open(input_filename.c_str(), "READ");
  if(inputfile == nullptr || !inputfile->IsOpen()){
    std::cout<<"Could not find file "<<input_filename<<". Skipping."<<std::endl;
    delete inputfile;
    return;
  }

  TTree *inTree = (TTree *)inputfile->Get("BarcQQQTreeData");
  if(inTree == nullptr){
    std::cout<<"No tree named BarcQQQTreeData found inside file "<<input_filename<<". Skipping."<<std::endl;
    inputfile->Close();
    delete inputfile;
    return;
  }

  // ***** Output File *****
  TFile *outputfile = TFile::Open(output_filename.c_str(), "RECREATE");
  std::cout<<"Writing to "<<output_filename<<std::endl;

  // ***** initialize *****
  int QQQmult, BarcMult, BarcUpMult, BarcDnMult;
  ULong64_t n = inTree->GetEntries();
   
  //link variables to values from tree
  inTree->SetBranchAddress("QQQmult",&QQQmult);
  inTree->SetBranchAddress("BarcMult",&BarcMult);
  
  //copy tree header only
  TTree *outTree = inTree->CloneTree(0);
  
  for(ULong64_t i=0; i<n; i++) // loop over all events
  {
    //load entry
    inTree->GetEntry(i);

    //if pruning conditions met, copy entry to new tree
    if(QQQmult>0 && BarcMult>0){outTree->Fill();}

    if(i%10000 == 0) std::cout<<"Entry "<<i<<" of "<<n<<", "<<100 * i/n<<"\% complete"<<"\r"<<std::flush;
  }
  
  outputfile->cd();
  //std::cout << "Printing" << std::endl; outTree->Print();
  std::cout << "\nWriting" << std::endl; outTree->Write();
  inputfile->Close();
  outputfile->Close();  
  delete outputfile;
  delete inputfile;
}

/*void PruneTree(int runMin, int runMax)
{
  for(int i=runMin; i<=runMax; i++)
    Run_PruneTree(i);
}*/