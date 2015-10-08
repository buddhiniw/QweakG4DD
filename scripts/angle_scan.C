/*
A script to draw the pe distributions on the PMTs.
08-31-2015  Buddhini Waidyawansa
10-05-2015  Buddhini - In the tree draw, selected MD 3 data from the QweakSimUserMainEvent.Cerenkov.PMT.PMTLeftNbOfPEs array as QweakSimUserMainEvent.Cerenkov.PMT.PMTLeftNbOfPEs[3].
                       Added the folder stem "new_" to use the new simulation results with the updated angle and position ranges.
		       Modified the code to enable drawing of either "xl" (along the height of the bar) or "yl" (along the length of the bar)  scans and selecting different data folder extensions such as "glue" or "new". NOTE. 


*/
#include <stdio.h>
#include <TMath.h>

double*  get_no_pe(TString energy, TString angle, TString pos);
double*  get_pe_asym(TString energy, TString angle, TString pos);
void draw_angle_scans_at_fixed_pos(int X);
void draw_angle_scans_at_fixed_E(int E);
void draw_position_scans_at_fixed_theta(int T);



const int npos = 21;
const int nbeame = 4;
const int nangle = 25;

Double_t Pos[npos]={-90,-80,-60,-40,-20,-10,-8,-6,-4,-2,0,2,4,6,8,10,20,40,60,80,90}; //cm
//Double_t Pos[npos]={-4,-2,0,2,4}; //cm	      
	      
Double_t beamE[nbeame]={5,10,50,100}; //MeV
Double_t Ang[nangle]={-80,-60,-40,-35,-30,-25,-20,-15,-10,-7.5,-5,-2.5,0,2.5,5,7.5,10,15,20,25,30,35,40,60,80};
//Double_t Ang[nangle]={-2.5,0,2.5};

double data[npos][nbeame][nangle];
double dataerr[npos][nbeame][nangle];
double peL[npos][nbeame][nangle];
double peLE[npos][nbeame][nangle];
double peR[npos][nbeame][nangle];
double peRE[npos][nbeame][nangle];



Double_t AngE[nangle];
Double_t PosE[npos];

TString fstem;
TString cstem;
TString posl;
TString angl;

void angle_scan(TString file, TString coordinate){

  gDirectory->Delete("*");

  // Fit and stat parameters
  gStyle->SetOptFit(0);
  gStyle->SetOptStat(1);
  gStyle->SetStatW(0.20);
  gStyle->SetStatH(0.07);
  gStyle->SetStatColor(0);
  gStyle->SetStatFont(132);  
  gStyle->SetStatFormat("4.2f");
  gStyle->SetFitFormat("4.2f");
  gStyle->SetStatFontSize(0.09);  

  //Pad parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadTopMargin(0.15);
  gStyle->SetPadBottomMargin(0.18);
  gStyle->SetPadRightMargin(0.06);
  gStyle->SetPadLeftMargin(0.1);

  
  // histo parameters
  gStyle->SetOptTitle(1);
  gStyle->SetTitleX(0.45);
  gStyle->SetTitleY(0.99);
  gStyle->SetTitleFontSize(0.08);

  gStyle->SetTitleYOffset(1.0);
  gStyle->SetTitleXOffset(1.0);
  gStyle->SetLabelSize(0.05,"x");
  gStyle->SetLabelSize(0.05,"y");
  gStyle->SetTitleSize(0.05,"x");
  gStyle->SetTitleSize(0.05,"y");
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFont(132,"x");
  gStyle->SetTitleFont(132,"y");
  gStyle->SetTitleFont(132,"1");
  gStyle->SetLabelFont(132,"x");
  gStyle->SetLabelFont(132,"y");
  
  //error vectors
  for(int i=0;i<nangle;i++) AngE[i]=0.0;
  for(int i=0;i<npos;i++) PosE[i]=0.0;

  fstem = file;
  cstem = coordinate;

  if(cstem.Contains("yl")){
    posl = "Y_{L} (cm)";
    angl="Y_{L}^{'} (deg)";
  }
  if(cstem.Contains("xl")){
    posl = "X_{L} (cm)";
    angl="X_{L}^{'} (deg)";
  }


  double *p;
  double *r;

  TString energy;
  TString angle;
  TString position;  

  FILE *fp;
  fp=fopen(Form("%s_md3_angle_scan.txt",cstem.Data()), "w");
  if(cstem.Contains("yl"))
    fprintf(fp, "Y_L(cm),\tE(MeV),\tYTheta_L(deg),\t A,\t dA,\t peL,\t dpeL,\t peR,\t dpeR\n");
  if(cstem.Contains("xl"))
    fprintf(fp, "X_L(cm),\tE(MeV),\tXTheta_L(deg),\t A,\t dA,\t peL,\t dpeL,\t peR,\t dpeR\n");

  for(Int_t X=0;X<npos;X++){
    // for each position
    position = Form("%06.2f",Pos[X]);
    //std::cout<<" shift in position (cm) ="<<position<<std::endl;
    
    for(int E=0;E<nbeame;E++){
      //For each beam energy
      energy = Form("%04.0f",beamE[E]);
      //std::cout<<" energy (MeV) ="<<energy<<std::endl;
      
     
      for(Int_t T=0;T<nangle;T++){
	//for each angle
     
	angle = Form("%06.2f",Ang[T]);
	std::cout<<"\n### angle = "<<Ang[T]<<" deg "<<std::endl;

	// get the left right pmt asymmetry
      	p =  get_pe_asym(energy, Form("%06.2f",Ang[T]), position);
	for ( int i = 0; i < 2; i++ ){
	  printf( "*(p + %d) : %f\n", i, *(p + i));
	}
	//store the data in a 3-d array for later use
	data[X][E][T] = *(p);
	dataerr[X][E][T] = *(p+1);
	std::cout<<"\n##################################"<<std::endl;
	std::cout<<"asym at position = "<<Pos[X]
		 <<" cm; E="<<beamE[E]
		 <<" MeV; #theta ="<<Ang[T]<<" is "
		 <<data[X][E][T]<<"+-"<<dataerr[X][E][T]<<std::endl;

	// get the total number of pe in each tube
      	r =  get_no_pe(energy, Form("%06.2f",Ang[T]), position);
	for ( int i = 0; i < 4; i++ ){
	  printf( "*(r + %d) : %f\n", i, *(r + i));
	}
	peL[X][E][T] = *(r);
	peLE[X][E][T] = *(r+1);
	peR[X][E][T] = *(r+2);
	peRE[X][E][T] = *(r+3);
	std::cout<<"\n##################################"<<std::endl;
	std::cout<<"pe at position = "<<Pos[X]
		 <<" cm; E="<<beamE[E]
		 <<" MeV; #theta ="<<Ang[T]
		 <<"; Left =  "<<peL[X][E][T]<<"+-"<<peLE[X][E][T]
		 <<"; Right =  "<<peR[X][E][T]<<"+-"<<peRE[X][E][T]
		 <<std::endl;



       	fprintf(fp, "%3.2f,\t%3.2f,\t%3.2f,\t%3.4f,\t%3.4f,\t%3.2f,\t%3.2f,\t%3.2f,\t%3.2f\n",Pos[X],beamE[E],Ang[T],data[X][E][T],dataerr[X][E][T],peL[X][E][T],peLE[X][E][T],peR[X][E][T],peRE[X][E][T]);

      }
    }
  }

 
  //////////////////////////////
  // Angle scans at different Es and at fixed X
  //////////////////////////////
  for(int j=0;j<npos;j++) draw_angle_scans_at_fixed_pos(j); 
 
 
  //////////////////////////////
  // Angle scans at different X positions along the bar at a fixed energy
  //////////////////////////////
  for(j=0;j<nbeame;j++) draw_angle_scans_at_fixed_E(j); 


  //////////////////////////////
  // Position scans at a fixed energy
  //////////////////////////////
  draw_position_scans_at_fixed_theta(12);


  fclose(fp);
  std::cout<<"Done!"<<std::endl;



}

void draw_angle_scans_at_fixed_pos(int X){
 
  TString title;
  if(cstem.Contains("yl"))
    title = Form("pe asymmetry vs electron entrance angle on MD3 bare bar [e Gun @ Y_{L}=%2.0fcm, Y_{G}=335cm, Z_{G}=575cm]",Pos[X]);

  if(cstem.Contains("xl"))
    title = Form("pe asymmetry vs electron entrance angle on MD3 bare bar [e Gun @ X_{L}=%2.0fcm, X_{G}=xxxcm, Z_{G}=575cm]",Pos[X]);
  
  TGraphErrors *gE[nbeame];
  TMultiGraph * mgE = new TMultiGraph("",title);
  TLegend * lege = new TLegend(0.1,0.7,0.48,0.9);
  lege-> SetNColumns(3);

  TGraphErrors *gPL[nbeame];
  TGraphErrors *gPR[nbeame];
  TMultiGraph * mgP;
  if(cstem.Contains("yl")) mgP = new TMultiGraph("",Form("pe count vs electron entrance angle on MD3 bare bar [e Gun @ Y_{L}=%2.0fcm, Y_{G}=335cm, Z_{G}=575cm]",Pos[X]));
  if(cstem.Contains("xl")) mgP = new TMultiGraph("",Form("pe count vs electron entrance angle on MD3 bare bar [e Gun @ X_{L}=%2.0fcm, Y_{G}=335cm, Z_{G}=575cm]",Pos[X]));


  //TMultiGraph * mgP = new TMultiGraph("",Form("pe's vs electron entrance angle on MD3 bare bar [e Gun @ Y_{L}=%2.0fcm, Y_{G}=335cm, Z_{G}=575cm]",Pos[X]));
  TLegend * legP = new TLegend(0.1,0.7,0.48,0.9);
  legP-> SetNColumns(3);


  TCanvas * C = new TCanvas("c", "",0,0,1500,800);
  C->Draw();
  for(int E=0;E<nbeame;E++){
    gE[E] = new TGraphErrors(nangle,Ang,(Double_t*)data[X][E],AngE,(Double_t*)dataerr[X][E]);
    gE[E]->SetMarkerColor(E+1);
    gE[E]->SetLineColor(E+1);
    gE[E]->SetMarkerStyle(21);
    mgE->Add(gE[E]);
    lege->AddEntry(gE[E],Form("E = %3.0f MeV",beamE[E]),"lep");

    gPL[E] = new TGraphErrors(nangle,Ang,(Double_t*)peL[X][E],AngE,(Double_t*)peLE[X][E]);
    gPL[E]->SetMarkerColor(E+1);
    gPL[E]->SetLineColor(E+1);
    gPL[E]->SetMarkerStyle(20);
    mgP->Add(gPL[E]);
    legP->AddEntry(gPL[E],Form("LEFT : E = %3.0f MeV",beamE[E]),"lep");
    gPR[E] = new TGraphErrors(nangle,Ang,(Double_t*)peR[X][E],AngE,(Double_t*)peRE[X][E]);
    gPR[E]->SetMarkerColor(E+1);
    gPR[E]->SetLineColor(E+1);
    gPR[E]->SetMarkerStyle(24);
    mgP->Add(gPR[E]);
    legP->AddEntry(gPR[E],Form("RIGHT : E = %3.0f MeV",beamE[E]),"lep");
 
  }

  // Draw asymmetry
  mgE->Draw("AECP");
  mgE->GetXaxis()->SetTitle(angl);
  mgE->GetYaxis()->SetTitle("Left/Right pe asymmetry");
  lege->Draw();
  gPad->SetGridx();
  gPad->SetGridy();

  C->Print(Form("E_angle_scans_asym_at_%s_%3.2f.png",cstem.Data(),Pos[X]));
  C->Print(Form("E_angle_scans_asym_at_%s_%3.2f.C",cstem.Data(),Pos[X]));

  // Draw total pe
  C->Clear();
  mgP->Draw("AECP");
  mgP->GetXaxis()->SetTitle(angl);
  mgP->GetYaxis()->SetTitle("Number of pe's");
  mgP->SetTitle(Form("Angle scan for %s=%3.2f cm",posl.Data(),Pos[X]));
  legP->Draw();
  gPad->SetGridx();
  gPad->SetGridy();

  C->Print(Form("E_angle_scans_pe_at_%s_%3.2f.png",cstem.Data(),Pos[X]));
  C->Print(Form("E_angle_scans_pe_at_%s_%3.2f.C",cstem.Data(),Pos[X]));


}

void draw_angle_scans_at_fixed_E(int E){
 
  TGraphErrors *gE[npos];


  TMultiGraph * mgE = new TMultiGraph("",Form("pe asymmetry vs electron entrance angle on MD3 bare bar [e Gun E = %2.0f MeV]",beamE[E]));
  TLegend * lege = new TLegend(0.1,0.7,0.48,0.9);
  lege-> SetNColumns(3);

  TGraphErrors *gPL[npos];
  TGraphErrors *gPR[npos];
  TMultiGraph * mgP = new TMultiGraph("",Form("pe's vs e entrance angle on MD3 bare bar [e Gun E = %2.0f MeV]",beamE[E]));
  TLegend * legP = new TLegend(0.1,0.7,0.48,0.9);
  legP-> SetNColumns(3);

  TCanvas * C = new TCanvas("c","",0,0,1500,800);
  C->Draw();

  Double_t d[nangle];
  Double_t de[nangle];
  Double_t dl[nangle];
  Double_t dle[nangle];
  Double_t dr[nangle];
  Double_t dre[nangle];

  for(int i=0;i<npos;i++){
    // std::cout<<" energy = "<<beamE[E]<<std::endl;
    for(Int_t T=0;T<nangle;T++){	
      d[T]=data[i][E][T];
      de[T]=dataerr[i][E][T];
      dl[T]=peL[i][E][T];
      dle[T]=peLE[i][E][T];
      dr[T]=peR[i][E][T];
      dre[T]=peRE[i][E][T];
    }

    gE[i] = new TGraphErrors(nangle,Ang,d,AngE,de);
    gE[i]->SetMarkerColor(i+30);
    gE[i]->SetMarkerStyle(21);
    gE[i]->SetLineColor(i+30);
    mgE->Add(gE[i]);
    lege->AddEntry(gE[i],Form("%s = %3.0f cm",posl.Data(),Pos[i]),"lep");
    
    gPL[i] = new TGraphErrors(nangle,Ang,dl,AngE,dle);
    gPL[i]->SetMarkerColor(i+30);
    gPL[i]->SetLineColor(i+30);
    gPL[i]->SetMarkerStyle(20);
    mgP->Add(gPL[i]);

    legP->AddEntry(gPL[i],Form("LEFT : %s = %3.0f cm",posl.Data(),Pos[i]),"lep");
    gPR[i] = new TGraphErrors(nangle,Ang,dr,AngE,dre);
    gPR[i]->SetMarkerColor(i+30);
    gPR[i]->SetLineColor(i+30);
    gPR[i]->SetMarkerStyle(24);
    mgP->Add(gPR[i]);
    legP->AddEntry(gPR[i],Form("RIGHT : %s = %3.0f cm",posl.Data(),Pos[i]),"lep");
 

  }

  // Draw asymmetry
  mgE->Draw("AECP");
  mgE->GetXaxis()->SetTitle(angl);
  mgE->GetYaxis()->SetTitle("Left/Right pe asymmetry");
  lege->Draw();
  gPad->SetGridx();
  gPad->SetGridy();
  
  C->Print(Form("%s_angle_scans_asym_at_E_%3.2f.png",cstem.Data(),beamE[E]));
  C->Print(Form("%s_angle_scans_asym_at_E_%3.2f.C",cstem.Data(),beamE[E]));
  
  // Draw pe
  C->Clear();
  mgP->Draw("AECP");
  mgP->GetXaxis()->SetTitle(angl);
  mgP->GetYaxis()->SetTitle("Number of pe's");
  mgP->SetTitle(Form("Angle scan for E=%3.0f MeV",beamE[E]));
  legP->Draw();
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetLogy();



  C->Print(Form("%s_angle_scans_pe_at_E_%3.2f.png",cstem.Data(),beamE[E]));
  C->Print(Form("%s_angle_scans_pe_at_E_%3.2f.C",cstem.Data(),beamE[E]));

}


void draw_position_scans_at_fixed_theta(int T){

  TString title;
  if(cstem.Contains("yl"))
    title = "Position dependance of pe count along the length of the MD3 bare bar [e Gun @ Y_{G}=335cm, Z_{G}=575cm]";
  
  if(cstem.Contains("xl"))
    title ="Position dependance of pe count along the width of the MD3 bare bar [e Gun @ X_{G}=xxxcm, Z_{G}=575cm]";


  TMultiGraph * mgP = new TMultiGraph("",title);
  TLegend * lege = new TLegend(0.1,0.7,0.48,0.9);
  lege-> SetNColumns(3);

  TGraphErrors *gPL[npos];
  TGraphErrors *gPR[npos];

  TLegend * legP = new TLegend(0.1,0.7,0.48,0.9);
  legP-> SetNColumns(3);

  TCanvas * C = new TCanvas("c","",0,0,1500,800);
  C->Draw();

  Double_t d[npos];
  Double_t de[npos];
  Double_t dl[npos];
  Double_t dle[npos];
  Double_t dr[npos];
  Double_t dre[npos];

  for(int i=0;i<nbeame;i++){
    // std::cout<<" energy = "<<beamE[E]<<std::endl;
    for(Int_t k=0;k<npos;k++){	
      d[k]=data[k][i][T];
      de[k]=dataerr[k][i][T];
      dl[k]=peL[k][i][T];
      dle[k]=peLE[k][i][T];
      dr[k]=peR[k][i][T];
      dre[k]=peRE[k][i][T];
    }

   
    gPL[i] = new TGraphErrors(npos,Pos,dl,PosE,dle);
    gPL[i]->SetMarkerColor(i+1);
    gPL[i]->SetLineColor(i+1);
    gPL[i]->SetMarkerStyle(20);
    mgP->Add(gPL[i]);
    legP->AddEntry(gPL[i],Form("LEFT : E = %3.0f MeV",beamE[i]),"lep");
    gPR[i] = new TGraphErrors(npos,Pos,dr,PosE,dre);
    gPR[i]->SetMarkerColor(i+1);
    gPR[i]->SetLineColor(i+1);
    gPR[i]->SetMarkerStyle(24);
    mgP->Add(gPR[i]);
    legP->AddEntry(gPR[i],Form("RIGHT : E = %3.0f MeV",beamE[i]),"lep");

  }

   
  // Draw pe
  C->Clear();
  mgP->Draw("AECP");
  mgP->GetXaxis()->SetTitle(posl);
  mgP->GetYaxis()->SetTitle("Number of pe's");
  mgP->SetTitle(Form("Position scan for %s",posl.Data()));
  legP->Draw();
  gPad->SetGridx();
  gPad->SetGridy();
  


  C->Print(Form("%s_position_scans_at_theta_0.png",cstem.Data()));
  C->Print(Form("%s_position_scans_pe_at_theta_0.C",cstem.Data()));
}



double*  get_no_pe(TString energy, TString angle, TString pos){

  static double results[4];


  //  Get the rootfiles
  // I am using strings here instead of double because for some reason I cannot get leading zeros to print on the 2nd and 3rd variables.
  TString dir = Form("/lustre/expphy/volatile/hallc/qweak/buddhiniw/farmoutput/jobs/new_L_%s_%s_335.00_575.00_%s_000",energy.Data(),pos.Data(),angle.Data());
  TFile * file = TFile::Open(Form("%s/QwSim_0.root",dir.Data()));

  std::cout<<"Opeining file ="<<dir<<std::endl;
  TTree *t =  (TTree*)file->Get("QweakSimG4_Tree");
  t->Draw("QweakSimUserMainEvent.Cerenkov.PMT.PMTLeftNbOfPEs[3]>>left","","goff");
  TH1 * hl = gDirectory->Get("left");
  results[0] = hl->GetMean();
  results[1]=(hl->GetRMS())/sqrt(hl->GetEntries());


  t->Draw("QweakSimUserMainEvent.Cerenkov.PMT.PMTRightNbOfPEs[3]>>right","","goff");
  TH1 * hr = gDirectory->Get("right");
  results[2]= hr->GetMean();
  results[3]=(hr->GetRMS())/sqrt(hr->GetEntries());

  std::cout<<"No Of PE in LEFT PMT = "<< hl->GetMean()<<"+-"<< hl->GetRMS()<<std::endl;
  std::cout<<"No Of PE in RIGHT PMT = "<<  hr->GetMean()<<"+-"<< hr->GetRMS()<<std::endl;
  delete file;
  file = ((TFile *)0);

  return results;
}

double*  get_pe_asym(TString energy, TString angle, TString pos){

  static double results[2];

  //  Get the rootfiles
  // I am using strings here instead of double because for some reason I cannot get leading zeros to print on the 2nd and 3rd variables.
  TString dir = Form("/lustre/expphy/volatile/hallc/qweak/buddhiniw/farmoutput/jobs/new_L_%s_%s_335.00_575.00_%s_000",energy.Data(),pos.Data(),angle.Data());
  TFile * file = TFile::Open(Form("%s/QwSim_0.root",dir.Data()));

  //std::cout<<"Opeining file ="<<dir<<std::endl;
  TTree *t =  (TTree*)file->Get("QweakSimG4_Tree");
  t->Draw("(QweakSimUserMainEvent.Cerenkov.PMT.PMTLeftNbOfPEs[3]-QweakSimUserMainEvent.Cerenkov.PMT.PMTRightNbOfPEs[3])/(QweakSimUserMainEvent.Cerenkov.PMT.PMTLeftNbOfPEs[3]+QweakSimUserMainEvent.Cerenkov.PMT.PMTRightNbOfPEs[3])>>asym","","");
  TH1 * h = gDirectory->Get("asym");
  results[0] = h->GetMean();
  results[1]=(h->GetRMS())/sqrt(h->GetEntries());

  // //std::cout<<"LEFT-RIGHT PMT asymmetry = "<< h->GetMean()<<"+-"<< h->GetRMS()<<std::endl;
  delete file;
  file = ((TFile *)0);

  return results;
}
