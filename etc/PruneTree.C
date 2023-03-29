#include <iostream>
#include <string>
#include "TTree.h"
#include "TFile.h"

void PruneTree(int runNumber)
{
  std::string input_filename = "/Users/theefizzicist/Documents/Projects/18Ne-a-p/WorkingData/trees/run_"+std::to_string(runNumber)+"_18F.root";
  std::string output_filename = "/Users/theefizzicist/Documents/Projects/18Ne-a-p/WorkingData/trees/run_"+std::to_string(runNumber)+"_18F_pruned.root";
  
  std::cout<<"Pruning data in "<<input_filename<<std::endl;

  // ***** Input File *****
  TFile *inputfile = TFile::Open(input_filename.c_str(), "READ");
  TTree *inTree = (TTree *)inputfile->Get("BarcQQQTreeData");

  // ***** Output File *****
  TFile *outputfile = TFile::Open(output_filename.c_str(), "RECREATE");

  // ***** initialize *****
  int QQQmult, BarcMult, BarcUpMult, BarcDnMult;
  ULong64_t n = inTree->GetEntries();
  
  std::cout << "Entries = " << n << std::endl;
 
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
  }
  
  outputfile->cd();
  std::cout << "Printing" << std::endl; outTree->Print();
  std::cout << "Writing" << std::endl; outTree->Write();
  inputfile->Close();
  outputfile->Close();  
  delete outputfile;
  delete inputfile;
}
