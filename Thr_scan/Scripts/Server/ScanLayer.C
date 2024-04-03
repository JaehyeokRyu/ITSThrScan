#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TRandom.h>
#include <TClonesArray.h>
#include <TStopwatch.h>
#include <TAxis.h>
#include <THnSparse.h>
#include <THashList.h>
#include <TString.h>
#include <TLorentzVector.h>
#include <TVector2.h>
#include <TRandom3.h>

#include <fstream>
#include <sstream>

using namespace std;


void ScanLayer()
{

    int lnchip[8] = {0, 108, 252, 432, 3120, 6480, 14712, 24120};
    char ffname[2000];
    int nlayer = 2;
    int nchips = lnchip[nlayer+1]-lnchip[nlayer];

    TH2D *Threshold[nchips];
    

   
    ifstream flist;
	flist.open("flist.txt");
	cout << "flist open" << endl;

    // TFile *infile = new TFile("532623_merged.root", "read");
   

     for (int c = 0; c < nchips; c++){
        // Threshold[c] = new TH2D(Form("Threshold_layer%d_stave%d_chipID%d", l, s, c), "", 1024, 0, 1024, 512, 0, 512);
        Threshold[c] = new TH2D(Form("Threshold_chip%d", c + lnchip[nlayer]), Form("Threshold_chip_%d", c + lnchip[nlayer]), 1024, 0, 1024, 512, 0, 512);

    }

    // TH2D *threshold2 = new TH2D("threshold2", "",1024, 0, 1024, 512, 0, 512);


    short chipid[1024];
    short row[1024];
    short thr[1024];
    UChar_t noise[1024];

    

   while ( flist >> ffname ){
   
        cout << "Open : " << ffname << endl;

	    TFile *infile = new TFile(Form("%s", ffname), "read");
        // stringstream ss[1024];

        TTree *T = (TTree*)infile->Get("ITS_calib_tree");

	    if ( !T ){
	    	infile->Close();
	    	delete infile;
	    	return;
	    }

        T->SetBranchAddress("chipid", &chipid);
	    T->SetBranchAddress("row", &row);
	    T->SetBranchAddress("thr", &thr);
	    T->SetBranchAddress("noise", &noise);
        int nent = T->GetEntries();




        for (int ien=0; ien<nent; ien++)
        {
            T->GetEntry(ien);
            int j = chipid[0];
            // if (chipid[0] > 107) continue;
            if (chipid[0] < lnchip[nlayer]) continue;
            int index = j - lnchip[nlayer];
            if (index < 0 || index > nchips) continue;
            for (int i = 0; i < 514; i++)
            {
                // if (row[i]==0)continue;  
                for (int k = 0; k < 1024; k++)
                {
                    // cout << "thr " << noise[k] << endl;
                    
                    if (Threshold[index]) Threshold[index]->SetBinContent(k+1, Threshold[index]->GetYaxis()->FindBin(row[i]), thr[k]);

                }
            }

        }

        delete infile;
   }// while

    TFile *outfile = new TFile("outfile_Layers.root", "RECREATE");
    for (int i=0; i < nchips; i++)
    {
        Threshold[i]->GetXaxis()->SetTitle("column");
        Threshold[i]->GetYaxis()->SetTitle("row");
        // Threshold[i]->Draw("COLZ");
        Threshold[i]->SetStats(0);
        Threshold[i]->Write();
        delete Threshold[i];
    }
   

    cout << "Done" << endl;

}




    



    

