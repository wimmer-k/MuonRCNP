#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include "TStyle.h"
#include "TROOT.h"

#include "TFile.h"
#include "TString.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraphErrors.h"
#include "TLine.h"
#include "TF1.h"
#include "TEnv.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TSpectrum.h"
#include "TVirtualFitter.h"
double lbinw;
Double_t gausbg(Double_t *x, Double_t *par); //fit func : gausian + background

using namespace TMath;
using namespace std;
void Calibrate_tk(char* filename, int source, double rough){
  TCanvas *c = new TCanvas("c","c",800,800);
  c->Divide(2,2);
  if(source != 0 && source != 1 && source != 2 && source != 3 && source != 4 && source != 5){
    cerr<<"source not yet implemented!"<<endl;
    exit(1);
  }
  int cores = 2;  //re-using some GRETINA script here
  int npeaks;
    if(source==0){
    cout << "152Eu" << endl;
    npeaks = 8;
  }
  if(source==1){
    cout << "60Co" << endl;
    npeaks = 2;
  }
  if(source==2){
    cout << "56Co" << endl;
    npeaks = 5;
  }
  if(source==3){
    cout << "22Na" << endl;
    npeaks = 2;
  }
  if(source==4){
    cout << "133Ba" << endl;
    npeaks = 4;
  }
  if(source==5){
    cout << "137Cs" << endl;
    npeaks = 1;
  }
  TFile* infile = new TFile(filename);
  TH2F* h2d = (TH2F*)infile->Get("hGe_raw_sum");
  TEnv* output = new TEnv("cal/cal_tk.dat");
  TFile* hfile = new TFile("cal/cal_tk.root","RECREATE");
  //TEnv* outdots = new TEnv(Form("cal/caldots_tk%d.dat",source));
  ofstream fout;
  fout.open(Form("cal/caldots_tk%d.dat",source));
  vector <double> en;
  vector <double> ene;
  vector <vector<double> > fitv;
  vector <vector<double> > fitve;
  vector <TH1F*> hcore;
  vector <vector<TF1*> > fcore;
  vector <double> cgain;
  vector <double> coffset;
  en.resize(npeaks);
  ene.resize(npeaks);
  fitv.resize(cores);
  fitve.resize(cores);
  hcore.resize(cores); //histos
  fcore.resize(cores); //fit funtions
  cgain.resize(cores); //gains
  coffset.resize(cores); //offsets
  if(source==0){ //Eu
    en[0] = 121.8; 
    en[1] = 244.7; 
    en[2] = 344.3; 
    en[3] = 778.9; 
    en[4] = 964.1; 
    en[5] = 1085.9; 
    en[6] = 1112.1; 
    en[7] = 1408.0;
    ene[0] = 0.1;
    ene[1] = 0.1;
    ene[2] = 0.1;
    ene[3] = 0.1;
    ene[4] = 0.1;
    ene[5] = 0.1;
    ene[6] = 0.1; 
    ene[7] = 0.1; 
  }
  else if(source==1){ //60Co
    en[0] = 1173.228;
    en[1] = 1332.492;
    ene[0] = 0.1;
    ene[1] = 0.1;
  }
  else if(source==2){ //56Co
    en[0] = 511.;
    en[1] = 846.77;
    en[2] = 1037.843;
    en[3] = 1238.288;
    en[4] = 1771.357;
    //en[5] = 2034.791;
    //en[6] = 2598.5;
    //en[7] = 3253.503;
    ene[0] = 0.1;
    ene[1] = 0.1;
    ene[2] = 0.1;
    ene[3] = 0.1;
    ene[4] = 0.1;
    //ene[5] = 0.1;
    //ene[6] = 0.1;
    //ene[7] = 0.1;
  }
  else if(source==3){ //Na
    en[0] = 511.;
    en[1] = 1274.537;
    ene[0] = 0.1;
    ene[0] = 0.1;
  }
  else if(source==4){ //Ba
    //en[0] = 79.6142;
    //en[0] = 80.9979;
    //en[2] = 160.612;
    //en[3] = 223.2368;
    en[0] = 276.3989;
    en[1] = 302.8508;
    en[2] = 356.0129;
    en[3] = 383.8485;
    ene[0] = 0.1;
    ene[1] = 0.1;
    ene[2] = 0.1;
    ene[3] = 0.1;
    //ene[4] = 0.1;
    // ene[5] = 0.1;
    //ene[6] = 0.1;
    //ene[7] = 0.1;    
  }
  else if(source==5){ //Cs
    en[0] = 661.657;
    ene[0] = 0.1;
  }
  for(int c=0;c<cores;c++){
    fitv[c].resize(npeaks);
    fitve[c].resize(npeaks);
    fcore[c].resize(npeaks);
    for(int i=0;i<npeaks;i++)
      fitv[c][i]=en[i]/rough;    
  }
  vector<TGraphErrors*> gcore;
  vector<TF1*> ccore;
  gcore.resize(cores);
  ccore.resize(cores);
  for(int core=0;core<cores;core++){
    cout << " core " << core <<endl;
    c->cd(1+core);
    hcore[core] = (TH1F*)h2d->ProjectionY(Form("hGe_raw_%d",core),core+1,core+1);
    //hcore[core]->Rebin(10);
    if(!hcore[core] || hcore[core]->Integral()==0){
      cgain[core] =0;
      coffset[core] =0;
      cout << "no counts in " << Form("hGe_raw_%d",core) << endl;
      //continue;
    }
    else{
      /*if(source==4){ //133Ba
	hcore[core]->GetXaxis()->SetRangeUser(50,250);
	TSpectrum *sp1 = new TSpectrum(1,1); //(num of peak , resolution[keV])
	sp->SetResolution(1);
	Int_t nfound1 = 0;
	double thresh1 = 0.5;
	nfound1 = sp1->Search(hcore[core],1,"nobackground",thresh1);
	}*/
      // else{
	hcore[core]->GetXaxis()->SetRangeUser(0,5000); //4000 was not enough for 56Co
      if(source==2)
	hcore[core]->GetXaxis()->SetRangeUser(500,5000);
      if(source==4) //133Ba
	hcore[core]->GetXaxis()->SetRangeUser(250,600);
      TSpectrum *sp = new TSpectrum(npeaks,1); //(num of peak , resolution[keV])
      sp->SetResolution(1);
      Int_t nfound = 0;
      double thresh = 0.05; //original 0.05
      if(source==1)
	thresh = 0.5; // to be changed especially for 56Co
      nfound = sp->Search(hcore[core],1,"nobackground",thresh);
      //nfound = sp->Search(hcore[core],1,"",thresh);
      
      Double_t *xpeaks = sp->GetPositionX();
      //Float_t *ypeaks = sp->GetPositionY();
      //}
      hfile->cd();
      hcore[core]->Write("",TObject::kOverwrite);
      c->cd(1+core);
      hcore[core]->Draw();
      if(nfound!=npeaks){
	cout << "Found " << nfound << " peaks in spectrum, aborting" << endl;
	continue;
      }
      //reorder peaks by energy
      vector<int> order;
      order.resize(npeaks);
      for(int i=0;i<npeaks;i++){
	order[i] = i;
      }
      int SwapCount = 0;
      for(int i=0;i<npeaks;i++){
	for(int j=0;j<(npeaks-1);j++){
	  if( xpeaks[j] > xpeaks[j+1] ){
	    double temp = xpeaks[j];
	    int itemp = order[j];
	    xpeaks[j] = xpeaks[j+1];
	    order[j] = order[j+1];
	    xpeaks[j+1] = temp;
	    order[j+1] = itemp;
	    SwapCount++;	
	  }      
	}
	
	if(SwapCount == 0)
	  break;
	else
	  SwapCount =0;
      }
      //peaks reordered
      for(int j=0;j<nfound;j++)
	cout << xpeaks[j] << "\t" << order[j] <<endl;
      cout << "Found " << nfound << " peaks in spectrum" << endl;


      //perform the peak fitting
      for(int p=0;p<npeaks;p++){
	lbinw = hcore[core]->GetBinWidth(1);
	fcore[core][p] = new TF1(Form("f_%d_%d",core,p),gausbg,xpeaks[p]-10/rough,xpeaks[p]+10/rough,5); 
	fcore[core][p]->SetLineColor(3);
	fcore[core][p]->SetLineWidth(1);
	double hint = hcore[core]->Integral(hcore[core]->FindBin(xpeaks[p]-20),hcore[core]->FindBin(xpeaks[p]+20));
	if(hint<10)
	  continue;

	//background estimate
	fcore[core][p]->SetParameter(0,(hcore[core]->GetBinContent(fitv[core][p]/hcore[core]->GetBinWidth(0)-20)+ hcore[core]->GetBinContent(fitv[core][p]/hcore[core]->GetBinWidth(0)+20) )/2 ); //bg const.
	fcore[core][p]->SetParameter(1,0); //bg slope

	fcore[core][p]->SetParameter(2,hcore[core]->Integral(xpeaks[p]-10/rough, xpeaks[p]+10/rough)); //norm

	fcore[core][p]->SetParameter(3,xpeaks[p]); //mean
	fcore[core][p]->SetParameter(4,1); //sigma ~ Ge FWHM ~ 1keV
	fcore[core][p]->SetParLimits(4,0.01,100);
	
	hcore[core]->Fit(fcore[core][p],"q0Rn");
	
	fitv[core][p]=fcore[core][p]->GetParameter(3);
	fitve[core][p]=fcore[core][p]->GetParError(3);

	
	hfile->cd();
	fcore[core][p]->Write("",TObject::kOverwrite);
      }//npeaks
      c->cd(1+core);
      hcore[core]->DrawCopy();
      for(int p=0;p<npeaks;p++){
	fcore[core][p]->SetLineColor(3);
	fcore[core][p]->Draw("same");
      }
      
      c->cd(3+core);
      ccore[core] = new TF1(Form("ccore_%d",core),"pol1",0,4000);
      ccore[core]->SetLineColor(3);
      ccore[core]->SetLineWidth(1);

      gcore[core] = new TGraphErrors(npeaks,&fitv[core][0],&en[0],&fitve[core][0],&ene[0]);
      gcore[core]->SetMarkerStyle(20);
      gcore[core]->Fit(ccore[core],"q0n");
      cgain[core] = ccore[core]->GetParameter(1);
      coffset[core] = ccore[core]->GetParameter(0);

      gcore[core]->Draw("AP");
      ccore[core]->Draw("same");
      output->SetValue(Form("Ge.%d.Gain",core),cgain[core]);
      output->SetValue(Form("Ge.%d.Offset",core),coffset[core]);

      /*for(Int_t i=0;i<npeaks;++i){
	outdots->SetValue(en[i],fcore[core][3]);
	}*/
      for(Int_t i=0;i<npeaks;++i){
	fout << en[i] << " " << fitv[core][i] << " " << fitve[core][i] << endl; 
      }
      hfile->cd();
      gcore[core]->SetName(Form("GraphCore_%d",core));
      gcore[core]->Write("",TObject::kOverwrite);
      ccore[core]->SetName(Form("CalCore_%d",core));
      ccore[core]->Write("",TObject::kOverwrite);

      
    }//core good, with data
  }//loop over cores
  output->SaveLevel(kEnvLocal);
  cout << "done with fitting cores "<< endl;
  /*
  TCanvas *ca = new TCanvas("quickcheck","quickcheck",600,300);
  ca->Divide(2,1);
  TPad *pad[2];
  for(int i=0;i<2;i++){
    pad[i] = new TPad(Form("p%d",i),Form("p%d",i),0,0,1,1);
    pad[i]->SetTopMargin(0.02);
    pad[i]->SetLeftMargin(0.12);
    pad[i]->SetBottomMargin(0.13);
    pad[i]->SetRightMargin(0.11);
  }
  TH2F *qcp = new TH2F("qcp","qcp",cores,0,cores,2000,0,2000);
  for(int c=0;c<cores;c++){
    if(hcore[c]->Integral()<1)
      continue;
    for(int e=0;e<hcore[c]->GetXaxis()->GetNbins();e++){
      if(hcore[c]->GetBinContent(e)<1)
	continue;
      //cout << "c " << c << " e " << e << endl;
      //cout << "center " << hcore[c]->GetBinCenter(e) << endl;
      //cout << "corrected " << hcore[c]->GetBinCenter(e)*cgain[c]+coffset[c] << endl;
      qcp->Fill(c,hcore[c]->GetBinCenter(e)*cgain[c]+coffset[c],hcore[c]->GetBinContent(e));
    }    
  }//cores 
  ca->cd(1);
  pad[0]->Draw();
  pad[0]->cd();
  qcp->SetTitle("");
  qcp->GetXaxis()->SetTitle("# detector");
  qcp->GetYaxis()->SetTitle("E [keV]");
  qcp->GetYaxis()->SetTitleOffset(1.5);
  qcp->Draw("colz");
  for(int l=0;l<npeaks;l++){
    TLine *li = new TLine(0,en[l],cores,en[l]);
    li->Draw();
  }
  ca->cd(2);
  pad[1]->Draw();
  pad[1]->cd();
  pad[1]->SetLogz();
  qcp->Draw("colz");


  hfile->cd();
  qcp->Write("",TObject::kOverwrite);
  */  
  hfile->Close();
    

  return;
 
}
Double_t gausbg(Double_t *x, Double_t *par){
  static Float_t sqrt2pi = TMath::Sqrt(2*TMath::Pi()), sqrt2 = TMath::Sqrt(2.);
  Double_t arg;
  /*
  par[0]   background constant
  par[1]   background slope
  par[2]   gauss0 content
  par[3]   gauss0 mean
  par[4]   gauss0 width
  */
  Double_t result = par[0] + par[1]*x[0];

  Double_t norm  = par[2];
  Double_t mean  = par[3];
  Double_t sigma = par[4];
  if(sigma==0)
    return 0;
  arg = (x[0]-mean)/(sqrt2*sigma);
  result += lbinw/(sqrt2pi*sigma) * norm * exp(-arg*arg);

  return result;
}
/*
int main(int argc, char* argv[]){
  //vector<int> range;
  char* InputFile = NULL;
  char* OutputFile = NULL;
  int HOutput = 0;
  int source =-1;
  double corea =1;
  CommandLineInterface* interface = new CommandLineInterface();

  interface->Add("-i", "inputfile", &InputFile);
  interface->Add("-o", "outputfile", &OutputFile);
  interface->Add("-s", "source: 0: 152Eu, 1: 60Co, 10 time", &source);
  interface->Add("-a", "approximate gain", &corea);
  interface->Add("-h", "also produce histogramm output file", &HOutput);
  interface->CheckFlags(argc, argv);

  if(InputFile == NULL || OutputFile == NULL ){
    cerr<<"You have to provide both the input file and the output file!"<<endl;
    exit(1);
  }
  if(source <0){
    cerr<<"You have to specify the source!"<<endl;
    exit(1);
  }
  if(HOutput == 0){
    cout<<"No Histogram file created."<<endl;
  }
  
  if(source != 0 && source != 1){
    cerr<<"source not yet implemented!"<<endl;
    exit(1);
  }
  int cores =7*4;  
  //int cores =7;  
  int npeaks;
  if(source==0){
    cout << "152Eu" << endl;
    npeaks = 8;
  }
  if(source==1){
    cout << "60Co" << endl;
    npeaks = 2;
  }
  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1); 
  TFile* File = new TFile(InputFile);
  TEnv* output = new TEnv(Form("%s.dat",OutputFile));
  TFile* HFile = NULL;
  if(HOutput)
    HFile = new TFile(Form("%s.root",OutputFile),"RECREATE");

  vector <double> en;
  vector <double> ene;
  vector <vector<double> > fitv;
  vector <vector<double> > fitve;
  vector <TH1F*> hcore;
  vector <vector<TF1*> > fcore;
  vector <double> cgain;
  vector <double> coffset;
  en.resize(npeaks);
  ene.resize(npeaks);
  fitv.resize(cores);
  fitve.resize(cores);
  hcore.resize(cores); //histos
  fcore.resize(cores); //fit funtions
  cgain.resize(cores); //gains
  coffset.resize(cores); //offsets
  if(source==0){
   en[0] = 121.8; 
   en[1] = 244.7; 
   en[2] = 344.3; 
   en[3] = 778.9; 
   en[4] = 964.1; 
   en[5] = 1085.9; 
   en[6] = 1112.1; 
   en[7] = 1408.0;
   ene[0] = 0.1;
   ene[1] = 0.1;
   ene[2] = 0.1;
   ene[3] = 0.1;
   ene[4] = 0.1;
   ene[5] = 0.1;
   ene[6] = 0.1; 
   ene[7] = 0.1; 
  }
//   if(source==0){
//    en[0] = 121.8; 
//    en[1] = 244.7; 
//    en[2] = 344.3; 
//    en[3] = 778.9; 
//    en[4] = 867.4; 
//    en[5] = 964.1; 
//    en[6] = 1085.9; 
//    en[7] = 1112.1; 
//    en[8] = 1408.0;
//    ene[0] = 0.1;
//    ene[1] = 0.1;
//    ene[2] = 0.1;
//    ene[3] = 0.1;
//    ene[4] = 0.1;
//    ene[5] = 0.1;
//    ene[6] = 0.1; 
//    ene[7] = 0.1; 
//    ene[8] = 0.1;
//   }
  else if(source==1){
    en[0] = 1173.228;
    en[1] = 1332.492;
    ene[0] = 0.1;
    ene[1] = 0.1;
  }
  for(int c=0;c<cores;c++){
    fitv[c].resize(npeaks);
    fitve[c].resize(npeaks);
    fcore[c].resize(npeaks);
    for(int i=0;i<npeaks;i++)
      fitv[c][i]=en[i]/corea;    
  }

  
  TGraphErrors* gcore;
  TF1* ccore;
  TCanvas* spectra = new TCanvas("spectra","spectra",600,300);
  spectra->Divide(2,1);
  ccore = new TF1("ccore","pol1",0,1.2e6);
  ccore->SetLineColor(3);
  ccore->SetLineWidth(1);
  for(int core=0;core<cores;core++){
    cout << " core " << core << " detector " << core/4 << "\tcrystal "<< core%4 << endl;
    hcore[core] = (TH1F*) File->Get(Form("hcc_d%d_c%d",core/4,core%4));
    hcore[core]->Rebin(10);
    if(hcore[core]->Integral()==0){
      cgain[core] =0;
      coffset[core] =0;
      //cout << "no counts in " << Name << endl;
      //continue;
    }
    else{
      hcore[core]->GetXaxis()->SetRangeUser(50000,1.2e6);
      TSpectrum *sp = new TSpectrum(8,5);
      sp->SetResolution(5);
      Int_t nfound = 0;
      double thresh = 0.05;
      if(core == 20)
	thresh = 0.09;
      nfound = sp->Search(hcore[core],5,"nobackground",thresh);
      
      Float_t *xpeaks = sp->GetPositionX();
      //Float_t *ypeaks = sp->GetPositionY();
      if(HOutput){
	HFile->cd();
	hcore[core]->Write("",TObject::kOverwrite);
      }
      if(nfound!=npeaks){
	cout << "Found " << nfound << " peaks in spectrum, aborting" << endl;
	continue;
      }
      vector<int> order;
      order.resize(npeaks);
      for(int i=0;i<npeaks;i++){
	order[i] = i;
      }
      int SwapCount = 0;
      for(int i=0;i<npeaks;i++){
	for(int j=0;j<(npeaks-1);j++){
	  if( xpeaks[j] > xpeaks[j+1] ){
	    double temp = xpeaks[j];
	    int itemp = order[j];
	    xpeaks[j] = xpeaks[j+1];
	    order[j] = order[j+1];
	    xpeaks[j+1] = temp;
	    order[j+1] = itemp;
	    SwapCount++;	
	  }      
	}
	
	if(SwapCount == 0)
	  break;
	else
	  SwapCount = 0;
      }
//       for(int j=0;j<nfound;j++)
// 	cout << xpeaks[j] << "\t" << order[j] <<endl;
//       cout << "Found " << nfound << " peaks in spectrum" << endl;
      
      for(int p=0;p<npeaks;p++){
	hcore[core]->GetXaxis()->SetRangeUser(xpeaks[p]-50/corea, xpeaks[p]+50/corea );
	//cout << "range " << xpeaks[p]-30/corea <<"\t"<< xpeaks[p]+30/corea << endl;

	fcore[core][p] = new TF1(Form("f_%d_%d",core,p),fgammagausbg,xpeaks[p]-10/corea,xpeaks[p]+10/corea,5);
	fcore[core][p]->SetLineColor(3);
	fcore[core][p]->SetLineWidth(1);

	fcore[core][p]->SetParameter(0,(hcore[core]->GetBinContent(fitv[core][p]/hcore[core]->GetBinWidth(0)-20)+ hcore[core]->GetBinContent(fitv[core][p]/hcore[core]->GetBinWidth(0)+20) )/2 );
	fcore[core][p]->SetParameter(1,0);
	//cout << "Stting range" << fitv[core][p]-20<<"\t"<< fitv[core][p]+20 <<endl;
	//cout << p<<" \t" << hcore[core]->GetMaximum() << endl;
	fcore[core][p]->SetParameter(2,hcore[core]->Integral(xpeaks[p]-10/corea, xpeaks[p]+10/corea));
	//fcore[core][p]->SetParLimits(2,hcore[core]->Integral(xpeaks[p]-10/corea, xpeaks[p]+10/corea)*0.1, hcore[core]->Integral(xpeaks[p]-10/corea, xpeaks[p]+10/corea)*10);
	fcore[core][p]->SetParameter(3,xpeaks[p]);
	fcore[core][p]->SetParameter(4,1000);
	//fcore[core][p]->SetParLimits(2,hcore[core]->GetMaximum()*2.5*0.3,hcore[core]->GetMaximum()*2.5*3);
	//fcore[core][p]->SetParLimits(4,100,10000);
	
	hcore[core]->Fit(fcore[core][p],"q0R");
	
	//cout << " limits " << hcore[core]->GetMaximum()*2.5*0.3 <<"\t"<<hcore[core]->GetMaximum()*2.5*3<<endl;
	//cout << "result " << fcore[core][p]->GetParameter(2) << endl;
	fitv[core][p]=fcore[core][p]->GetParameter(3);
	fitve[core][p]=fcore[core][p]->GetParError(3);

	if(HOutput){
	  HFile->cd();
	  fcore[core][p]->Write("",TObject::kOverwrite);
	}
      }//npeaks
      spectra->cd(1);
      hcore[core]->GetXaxis()->SetRangeUser(0,1.2e6);//SetRangeUser(1,corea*en[npeaks-1]+20);
      hcore[core]->Draw();
      for(int p=0;p<npeaks;p++)
	fcore[core][p]->Draw("same");

      gcore = new TGraphErrors(npeaks,&fitv[core][0],&en[0],&fitve[core][0],&ene[0]);
      gcore->SetMarkerStyle(20);
      gcore->Fit(ccore,"q0");
      cgain[core] = ccore->GetParameter(1);
      coffset[core] = ccore->GetParameter(0);

      spectra->cd(2);
      gcore->Draw("AP");
      ccore->Draw("same");
      output->SetValue(Form("Slope.d%d.c%d",core/4,core%4),cgain[core]);
      output->SetValue(Form("Offset.d%d.c%d",core/4,core%4),coffset[core]);


      if(HOutput){
	HFile->cd();
	gcore->SetName(Form("GraphCore_%d",core));
	gcore->Write("",TObject::kOverwrite);
	ccore->SetName(Form("CalCore_%d",core));
	ccore->Write("",TObject::kOverwrite);
      }	
       
    }//coutns in histo
    if(core==0)
      spectra->SaveAs(Form("%s.ps(",OutputFile));
    else
      spectra->SaveAs(Form("%s.ps",OutputFile));
  }//loop over cores


  
  output->SaveLevel(kEnvLocal);
  cout << "done with fitting cores "<< endl;
  TCanvas *ca = new TCanvas("quickcheck","quickcheck",600,300);
  ca->Divide(2,1);
  TPad *pad[2];
  for(int i=0;i<2;i++){
    pad[i] = new TPad(Form("p%d",i),Form("p%d",i),0,0,1,1);
    pad[i]->SetTopMargin(0.02);
    pad[i]->SetLeftMargin(0.12);
    pad[i]->SetBottomMargin(0.13);
    pad[i]->SetRightMargin(0.11);
  }
  TH2F *qcp = new TH2F("qcp","qcp",cores,0,cores,2000,0,2000);
  for(int c=0;c<cores;c++){
    if(hcore[c]->Integral()<1)
      continue;
    for(int e=0;e<hcore[c]->GetXaxis()->GetNbins();e++){
      if(hcore[c]->GetBinContent(e)<1)
	continue;
      //cout << "c " << c << " e " << e << endl;
      //cout << "center " << hcore[c]->GetBinCenter(e) << endl;
      //cout << "corrected " << hcore[c]->GetBinCenter(e)*cgain[c]+coffset[c] << endl;
      qcp->Fill(c,hcore[c]->GetBinCenter(e)*cgain[c]+coffset[c],hcore[c]->GetBinContent(e));
    }    
  }//cores
  ca->cd(1);
  pad[0]->Draw();
  pad[0]->cd();
  qcp->SetTitle("");
  qcp->GetXaxis()->SetTitle("# detector");
  qcp->GetYaxis()->SetTitle("E [keV]");
  qcp->GetYaxis()->SetTitleOffset(1.5);
  qcp->Draw("colz");
  for(int l=0;l<npeaks;l++){
    TLine *li = new TLine(0,en[l],cores,en[l]);
    li->Draw();
  }
  ca->cd(2);
  pad[1]->Draw();
  pad[1]->cd();
  pad[1]->SetLogz();
  qcp->Draw("colz");
  ca->SaveAs(Form("%s.ps)",OutputFile));
  if(HOutput && HFile){
    HFile->cd();
    qcp->Write("",TObject::kOverwrite);
    HFile->Close();
  }
  
  

  return 0;

}
*/
