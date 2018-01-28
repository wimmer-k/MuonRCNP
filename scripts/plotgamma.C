{
  TFile *f = new TFile("hist/run0162.root");
  TH1F* hen[2];
  TH1F* htd[4][2];
  TH1F* heg[4][2];
  for(int g=0;g<2;g++){
    hen[g] = (TH1F*)f->Get(Form("hGe_cal_%d",g));
    hen[g]->GetXaxis()->SetRangeUser(600,1000);
    for(int p=0;p<4;p++){
      htd[p][g] = (TH1F*)f->Get(Form("hGe%d_Pla%d_TSdiff",g,p));
      heg[p][g] = (TH1F*)f->Get(Form("hGe%d_gatePla%d",g,p));
      heg[p][g]->GetXaxis()->SetRangeUser(600,1000);
    }
  }
  TCanvas *c[2];
  for(int k=0;k<2;k++){
    c[k] = new TCanvas(Form("c%d",k),Form("c%d",k),800,800);
  }
  c[0]->Divide(2,2);
  c[1]->Divide(1,3);
  for(int p=0;p<4;p++){
    c[0]->cd(1+p);
    gPad->SetLogy();
    htd[p][0]->Draw();
    htd[p][1]->Draw("same");
    htd[p][1]->SetLineColor(2);
  }
  c[1]->cd(1);
  //gPad->SetLogy();
  hen[0]->Draw();
  hen[1]->Draw("same");
  hen[1]->SetLineColor(2);
  c[1]->cd(2);
  //gPad->SetLogy();
  heg[1][0]->Draw();
  heg[1][1]->Draw("same");
  heg[1][1]->SetLineColor(2);
  c[1]->cd(3);
  //gPad->SetLogy();
  heg[3][0]->Draw();
  heg[3][1]->Draw("same");
  heg[3][1]->SetLineColor(2);
    
  
}
