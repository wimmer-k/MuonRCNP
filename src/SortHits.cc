#include "SortHits.hh"
#include "stdio.h"
#include "string.h"

SortHits::SortHits(){
  fevtnr = 0;
  ffragnr = 0;
  fdiscarded = 0;
  fmemdepth = 200;
  frootfile = NULL;
  fwindow = 2000;
  flastTS = 0;
}
SortHits::SortHits(TFile *out){
  SetRootFile(out);
  fevtnr = 0;
  ffragnr = 0;
  fdiscarded = 0;
  fmemdepth = 200;
  fwindow = 2000;
  flastTS = 0;
}
void SortHits::SetRootFile(TFile *out){
  frootfile = out;
  out->cd();
  ftr = new TTree("tr","events");
  ftr->SetDirectory(out);
  fevent = new Event();
  ftr->Branch("event",&fevent,32000000);
  ftr->SetAutoSave(-300000000);//300 MB
  ftr->SetAutoFlush(-30000000);//30 MB
  
  fhTSdiff = new TH1F("hTSdiff","hTSdiff",1000,0,100000);
  fhfragTS = new TH2F("hfragTS","hfragTS",1000,0,1e6,1000,0,1e12);
  fheventTS = new TH2F("heventTS","heventTS",1000,0,1e6,1000,0,1e12);
  fhTSdiff_PHAWave = new TH1F("hTSdiff_PHAWave","hTSdiff_PHAWave",1000,0,100000);
}
bool SortHits::Add(Fragment* addme){
  fhfragTS->Fill(ffragnr,addme->GetTS());
  ffragnr++;
  //PrintList();
  
  if(flist.empty()){
    //list is empty, add new fragment
    flist.push_back(addme);
    return true;
  }
  else if(ffragnr > fmemdepth){
    //add to a full list, remove the oldest fragment first
    return FullList(addme);
  }
  else{
    //insert fragment into list
    return Insert(addme);
  }
}
bool SortHits::FullList(Fragment* addme){
  list<Fragment*>::iterator it = flist.begin();

  if((*it)->GetTS() > addme->GetTS()){
    fdiscarded++;
    cout << __PRETTY_FUNCTION__ << " problem" << endl;
    cout << "current fragment: ";
    addme->Print();
    cout << "oldest fragment: ";
    (*it)->Print();
    cout << "discarding this fragment, you should increase memory depth" << endl;
    return false;
  }
  //write out oldest fragment on the list
  WriteFragment(*it);
  //remove it from list
  it = flist.erase(it);
  return Insert(addme);
}
bool SortHits::Insert(Fragment* insertme){
  if(fvl>1){
    cout << __PRETTY_FUNCTION__ << endl;
    cout << "inserted TS = " << insertme->GetTS() << endl;
  }
  list<Fragment*>::iterator it = flist.end();
  it--;
  while(it != flist.begin() && (*it)->GetTS() >= insertme->GetTS()){
    it--;
  }
  if(it != flist.begin())
    it++;
  flist.insert(it,insertme);
  if(fvl>1)
    PrintList();
  return true;
}
void SortHits::PrintList(){
  cout <<"-------list---------------------" << endl;
  list<Fragment*>::iterator it = flist.begin();
  while(it != flist.end()){
    cout << "list " << (*it)->GetTS() << endl;
    it++;
  }
}
void SortHits::WriteFragment(Fragment* writeme){
  if(fvl>1){
    cout << __PRETTY_FUNCTION__ << endl;
    cout << "writeme TS = " << writeme->GetTS() << " last one in the event " << fevent->GetLastTS() << endl;
  }
  fhTSdiff->Fill(writeme->GetTS() - fevent->GetLastTS());
  if(writeme->GetID()==1 && fevent->GetLastWaveTS()>0)
    fhTSdiff_PHAWave->Fill(writeme->GetTS() - fevent->GetLastWaveTS());
  if(writeme->GetID()==0 && fevent->GetLastPHATS()>0)
    fhTSdiff_PHAWave->Fill(fevent->GetLastPHATS() - writeme->GetTS());
    
  if(fevent->GetLastTS() <1){
    if(fvl>1)
      cout << "first fragment in the event"<<endl;
    //first fragment
    fevent->Add(writeme);
  }
  // check timestamp difference to fevent
  else if(writeme->GetTS() - fevent->GetLastTS() < fwindow){
    if(fvl>1)
      cout << "inside the window" << endl;
    //inside the event building window, add fragment to event
    fevent->Add(writeme);
  }
  else{
    //close fevent and write
    if(fvl>1)
      cout << "outside the window" << endl;
    CloseEvent();
    fevent->Add(writeme);
  }
}

void SortHits::CloseEvent(){
  if(fvl>1){
    cout << __PRETTY_FUNCTION__ << endl;
    fevent->PrintEvent();
  }
  fheventTS->Fill(fevtnr,fevent->GetLastTS());
  flastTS = fevent->GetLastTS();
  frootfile->cd();
  ftr->Fill();
  fevent->Clear();
  fevtnr++;
}
void SortHits::Flush(){
  if(fvl>1)
    cout << __PRETTY_FUNCTION__ << endl;
  list<Fragment*>::iterator it = flist.begin();
  int ctr = 0;
  if(fvl>1)
    PrintList();
  while(it != flist.end()){
    if(fvl>1)
      cout << "erased " << ctr << " times, TS to be deleted " << (*it)->GetTS() << " \t" << (*it)->GetBoard() << "\t "<< (*it)->GetCh() << endl;
    WriteFragment(*it);
    it = flist.erase(it);
    ctr++;
  }
  CloseEvent();
  Status();
  if(frootfile){
    cout << "writing tree to root file" << endl;
    frootfile->cd();
    ftr->Write("",TObject::kOverwrite);
    fhTSdiff->Write();
    fhTSdiff_PHAWave->Write();
    fhfragTS->Write();
    fheventTS->Write();
  }
}
void SortHits::Status(){
  cout << "\nStatus of SortHits:" << endl;
  cout << "Event memory depth: " << fmemdepth << endl;
  cout << "Fragments processed:   " << ffragnr << endl;
  cout << "Events built:   " << fevtnr << endl;
  if(fdiscarded)
    cout << "Fragments discarded:   " << fdiscarded << "  (increase mem depth!)"<< endl;
}
