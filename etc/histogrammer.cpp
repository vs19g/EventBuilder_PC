{
  TFile *f = TFile::Open("run_314_gw.root");
  TTree *t = (TTree*)f->FindObjectAny("TreeData");

  for(int i=0;i<4;i++){
      char buffer[256];
      sprintf(buffer,"SXFdetnum==11&SXFnum==%d",i);
      t->Draw("Efrntup/SXBenergy:Efrntdwn/SXBenergy>>hFB(500,0,1,500,0,1)",buffer,"col");
      
      TH1F *hFB = static_cast<TH1F*>(gROOT->FindObjectAny("hFB"));
      sprintf(buffer,"calib/frontsDivB_SX11_%d.root",i);
      hFB->SaveAs(buffer);
    }
}
