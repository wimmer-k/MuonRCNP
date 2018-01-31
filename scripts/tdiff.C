void tdiff(int run){
  TFile* f[2];
  f[0] = new TFile(Form("root/run%04d.root",run));
  f[1] = new TFile(Form("root/fast%04d.root",run));

  TH1F* h[2];
  TCanvas *c = new TCanvas("c","c",600,800);
  c->Divide(1,2);
  for(int i=0;i<2;i++){
    h[i] = (TH1F*)f[i]->Get("hTSdiff");
    c->cd(1+i);
    h[i]->Draw();
    gPad->SetLogy();
  }
}
void tdiffs(){
  TFile* f[3];
  int runs[3] = {1024,1025,1026};
  TH1F* h[3];
  THStack* hs = new THStack("hs","hs");
  for(int i=0;i<3;i++){
    f[i] = new TFile(Form("root/fast%04d.root",runs[i]));
    h[i] = (TH1F*)f[i]->Get("hTSdiff");
    h[i]->SetLineColor(2+i);
    hs->Add(h[i]);
  }
  gPad->SetLogy();
  hs->Draw("nostack");
}
void tsread(int run){
  TFile* f = new TFile(Form("root/fast%04d.root",run));
  TCanvas *c = new TCanvas("c","c",600,800);
  c->Divide(1,2);
  TH2F* h[2][3];
  h[0][0] = (TH2F*)f->Get("debugfragTSWave");
  h[0][1] = (TH2F*)f->Get("debugfragTSPHA_0");
  h[0][2] = (TH2F*)f->Get("debugfragTSPHA_1");
  c->cd(1);
  h[0][0]->Draw();
  h[1][0] = (TH2F*)f->Get("debugevntTSWave");
  h[1][1] = (TH2F*)f->Get("debugevntTSPHA_0");
  h[1][2] = (TH2F*)f->Get("debugevntTSPHA_1");
  c->cd(2);
  h[1][0]->Draw();
  for(int i=0;i<3;i++){
    for(int j=0;j<2;j++){
      c->cd(1+j);
      h[j][i]->SetMarkerColor(2+i);
      h[j][i]->Draw("same");
    }
  }
}
