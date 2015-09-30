/*
A script to draw the pe distributions on the PMTs.
08-31-2015  Buddhini Waidyawansa

*/
#include <stdio.h>
#include <TMath.h>

double*  get_no_pe(TString energy, TString angle, TString pos);
double*  get_pe_asym(TString energy, TString angle, TString pos);
void draw_angle_scans_at_fixed_X(int X);
void draw_angle_scans_at_fixed_E(int E);



const int xpos = 15;
const int beame = 4;
const int xangle = 14;

Double_t xPos[xpos]={-40,-20,-10,-8,-6,-4,-2,0,2,4,6,8,10,20,40}; //cm	      
Double_t beamE[beame]={5,10,50,100}; //MeV

// set X angle. But this needs to be converted using xAng = acosine(tan(xAng)) to get the correct X angle due
// to a mistake done in setting the xmomentum inside QwGDD as
// P_X = tan(xAng), P_y = tan(yAng) instead of P_X = Cos(yAng)Sin(xAng) and P_y = Sin(yAng)

//Double_t setAng[xangle]={-40,-35,-30,-25,-20,-15,-10,-7.5,-5,-2.5,0,2.5,7.5,10,15,20,25,30,35,40}; //deg

Double_t setAng[xangle]={-40,-35,-25,-10,-7.5,-5,-2.5,0,2.5,10,15,25,30,35}; //deg

Double_t xAng[xangle];

double data[xpos][beame][xangle];
double dataerr[xpos][beame][xangle];
double peL[xpos][beame][xangle];
double peLE[xpos][beame][xangle];
double peR[xpos][beame][xangle];
double peRE[xpos][beame][xangle];



Double_t xAngE[xangle];
Double_t xPosE[xpos];


void angle_scan(){

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

  gStyle->SetTitleYOffset(0.6);
  gStyle->SetTitleXOffset(1.0);
  gStyle->SetLabelSize(0.06,"x");
  gStyle->SetLabelSize(0.06,"y");
  gStyle->SetTitleSize(0.07,"x");
  gStyle->SetTitleSize(0.07,"y");
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFont(132,"x");
  gStyle->SetTitleFont(132,"y");
  gStyle->SetTitleFont(132,"1");
  gStyle->SetLabelFont(132,"x");
  gStyle->SetLabelFont(132,"y");
  
  //error vectors
  for(int i=0;i<xangle;i++) xAngE[i]=0.0;
  for(int i=0;i<xpos;i++) xPosE[i]=0.0;

  double *p;
  double *r;

  TString energy;
  TString angle;
  TString position;  

 FILE *fp;
  fp=fopen("md3_angle_scan.txt", "w");
  fprintf(fp, "X(cm),\tE(MeV),\tXTheta(deg),\t A,\t dA,\t peL,\t dpeL,\t peR,\t dpeR\n");

  for(Int_t X=0;X<xpos;X++){
    // for each X position
    position = Form("%06.2f",xPos[X]);
    //std::cout<<" shift in X (cm) ="<<position<<std::endl;
    
    for(int E=0;E<beame;E++){
      //For each beam energy
      energy = Form("%04.0f",beamE[E]);
      //std::cout<<" energy (MeV) ="<<energy<<std::endl;
      
     
      for(Int_t T=0;T<xangle;T++){
	//for each X angle
     
	xAng[T]= (180/TMath::Pi())*TMath::ASin(TMath::Tan((TMath::Pi()/180)*setAng[T]));
	angle = Form("%06.2f",xAng[T]);
	std::cout<<"\n### actual angle = "<<xAng[T]<<" deg "<<std::endl;
	std::cout<<"\n### set angle = "<<setAng[T]<<" deg "<<std::endl;

	// get the left right pmt asymmetry
      	p =  get_pe_asym(energy, Form("%06.2f",setAng[T]), position);
	for ( int i = 0; i < 2; i++ ){
	    printf( "*(p + %d) : %f\n", i, *(p + i));
	}
	//store the data in a 3-d array for later use
	data[X][E][T] = *(p);
	dataerr[X][E][T] = *(p+1);
	std::cout<<"\n##################################"<<std::endl;
	std::cout<<"asym at X = "
		 <<xPos[X]<<" cm; E="
		 <<beamE[E]<<" MeV; #theta ="
		 <<xAng[T]<<" = "
		 <<data[X][E][T]<<"+-"<<dataerr[X][E][T]<<std::endl;

	// get the total number of pe in each tube
      	r =  get_no_pe(energy, Form("%06.2f",setAng[T]), position);
	for ( int i = 0; i < 4; i++ ){
	  printf( "*(r + %d) : %f\n", i, *(r + i));
	}
	peL[X][E][T] = *(r);
	peLE[X][E][T] = *(r+1);
	peR[X][E][T] = *(r+2);
	peRE[X][E][T] = *(r+3);
	std::cout<<"\n##################################"<<std::endl;
	std::cout<<"pe at X = "
		 <<xPos[X]<<" cm; E="
		 <<beamE[E]<<" MeV; #theta ="
		 <<xAng[T]
		 <<"; Left =  "<<peL[X][E][T]<<"+-"<<peLE[X][E][T]
		 <<"; Right =  "<<peR[X][E][T]<<"+-"<<peRE[X][E][T]
		 <<std::endl;



       	fprintf(fp, "%3.2f,\t%3.2f,\t%3.2f,\t%3.4f,\t%3.4f,\t%3.2f,\t%3.2f,\t%3.2f,\t%3.2f\n",xPos[X],beamE[E],setAng[T],data[X][E][T],dataerr[X][E][T],peL[X][E][T],peLE[X][E][T],peR[X][E][T],peRE[X][E][T]);
	// 	exit(1);



      }
    }
  }

 
  //////////////////////////////
  // Angle scans at different Es and at fixed X
  //////////////////////////////
  
  for(int j=0;j<xpos;j++) draw_angle_scans_at_fixed_X(j); 
 
 
  //////////////////////////////
  // Angle scans at different X positions along the bar at a fixed energy
  //////////////////////////////
  // for(j=0;j<beame;j++) draw_angle_scans_at_fixed_E(j); 
  
  std::cout<<"Done!"<<std::endl;



}

void draw_angle_scans_at_fixed_X(int X){
 
  TGraphErrors *gE[beame];
  TMultiGraph * mgE = new TMultiGraph("",Form("pe asymmetry vs electron entrance angle on MD3 bare bar [e Gun @ X=%2.0fcm, Y=335cm, Z=575cm]",xPos[X]));
  TLegend * lege = new TLegend(0.1,0.7,0.48,0.9);
  lege-> SetNColumns(3);

  TGraphErrors *gPL[beame];
  TGraphErrors *gPR[beame];
  TMultiGraph * mgP = new TMultiGraph("",Form("pe's vs electron entrance angle on MD3 bare bar [e Gun @ X=%2.0fcm, Y=335cm, Z=575cm]",xPos[X]));
  TLegend * legP = new TLegend(0.1,0.7,0.48,0.9);
  legP-> SetNColumns(3);


  TCanvas * C = new TCanvas("c", "",0,0,1500,800);
  C->Draw();
  for(int E=0;E<beame;E++){
    gE[E] = new TGraphErrors(xangle,xAng,(Double_t*)data[X][E],xAngE,(Double_t*)dataerr[X][E]);
    gE[E]->SetMarkerColor(E+1);
    gE[E]->SetLineColor(E+1);
    gE[E]->SetMarkerStyle(21);
    mgE->Add(gE[E]);
    lege->AddEntry(gE[E],Form("E = %3.0f MeV",beamE[E]),"lep");

    gPL[E] = new TGraphErrors(xangle,xAng,(Double_t*)peL[X][E],xAngE,(Double_t*)peLE[X][E]);
    gPL[E]->SetMarkerColor(E+1);
    gPL[E]->SetLineColor(E+1);
    gPL[E]->SetMarkerStyle(20);
    mgP->Add(gPL[E]);
    legP->AddEntry(gPL[E],Form("LEFT : E = %3.0f MeV",beamE[E]),"lep");
    gPR[E] = new TGraphErrors(xangle,xAng,(Double_t*)peR[X][E],xAngE,(Double_t*)peRE[X][E]);
    gPR[E]->SetMarkerColor(E+1);
    gPR[E]->SetLineColor(E+1);
    gPR[E]->SetMarkerStyle(24);
    mgP->Add(gPR[E]);
    legP->AddEntry(gPR[E],Form("RIGHT : E = %3.0f MeV",beamE[E]),"lep");
 
  }

  // Draw asymmetry
  mgE->Draw("AECP");
  mgE->GetXaxis()->SetTitle("X Angle (deg)");
  mgE->GetYaxis()->SetTitle("Left/Right pe asymmetry");
  lege->Draw();
  gPad->SetGridx();
  gPad->SetGridy();

  C->Print(Form("angle_scans_asym_at_x_%3.2f.png",xPos[X]));
  C->Print(Form("angle_scans_asym_at_x_%3.2f.C",xPos[X]));

  // Draw total pe
  C->Clear();
  mgP->Draw("AECP");
  mgP->GetXaxis()->SetTitle("X Angle (deg)");
  mgP->GetYaxis()->SetTitle("Number of pe's");
  mgP->SetTitle(Form("Angle scan for X=%3.2f cm",xPos[X]));
  legP->Draw();
  gPad->SetGridx();
  gPad->SetGridy();

  C->Print(Form("angle_scans_pe_at_x_%3.2f.png",xPos[X]));
  C->Print(Form("angle_scans_pe_at_x_%3.2f.C",xPos[X]));


}

void draw_angle_scans_at_fixed_E(int E){
 
  TGraphErrors *gE[xpos];
  TMultiGraph * mgE = new TMultiGraph("",Form("pe asymmetry vs electron entrance angle on MD3 bare bar [e Gun E = %2.0f MeV]",beamE[E]));
  TLegend * lege = new TLegend(0.1,0.7,0.48,0.9);
  lege-> SetNColumns(3);

  TGraphErrors *gPL[xpos];
  TGraphErrors *gPR[xpos];
  TMultiGraph * mgP = new TMultiGraph("",Form("pe's vs e entrance angle on MD3 bare bar [e Gun E = %2.0f MeV]",beamE[E]));
  TLegend * legP = new TLegend(0.1,0.7,0.48,0.9);
  legP-> SetNColumns(3);

  TCanvas * C = new TCanvas("c", "",0,0,1500,800);
  C->Draw();

  Double_t d[xangle];
  Double_t de[xangle];
  Double_t dl[xangle];
  Double_t dle[xangle];
  Double_t dr[xangle];
  Double_t dre[xangle];

  for(int i=0;i<xpos;i++){
    std::cout<<" energy = "<<beamE[E]<<std::endl;
    for(Int_t T=0;T<xangle;T++){	
      d[T]=data[i][E][T];
      de[T]=dataerr[i][E][T];
      dl[T]=peL[i][E][T];
      dle[T]=peLE[i][E][T];
      dr[T]=peR[i][E][T];
      dre[T]=peRE[i][E][T];
    }

    gE[i] = new TGraphErrors(xangle,xAng,d,xAngE,de);
    gE[i]->SetMarkerColor(i+1);
    gE[i]->SetMarkerStyle(21);
    gE[i]->SetLineColor(i+1);
    mgE->Add(gE[i]);
    lege->AddEntry(gE[i],Form("X = %3.0f cm",xPos[i]),"lep");
    
    gPL[i] = new TGraphErrors(xangle,xAng,dl,xAngE,dle);
    gPL[i]->SetMarkerColor(i+1);
    gPL[i]->SetLineColor(i+1);
    gPL[i]->SetMarkerStyle(20);
    mgP->Add(gPL[i]);
    legP->AddEntry(gPL[i],Form("LEFT : X = %3.0f cm",xPos[i]),"lep");
    gPR[i] = new TGraphErrors(xangle,xAng,dr,xAngE,dre);
    gPR[i]->SetMarkerColor(i+1);
    gPR[i]->SetLineColor(i+1);
    gPR[i]->SetMarkerStyle(24);
    mgP->Add(gPR[i]);
    legP->AddEntry(gPR[i],Form("RIGHT : X = %3.0f cm",xPos[i]),"lep");
 

  }

  // Draw asymmetry
  mgE->Draw("AECP");
  mgE->GetXaxis()->SetTitle("X Angle (deg)");
  mgE->GetYaxis()->SetTitle("Left/Right pe asymmetry");
  lege->Draw();
  gPad->SetGridx();
  gPad->SetGridy();
  
  C->Print(Form("angle_scans_asym_at_E_%3.2f.png",beamE[E]));
  C->Print(Form("angle_scans_asym_at_E_%3.2f.C",beamE[E]));
  
  // Draw pe
  C->Clear();
  mgP->Draw("AECP");
  mgP->GetXaxis()->SetTitle("X Angle (deg)");
  mgP->GetYaxis()->SetTitle("Number of pe's");
  mgP->SetTitle(Form("Angle scan for E=%3.0f MeV",beamE[E]));
  legP->Draw();
  gPad->SetGridx();
  gPad->SetGridy();


  C->Print(Form("angle_scans_pe_at_E_%3.2f.png",beamE[E]));
  C->Print(Form("angle_scans_pe_at_E_%3.2f.C",beamE[E]));

}




double*  get_no_pe(TString energy, TString angle, TString pos){

  static double results[4];


  //  Get the rootfiles
  // I am using strings here instead of double because for some reason I cannot get leading zeros to print on the 2nd and 3rd variables.
  TString dir = Form("/lustre/expphy/volatile/hallc/qweak/buddhiniw/farmoutput/jobs/L_%s_%s_335.00_575.00_%s_000",energy.Data(),pos.Data(),angle.Data());
  TFile * file = TFile::Open(Form("%s/QwSim_0.root",dir.Data()));

  std::cout<<"Opeining file ="<<dir<<std::endl;
  TTree *t =  (TTree*)file->Get("QweakSimG4_Tree");
  t->Draw("QweakSimUserMainEvent.Cerenkov.PMT.PMTLeftNbOfPEs>>left","","goff");
  TH1 * hl = gDirectory->Get("left");
  results[0] = hl->GetMean();
  results[1]=(hl->GetRMS())/sqrt(hl->GetEntries());


  t->Draw("QweakSimUserMainEvent.Cerenkov.PMT.PMTRightNbOfPEs>>right","","goff");
  TH1 * hr = gDirectory->Get("right");
  results[2]= hr->GetMean();
  results[3]=(hr->GetRMS())/sqrt(hr->GetEntries());

  //std::cout<<"No Of PE in LEFT PMT = "<< hl->GetMean()<<"+-"<< hl->GetRMS()<<std::endl;
  //std::cout<<"No Of PE in RIGHT PMT = "<<  hr->GetMean()<<"+-"<< hr->GetRMS()<<std::endl;
  delete file;
  file = ((TFile *)0);

  return results;
}

double*  get_pe_asym(TString energy, TString angle, TString pos){

  static double results[2];

  //  Get the rootfiles
  // I am using strings here instead of double because for some reason I cannot get leading zeros to print on the 2nd and 3rd variables.
  TString dir = Form("/lustre/expphy/volatile/hallc/qweak/buddhiniw/farmoutput/jobs/L_%s_%s_335.00_575.00_%s_000",energy.Data(),pos.Data(),angle.Data());
  TFile * file = TFile::Open(Form("%s/QwSim_0.root",dir.Data()));

  //std::cout<<"Opeining file ="<<dir<<std::endl;
  TTree *t =  (TTree*)file->Get("QweakSimG4_Tree");
  t->Draw("(QweakSimUserMainEvent.Cerenkov.PMT.PMTLeftNbOfPEs-QweakSimUserMainEvent.Cerenkov.PMT.PMTRightNbOfPEs)/(QweakSimUserMainEvent.Cerenkov.PMT.PMTLeftNbOfPEs+QweakSimUserMainEvent.Cerenkov.PMT.PMTRightNbOfPEs)>>asym","","");
  TH1 * h = gDirectory->Get("asym");
  results[0] = h->GetMean();
  results[1]=(h->GetRMS())/sqrt(h->GetEntries());

  // //std::cout<<"LEFT-RIGHT PMT asymmetry = "<< h->GetMean()<<"+-"<< h->GetRMS()<<std::endl;
  delete file;
  file = ((TFile *)0);

  return results;
}
