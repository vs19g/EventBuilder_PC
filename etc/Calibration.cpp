#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TSpectrum.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TGraph.h>
#include <iostream>
#include <fstream>
#include <TAxis.h>
#include <TH2.h>

double gains[4], offsets[4], alpha_E[3] = {5423.15, 5685.37, 6288.08};


void Calibration()
{
  ofstream mOutput;
  mOutput.open("calib/SX9B.txt");

  TH2F *unCalibADCs = new TH2F("unCalibADCs","unCalibADCs",400,250,4000,4,0,4);
  TH2F *CalibADCs = new TH2F("CalibADCs","CalibADCs",400,250,10000,4,0,4);

  //TFile *fin = TFile::Open("sorted01109.root");

  for(int i=0;i<4;i++)
    {
      gains[i] = 0; offsets[i] = 0;
      TCanvas *c1 = new TCanvas("c1","c1",1600,1200);
      char buffer[256];
      sprintf(buffer,"calib/SX9_%d.root",i);
      TFile *fin = TFile::Open(buffer);
      TH1F *h = (TH1F*)fin->FindObjectAny("hA");
      //sprintf(buffer,"hADC2DModule%d",(int)((i-i%32)/32));
      //printf("Channel %d, Module %d\n",i,(int)((i-i%32)/32));
      //TH2F *h2 = (TH2F*)fin->FindObjectAny(buffer);

      //TH1D *h = h2->ProjectionX("_px",i+1,i+1);
      h->Rebin(4);
      h->Draw();
      h->GetXaxis()->SetRangeUser(500,3000);

      for(int j=0;j<h->GetXaxis()->GetNbins();j++)
	{
	  unCalibADCs->Fill(h->GetXaxis()->GetBinCenter(j),i,h->GetBinContent(j));
	}

      TSpectrum *sp = new TSpectrum();
      sp->Search(h,2,"",0.1);
      
      if(sp->GetNPeaks()==3)
	{
	  double_t *chan = sp->GetPositionX();
	  for(int n=0;n<2;n++){//Ensure that the peaks are in the right order...
          for(int nn=n+1;nn<3;nn++){
            if(chan[nn]<chan[n]){
              float chan_tmp = chan[n];
              chan[n]=chan[nn];
              chan[nn]=chan_tmp;
            }}}
	  TGraph *g = new TGraph();
	  for(int n=0;n<3;n++){g->SetPoint(n,chan[n],alpha_E[n]);}
	  TF1 *fit = new TF1("fit","[0]+[1]*x",0,10000);
	  g->Fit(fit,"Q");
	  g->Draw("L* same");
	  offsets[i] = fit->GetParameter(0);
	  gains[i] = fit->GetParameter(1);
	}
      else if(sp->GetNPeaks()==6)
	{
	  double_t *chan = sp->GetPositionX();
	  for(int n=0;n<5;n++){
	    for(int nn=n+1;nn<6;nn++){
	      if(chan[nn]<chan[n]){
		float chan_tmp = chan[n];
		chan[n] = chan[nn];
		chan[nn] = chan_tmp;
	      }}}
	  TGraph *g = new TGraph();
	  for(int n=0;n<5;n++){g->SetPoint(n,chan[n+1],alpha_E[n]);}
	  TF1 *fit = new TF1("fit","[0]+[1]*x",0,10000);
	  g->Fit(fit,"Q");
	  g->Draw("L* same");
	  offsets[i] = fit->GetParameter(0);
	  gains[i] = fit->GetParameter(1);
	}
      mOutput << i << "\t" << offsets[i] << "\t" << gains[i] << endl;
      sprintf(buffer,"calib/FitSX9_%d.png",i);
      c1->SaveAs(buffer);

       for(int j=0;j<h->GetXaxis()->GetNbins();j++)
	{
	  CalibADCs->Fill(offsets[i]+gains[i]*h->GetXaxis()->GetBinCenter(j),i,h->GetBinContent(j));
	}
    }
  mOutput.close();
  unCalibADCs->SaveAs("calib/unCalibADCs.root");
  CalibADCs->SaveAs("calib/CalibADCs.root");
}
