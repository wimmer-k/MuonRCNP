void checkPd(int runnr,int from=-50000, int ttoo=50000){
  TFile *f = new TFile(Form("hist/fast%04d.root",runnr));
  TH2F* hentd[2];
  TH1D* htd[2];
  TH1D* hen[2];
  TH1D* hbg[2];
  TH1D* hsu[2];
  for(int g=0;g<2;g++){
    hentd[g] = (TH2F*)f->Get(Form("hGe%d_Pla0_cal_TSdiff",g));
  }
  TCanvas *c = new TCanvas("c","c",800,800);
  c->Divide(2,2);
  for(int g=0;g<2;g++){
    htd[g] = hentd[g]->ProjectionX(Form("TSdiff%d",g));
    c->cd(1);
    gPad->SetLogy();
    htd[g]->SetLineColor(1+g);
    if(g==0)
      htd[g]->DrawCopy();
    else
      htd[g]->DrawCopy("same");

    hen[g] = hentd[g]->ProjectionY(Form("peak%d",g),htd[g]->FindBin(from),htd[g]->FindBin(ttoo));
    c->cd(2);
    hen[g]->SetLineColor(1+g);
    if(g==0)
      hen[g]->DrawCopy();
    else
      hen[g]->DrawCopy("same");

    hbg[g] = hentd[g]->ProjectionY(Form("bgnd%d",g),1,htd[g]->FindBin(from));
    hbg[g]->Add(hentd[g]->ProjectionY(Form("bgnd%d",g),htd[g]->FindBin(ttoo),htd[g]->GetNbinsX()-1));
    c->cd(3);
    hbg[g]->SetLineColor(1+g);
    if(g==0)
      hbg[g]->DrawCopy();
    else
      hbg[g]->DrawCopy("same");

    hsu[g] = (TH1D*)hen[g]->Clone(Form("subt%d",g));
    hsu[g]->Add(hbg[g],-(ttoo-from)/(from-htd[g]->GetBinLowEdge(0) + htd[g]->GetBinLowEdge(htd[g]->GetNbinsX()-1)+htd[g]->GetBinWidth(1)-ttoo));
    c->cd(4);
    hsu[g]->SetLineColor(1+g);
    if(g==0)
      hsu[g]->DrawCopy();
    else
      hsu[g]->DrawCopy("same");
    
  }
  c->cd(1);
  TLine *gg[2];
  gg[0] = new TLine(from,0,from,max(htd[0]->GetMaximum(),htd[1]->GetMaximum()));
  gg[1] = new TLine(ttoo,0,ttoo,max(htd[0]->GetMaximum(),htd[1]->GetMaximum()));
  for(int g=0;g<2;g++){
    gg[g]->SetLineColor(3);
    gg[g]->Draw();
  }
  window();
  hsu[0]->DrawCopy();
  TFile *fout = new TFile("hist/savecoincs.root","UPDATE");
  hsu[0]->SetName(Form("sub_r%04d_g0",runnr));
  hsu[1]->SetName(Form("sub_r%04d_g1",runnr));
  hsu[0]->Write("",TObject::kOverwrite);
  hsu[1]->Write("",TObject::kOverwrite);
  fout->ls();
  fout->Close();
}
void compare(){
  const int nruns = 5;
  TFile *f = new TFile("hist/savecoincs.root");
  TH1F *h[nruns][2];
  int runs[nruns] = {1032,1033,1034,1035,1036};
  TCanvas *c = new TCanvas("c","c",900,500);
  c->Divide(2,1);
  for(int r=0;r<nruns;r++){
    h[r][0] = (TH1F*)f->Get(Form("sub_r%04d_g0",runs[r])); 
    h[r][1] = (TH1F*)f->Get(Form("sub_r%04d_g1",runs[r])); 
    h[r][0]->SetLineColor(1+r); 
    h[r][1]->SetLineColor(1+r); 
    c->cd(1);
    if(r==0)
      h[r][0]->Draw();
    else
      h[r][0]->Draw("same");
    c->cd(2);
    if(r==0)
      h[r][1]->Draw();
    else
      h[r][1]->Draw("same");
    
  }

}
