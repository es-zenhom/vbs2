#include <iostream>
#include <TH1D.h>
#include <vector>
#include <TMath.h>
#include <TH1F.h>
#include <TTree.h>
#include <TFile.h>
#include <TCanvas.h>
#include <Math/Vector4D.h>
#include <Math/GenVector/PtEtaPhiM4D.h>
#include <TString.h>
#include <Math/VectorUtil.h>
#include <TH2D.h>
#include "TChain.h"
#include <string>
#include "TString.h"
#include <TProfile.h>
#include <fstream>
#include <filesystem>
#include <map>

// Mapping from file names to dataset labels and their CSV column index (starting from 1 for first data column after 'Region')
std::map<std::string, int> datasetColumns = {
    {"QCD.root", 1},
    {"ttbar.root", 3},
    {"wjets.root", 5},
    {"zjets.root", 7},
    {"VV_VH.root", 9},
    {"rare_top.root", 11},
    {"rare_EWK.root", 13},
    {"VBSVVH.root", 17},
    {"data.root", 15}
};

std::vector<std::string> regions = {
    "RegionA",
    "RegionB",
    "RegionC",
    "RegionD",
    "RegionD1",
    "RegionD2",
    "RegionD3",
    "RegionD4",
    "RegionC1",
    "RegionC2",
    "RegionC3",
    "RegionC4",
    "RegionB1",
    "RegionB2",
    "RegionD1dublicate",
    "RegionD2dublicate"
};
void updateCsvForRegion(const std::string& outputFilename, const std::map<std::string, int>& datasetColumns, const std::string& datasetFile, const std::vector<TH1D*>& histograms, const std::vector<std::string>& regions) {
    int columnIndex = datasetColumns.at(datasetFile); // Get column index for the dataset

    std::ifstream infile(outputFilename);
    std::vector<std::string> lines;
    std::string line;
    while (getline(infile, line)) {
        lines.push_back(line);
    }
    infile.close();

    std::ofstream outfile(outputFilename);
    for (size_t i = 0; i < lines.size(); i++) {
        if (i == 0) { // Skip header
            outfile << lines[i] << "\n";
            continue;
        }

        std::string newLine;
        std::stringstream ss(lines[i]);
        std::vector<std::string> tokens;
        std::string token;

        // Split the line by commas and store tokens
        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        // Ensure we have enough tokens to update
        if (tokens.size() <= static_cast<size_t>(columnIndex + 1)) {
            std::cerr << "Error: Line " << i << " does not have enough columns." << std::endl;
            outfile << lines[i] << "\n"; // Write the line back as-is
            continue;
        }

        // Update the relevant columns for the current dataset
        if (i-1 < regions.size()) { // Ensure we are within the region lines
            std::string value = std::to_string(histograms[i-1]->GetBinContent(1));
            std::string error = std::to_string(histograms[i-1]->GetBinError(1));
            tokens[columnIndex] = value; // Value
            tokens[columnIndex + 1] = error; // Error
        }

        // Reconstruct the line
        newLine = tokens[0]; // Start with the first token (region)
        for (size_t j = 1; j < tokens.size(); j++) {
            newLine += "," + tokens[j];
        }

        outfile << newLine << "\n";
    }
    outfile.close();
}

int main(int argc, const char* argv[])
{
	std::string outputFilename = argv[7];;
    std::ofstream outfile;
    bool fillAllEvents = (argc > 8 && std::string(argv[8]) == "all");


    // Check if file exists and has content
    bool file_exists = std::ifstream(outputFilename).good();

    outfile.open(outputFilename, std::ios::out | std::ios::app);

    // If file doesn't exist, create it and write headers
    if (!file_exists) {
		std::cout << "Writing headers and initializing regions..." << std::endl;
        outfile << "Region, QCD, QCD_err, TTbar, TTbar_err, Wjets, Wjets_err, Zjets, Zjets_err, VV_VH, VV_VH_err, Rare_top, Rare_top_err, Rare_EWK, Rare_EWK_err, Data, Data_err, VBSVVH, VBSVVH_err\n";

        // Initialize rows for each region with empty data
        for (const auto& region : regions) {
            outfile << region;
            for (int iii = 0; iii < 18; ++iii) { // Assuming 8 datasets * 2 columns each
                outfile << ", ";
            }
            outfile << "\n";
        }
		outfile.flush(); // Ensure data is written to the file system

    }
	using namespace std;
	std::vector<TString> input_files;
	if (argc<8)
	{
		std::cout << "enter at lease two arguments" <<std::endl;
		std::cout << "help:" << std::endl;
        std::cout << "" << std::endl;
        std::cout << " e.g." << std::endl;
        std::cout << "    $ " << argv[0] << " " << "/user/sote/blahbalh/somedirectory" << std::endl;
		std::cout << "    $ " << argv[0] << " " << "don't put / at the end of the directory path" << std::endl;
        std::cout << "    $ " << argv[1] << " " << "name of first root file" <<  std::endl;
        std::cout << "    $ " << argv[2] << " " << "name of second root file" << std::endl;
        std::cout << "    $ " << argv[3] << " " << "name of third root file" << std::endl;
        std::cout << "Usage: " << argv[0] << " <directory> <input_file> <output_file> <cut_abcdnet_score> <cut_abs_deta_jj> <cut_m_jj> <output_csv> <all_events_flag>" << std::endl;

        return 1;
	}


	int n_jets,n_loose_b_jets, n_medium_b_jets, n_tight_b_jets, n_fatjets, bQuarksInHiggsJet;
	double HT,ld_vbsjet_pt,ld_vbsjet_eta,ld_vbsjet_phi,tr_vbsjet_pt,tr_vbsjet_eta,tr_vbsjet_phi,M_jj,deta_jj,abs_deta_jj;
	double MET,ld_vqqfatjet_pt,ld_vqqfatjet_eta,ld_vqqfatjet_phi,ld_vqqfatjet_mass,ld_vqqfatjet_msoftdrop,tr_vqqfatjet_pt,tr_vqqfatjet_eta;
	double tr_vqqfatjet_phi,tr_vqqfatjet_mass,tr_vqqfatjet_msoftdrop,hbbfatjet_pt,hbbfatjet_eta,hbbfatjet_phi,hbbfatjet_mass,hbbfatjet_msoftdrop,ST;
	double ld_vqqjet_pt,ld_vqqjet_eta,ld_vqqjet_phi,ld_vqqjet_mass,tr_vqqjet_pt,tr_vqqjet_eta,tr_vqqjet_phi,tr_vqqjet_mass,vqqjets_Mjj,vqqjets_dR;
	// for the semi merged channel (the two ak4 jets)

	double vqqjets_pt, vqqjets_phi, vqqjets_mass, vqqjets_eta;

	double ld_w_ld_fermion_pt, ld_w_ld_fermion_eta, ld_w_ld_fermion_phi, ld_w_ld_fermion_mass, ld_w_tr_fermion_pt, ld_w_tr_fermion_eta, ld_w_tr_fermion_phi,
	ld_w_tr_fermion_mass, tr_w_ld_fermion_pt, tr_w_ld_fermion_eta, tr_w_ld_fermion_phi, tr_w_ld_fermion_mass, tr_w_tr_fermion_pt, tr_w_tr_fermion_eta,
	tr_w_tr_fermion_phi, tr_w_tr_fermion_mass, ld_w_ld_fermion_pdgId, ld_w_tr_fermion_pdgId, tr_w_ld_fermion_pdgId, tr_w_tr_fermion_pdgId;
	// double lhe_M_qq;

	// gen level
	double gen_ld_v_ld_fermion_pt, gen_ld_v_ld_fermion_eta, gen_ld_v_ld_fermion_phi, gen_ld_v_ld_fermion_mass;
	double gen_ld_v_tr_fermion_pt, gen_ld_v_tr_fermion_eta, gen_ld_v_tr_fermion_phi, gen_ld_v_tr_fermion_mass;
	double gen_tr_v_ld_fermion_pt, gen_tr_v_ld_fermion_eta, gen_tr_v_ld_fermion_phi, gen_tr_v_ld_fermion_mass;
	double gen_tr_v_tr_fermion_pt, gen_tr_v_tr_fermion_eta, gen_tr_v_tr_fermion_phi, gen_tr_v_tr_fermion_mass;

	double gen_ld_v_ld_fermion_pdgId, gen_ld_v_tr_fermion_pdgId, gen_tr_v_ld_fermion_pdgId, gen_tr_v_tr_fermion_pdgId;

	double gen_ld_h_bb_pt, gen_ld_h_bb_eta, gen_ld_h_bb_phi, gen_ld_h_bb_mass;
	double gen_tr_h_bb_pt, gen_tr_h_bb_eta, gen_tr_h_bb_phi, gen_tr_h_bb_mass;

	double gen_ld_h_non_bb_pt, gen_ld_h_non_bb_eta, gen_ld_h_non_bb_phi, gen_ld_h_non_bb_mass;
	double gen_tr_h_non_bb_pt, gen_tr_h_non_bb_eta, gen_tr_h_non_bb_phi, gen_tr_h_non_bb_mass;

	double gen_ld_h_bb_pdgId, gen_tr_h_bb_pdgId, gen_ld_h_non_bb_pdgId, gen_tr_h_non_bb_pdgId;

	double gen_ld_vbf_quarks_pt, gen_ld_vbf_quarks_eta, gen_ld_vbf_quarks_phi, gen_ld_vbf_quarks_mass;
	double gen_tr_vbf_quarks_pt, gen_tr_vbf_quarks_eta, gen_tr_vbf_quarks_phi, gen_tr_vbf_quarks_mass;

	// for the addition
	double gen_ld_v_mass, gen_ld_v_pt, gen_ld_v_eta, gen_ld_v_phi;
	double gen_tr_v_mass, gen_tr_v_pt, gen_tr_v_eta, gen_tr_v_phi;

	double gen_h_bb_mass, gen_h_bb_pt, gen_h_bb_eta, gen_h_bb_phi;
	double gen_h_non_bb_mass, gen_h_non_bb_pt, gen_h_non_bb_eta, gen_h_non_bb_phi;

	double gen_tr_v_dR_fermmions, gen_ld_v_dR_fermmions, gen_h_bb_dR, gen_h_non_bb_dR;

	double gen_vbf_quarks_deltaphi, gen_vbf_quarks_deltaeta, gen_vbf_quarks_dijetmass;

	int v1_pdgid, v2_pdgid;
	double xsec_sf;
	double pu_sf;
	double prefire_sf;
	double puid_sf;
	double xwqq_ld_vqq_sf;
	double xbb_sf;
	double hbbfatjet_xbb;

	double ld_vqqfatjet_xwqq;
	double ld_vqqfatjet_xvqq;

	double M_VVH_semimerged;

	double abcdnet_score;

	double no_events=0;
	double no_eventsD=0;
	double no_events_weighted=0;

	float lhe_muF0p5_muR1p0=0;
	float lhe_muF2p0_muR1p0=0;

	float ps_isr2p0_fsr1p0=0;
	float ps_isr1p0_fsr2p0=0;
	float ps_isr0p5_fsr1p0=0;
	float ps_isr1p0_fsr0p5=0;

	double HT_fat;




	std::string arg2 = argv[2];
	std::string dir_for_rootfiles = argv[1];
	std::string outputname = argv[3];
    double cut_abcdnet_score = std::stod(argv[4]);
    double cut_abs_deta_jj = std::stod(argv[5]);
    double cut_m_jj = std::stod(argv[6]);

	for (int i = 2; i < 3; i++)
	{
		std::string name_root_file = argv[i];
		TString file_path = TString::Format("%s/%s", dir_for_rootfiles.c_str(),name_root_file.c_str());
		input_files.push_back(file_path);
	}
	TFile * f_output;
	TString outputname_constchar = TString::Format("%s", outputname.c_str());
	f_output =new TFile (outputname_constchar,"recreate");

	//creat the histograms
	TH1D* hist_HT = new TH1D ("hist_HT", "hist_HT", 80, 0, 1200);
	TH1F* hist_n_jets = new TH1F ("hist_n_jets", "hist_n_jets", 10, 0, 10);
	TH1F* hist_n_loose_b_jets = new TH1F ("hist_n_loose_b_jets", "hist_n_loose_b_jets", 5, 0, 5);
	TH1D* hist_n_medium_b_jets = new TH1D ("hist_n_medium_b_jets", "hist_n_medium_b_jets", 80, 0, 5);
	TH1D* hist_n_tight_b_jets = new TH1D ("hist_n_tight_b_jets", "hist_n_tight_b_jets", 80, 0, 5);
	TH1F* hist_n_fatjets = new TH1F ("hist_n_fatjets", "hist_n_fatjets", 5, 0, 5);
	TH1D* hist_ld_vbsjet_pt = new TH1D ("hist_ld_vbsjet_pt", "hist_ld_vbsjet_pt", 80, 0, 800);
	TH1D* hist_ld_vbsjet_eta = new TH1D ("hist_ld_vbsjet_eta", "hist_ld_vbsjet_eta", 80, -6, 6);
	TH1D* hist_ld_vbsjet_phi = new TH1D ("hist_ld_vbsjet_phi", "hist_ld_vbsjet_phi", 80, -6, 6);
	TH1D* hist_tr_vbsjet_pt = new TH1D ("hist_tr_vbsjet_pt", "hist_tr_vbsjet_pt", 80, 0, 500);
	TH1D* hist_tr_vbsjet_eta = new TH1D ("hist_tr_vbsjet_eta", "hist_tr_vbsjet_eta", 80, -6, 6);
	TH1D* hist_tr_vbsjet_phi = new TH1D ("hist_tr_vbsjet_phi", "hist_tr_vbsjet_phi", 80, -6, 6);
	TH1D* hist_M_jj = new TH1D ("hist_M_jj", "hist_M_jj", 80, 0, 10000);
	TH1D* hist_M_jj_0_250 = new TH1D ("hist_M_jj_0_250", "hist_M_jj_0_250", 80, 0, 250);
	TH1D* hist_deta_jj = new TH1D ("hist_deta_jj", "hist_deta_jj", 80, -10, 10);
	TH1D* hist_abs_deta_jj = new TH1D ("hist_abs_deta_jj", "hist_abs_deta_jj", 80, 0, 10);
	TH1D* hist_MET = new TH1D ("hist_MET", "hist_MET", 80, 0, 800);
	// TH1D* hist_ld_vqqfatjet_score = new TH1D ("hist_ld_vqqfatjet_score", "hist_ld_vqqfatjet_score", 100, 0, 1);
	TH1D* hist_ld_vqqfatjet_pt = new TH1D ("hist_ld_vqqfatjet_pt", "hist_ld_vqqfatjet_pt", 80, 0, 3000);
	TH1D* hist_ld_vqqfatjet_eta = new TH1D ("hist_ld_vqqfatjet_eta", "hist_ld_vqqfatjet_eta", 80, -6, 6);
	TH1D* hist_ld_vqqfatjet_phi = new TH1D ("hist_ld_vqqfatjet_phi", "hist_ld_vqqfatjet_phi", 80, -6, 6);
	TH1D* hist_ld_vqqfatjet_mass = new TH1D ("hist_ld_vqqfatjet_mass", "hist_ld_vqqfatjet_mass", 80, 0, 250);
	TH1D* hist_ld_vqqfatjet_msoftdrop = new TH1D ("hist_ld_vqqfatjet_msoftdrop", "hist_ld_vqqfatjet_msoftdrop", 80, 0, 250);
	// TH1D* hist_tr_vqqfatjet_score = new TH1D ("hist_tr_vqqfatjet_score", "hist_tr_vqqfatjet_score", 100, 0, 1);
	// TH1D* hist_tr_vqqfatjet_pt = new TH1D ("hist_tr_vqqfatjet_pt", "hist_tr_vqqfatjet_pt", 80, 0, 3000);
	// TH1D* hist_tr_vqqfatjet_eta = new TH1D ("hist_tr_vqqfatjet_eta", "hist_tr_vqqfatjet_eta", 80, -6, 6);
	// TH1D* hist_tr_vqqfatjet_phi = new TH1D ("hist_tr_vqqfatjet_phi", "hist_tr_vqqfatjet_phi", 80, -6, 6);
	// TH1D* hist_tr_vqqfatjet_mass = new TH1D ("hist_tr_vqqfatjet_mass", "hist_tr_vqqfatjet_mass", 80, 0, 250);
	// TH1D* hist_tr_vqqfatjet_msoftdrop = new TH1D ("hist_tr_vqqfatjet_msoftdrop", "hist_tr_vqqfatjet_msoftdrop", 80, 0, 250);
	// TH1D* hist_hbbfatjet_score = new TH1D ("hist_hbbfatjet_score", "hist_hbbfatjet_score", 100, 0, 1);
	TH1D* hist_hbbfatjet_pt = new TH1D ("hist_hbbfatjet_pt", "hist_hbbfatjet_pt", 80, 0, 3000);
	TH1D* hist_hbbfatjet_eta = new TH1D ("hist_hbbfatjet_eta", "hist_hbbfatjet_eta", 80, -6, 6);
	TH1D* hist_hbbfatjet_phi = new TH1D ("hist_hbbfatjet_phi", "hist_hbbfatjet_phi", 80, -6, 6);
	TH1D* hist_hbbfatjet_mass = new TH1D ("hist_hbbfatjet_mass", "hist_hbbfatjet_mass", 80, 0, 250);
	TH1D* hist_hbbfatjet_msoftdrop = new TH1D ("hist_hbbfatjet_msoftdrop", "hist_hbbfatjet_msoftdrop", 80, 0, 250);
	TH1D* hist_ST = new TH1D ("hist_ST", "hist_ST", 80, 0, 6000);

	// for the semi merged channel (the two ak4 jets)
	TH1D* hist_ld_vqqjet_pt = new TH1D ("hist_ld_vqqjet_pt", "hist_ld_vqqjet_pt", 80, 0, 2000);
	TH1D* hist_ld_vqqjet_eta = new TH1D ("hist_ld_vqqjet_eta", "hist_ld_vqqjet_eta", 80, -6, 6);
	TH1D* hist_ld_vqqjet_phi = new TH1D ("hist_ld_vqqjet_phi", "hist_ld_vqqjet_phi", 80, -6, 6);
	TH1D* hist_ld_vqqjet_mass = new TH1D ("hist_ld_vqqjet_mass", "hist_ld_vqqjet_mass", 80, 0, 100);
	TH1D* hist_tr_vqqjet_pt = new TH1D ("hist_tr_vqqjet_pt", "hist_tr_vqqjet_pt", 80, 0, 500);
	TH1D* hist_tr_vqqjet_eta = new TH1D ("hist_tr_vqqjet_eta", "hist_tr_vqqjet_eta", 80, -6, 6);
	TH1D* hist_tr_vqqjet_phi = new TH1D ("hist_tr_vqqjet_phi", "hist_tr_vqqjet_phi", 80, -6, 6);
	TH1D* hist_tr_vqqjet_mass = new TH1D ("hist_tr_vqqjet_mass", "hist_tr_vqqjet_mass", 80, 0, 100);
	TH1D* hist_vqqjets_Mjj = new TH1D ("hist_vqqjets_Mjj", "hist_vqqjets_Mjj", 80, 0, 250);
	TH1D* hist_vqqjets_dR = new TH1D ("hist_vqqjets_dR", "hist_vqqjets_dR", 80, 0, 10);

//  combining the two vqq ak4 jets four vectors
	TH1D* hist_vqqjets_pt = new TH1D ("hist_vqqjets_pt", "hist_vqqjets_pt", 80, 0, 2000);
	TH1D* hist_vqqjets_eta = new TH1D ("hist_vqqjets_eta", "hist_vqqjets_eta", 80, -6, 6);
	TH1D* hist_vqqjets_phi = new TH1D ("hist_vqqjets_phi", "hist_vqqjets_phi", 80, -6, 6);
	TH1D* hist_vqqjets_mass = new TH1D ("hist_vqqjets_mass", "hist_vqqjets_mass", 80, 0, 250);

	TH1D* hist_bQuarksInHiggsJet = new TH1D ("hist_bQuarksInHiggsJet", "hist_bQuarksInHiggsJet", 80, 0, 6);

	TH1D* hist_ld_w_ld_fermion_pt = new TH1D("hist_ld_w_ld_fermion_pt", "hist_ld_w_ld_fermion_pt", 80, 0, 2000);
	TH1D* hist_ld_w_ld_fermion_eta = new TH1D("hist_ld_w_ld_fermion_eta", "hist_ld_w_ld_fermion_eta", 80, -6, 6);
	TH1D* hist_ld_w_ld_fermion_phi = new TH1D("hist_ld_w_ld_fermion_phi", "hist_ld_w_ld_fermion_phi", 80, -6, 6);
	TH1D* hist_ld_w_ld_fermion_mass = new TH1D("hist_ld_w_ld_fermion_mass", "hist_ld_w_ld_fermion_mass", 80, 0, 200);
	TH1D* hist_ld_w_tr_fermion_eta = new TH1D("hist_ld_w_tr_fermion_eta", "hist_ld_w_tr_fermion_eta", 80, -6, 6);
	TH1D* hist_ld_w_tr_fermion_phi = new TH1D("hist_ld_w_tr_fermion_phi", "hist_ld_w_tr_fermion_phi", 80, -6, 6);
	TH1D* hist_ld_w_tr_fermion_mass = new TH1D("hist_ld_w_tr_fermion_mass", "hist_ld_w_tr_fermion_mass", 80, 0, 0.0001);
	TH1D* hist_ld_w_tr_fermion_pt = new TH1D("hist_ld_w_tr_fermion_pt", "hist_ld_w_tr_fermion_pt", 80, 0, 1000);
	TH1D* hist_tr_w_ld_fermion_eta = new TH1D("hist_tr_w_ld_fermion_eta", "hist_tr_w_ld_fermion_eta", 80, -6, 6);
	TH1D* hist_tr_w_ld_fermion_phi = new TH1D("hist_tr_w_ld_fermion_phi", "hist_tr_w_ld_fermion_phi", 80, -6, 6);
	TH1D* hist_tr_w_ld_fermion_mass = new TH1D("hist_tr_w_ld_fermion_mass", "hist_tr_w_ld_fermion_mass", 80, 0, 0.0001);
	TH1D* hist_tr_w_tr_fermion_mass = new TH1D("hist_tr_w_tr_fermion_mass", "hist_tr_w_tr_fermion_mass", 80, 0, 0.0001);
	TH1D* hist_tr_w_tr_fermion_eta = new TH1D("hist_tr_w_tr_fermion_eta", "hist_tr_w_tr_fermion_eta", 80, -6, 6);
	TH1D* hist_tr_w_tr_fermion_phi = new TH1D("hist_tr_w_tr_fermion_phi", "hist_tr_w_tr_fermion_phi", 80, -6, 6);
	TH1D* hist_tr_w_ld_fermion_pt = new TH1D("hist_tr_w_ld_fermion_pt", "hist_tr_w_ld_fermion_pt", 80, 0, 2000);
	TH1D* hist_tr_w_tr_fermion_pt = new TH1D("hist_tr_w_tr_fermion_pt", "hist_tr_w_tr_fermion_pt", 80, 0, 1000);

	TH1D* hist_ld_w_ld_fermion_pdgId = new TH1D("hist_ld_w_ld_fermion_pdgId", "hist_ld_w_ld_fermion_pdgId", 40, -20, 20);
	TH1D* hist_ld_w_tr_fermion_pdgId = new TH1D("hist_ld_w_tr_fermion_pdgId", "hist_ld_w_tr_fermion_pdgId", 40, -20, 20);
	TH1D* hist_tr_w_ld_fermion_pdgId = new TH1D("hist_tr_w_ld_fermion_pdgId", "hist_tr_w_ld_fermion_pdgId", 40, -20, 20);
	TH1D* hist_tr_w_tr_fermion_pdgId = new TH1D("hist_tr_w_tr_fermion_pdgId", "hist_tr_w_tr_fermion_pdgId", 40, -20, 20);

	// TH1D* hist_lhe_M_qq = new TH1D("lhe_M_qq", "lhe_M_qq", 80, 0, 2000);

	// gen level
	TH1D* hist_gen_ld_v_ld_fermion_pt = new TH1D("hist_gen_ld_v_ld_fermion_pt", "hist_gen_ld_v_ld_fermion_pt", 80, 0, 2000);
	TH1D* hist_gen_ld_v_ld_fermion_eta = new TH1D("hist_gen_ld_v_ld_fermion_eta", "hist_gen_ld_v_ld_fermion_eta", 80, -6, 6);
	TH1D* hist_gen_ld_v_ld_fermion_phi = new TH1D("hist_gen_ld_v_ld_fermion_phi", "hist_gen_ld_v_ld_fermion_phi", 80, -6, 6);
	TH1D* hist_gen_ld_v_ld_fermion_mass = new TH1D("hist_gen_ld_v_ld_fermion_mass", "hist_gen_ld_v_ld_fermion_mass", 80, 0, 50);
	TH1D* hist_gen_ld_v_tr_fermion_pt = new TH1D("hist_gen_ld_v_tr_fermion_pt", "hist_gen_ld_v_tr_fermion_pt", 80, 0, 2000);
	TH1D* hist_gen_ld_v_tr_fermion_eta = new TH1D("hist_gen_ld_v_tr_fermion_eta", "hist_gen_ld_v_tr_fermion_eta", 80, -6, 6);
	TH1D* hist_gen_ld_v_tr_fermion_phi = new TH1D("hist_gen_ld_v_tr_fermion_phi", "hist_gen_ld_v_tr_fermion_phi", 80, -6, 6);
	TH1D* hist_gen_ld_v_tr_fermion_mass = new TH1D("hist_gen_ld_v_tr_fermion_mass", "hist_gen_ld_v_tr_fermion_mass", 80, 0, 50);
	TH1D* hist_gen_tr_v_ld_fermion_pt = new TH1D("hist_gen_tr_v_ld_fermion_pt", "hist_gen_tr_v_ld_fermion_pt", 80, 0, 2000);
	TH1D* hist_gen_tr_v_ld_fermion_eta = new TH1D("hist_gen_tr_v_ld_fermion_eta", "hist_gen_tr_v_ld_fermion_eta", 80, -6, 6);
	TH1D* hist_gen_tr_v_ld_fermion_phi = new TH1D("hist_gen_tr_v_ld_fermion_phi", "hist_gen_tr_v_ld_fermion_phi", 80, -6, 6);
	TH1D* hist_gen_tr_v_ld_fermion_mass = new TH1D("hist_gen_tr_v_ld_fermion_mass", "hist_gen_tr_v_ld_fermion_mass", 80, 0, 50);
	TH1D* hist_gen_tr_v_tr_fermion_pt = new TH1D("hist_gen_tr_v_tr_fermion_pt", "hist_gen_tr_v_tr_fermion_pt", 80, 0, 1000);
	TH1D* hist_gen_tr_v_tr_fermion_eta = new TH1D("hist_gen_tr_v_tr_fermion_eta", "hist_gen_tr_v_tr_fermion_eta", 80, -6, 6);
	TH1D* hist_gen_tr_v_tr_fermion_phi = new TH1D("hist_gen_tr_v_tr_fermion_phi", "hist_gen_tr_v_tr_fermion_phi", 80, -6, 6);
	TH1D* hist_gen_tr_v_tr_fermion_mass = new TH1D("hist_gen_tr_v_tr_fermion_mass", "hist_gen_tr_v_tr_fermion_mass", 80, 0, 50);
	TH1D* hist_gen_ld_v_ld_fermion_pdgId = new TH1D("hist_gen_ld_v_ld_fermion_pdgId", "hist_gen_ld_v_ld_fermion_pdgId", 40, -20, 20);
	TH1D* hist_gen_ld_v_tr_fermion_pdgId = new TH1D("hist_gen_ld_v_tr_fermion_pdgId", "hist_gen_ld_v_tr_fermion_pdgId", 40, -20, 20);
	TH1D* hist_gen_tr_v_ld_fermion_pdgId = new TH1D("hist_gen_tr_v_ld_fermion_pdgId", "hist_gen_tr_v_ld_fermion_pdgId", 40, -20, 20);
	TH1D* hist_gen_tr_v_tr_fermion_pdgId = new TH1D("hist_gen_tr_v_tr_fermion_pdgId", "hist_gen_tr_v_tr_fermion_pdgId", 40, -20, 20);

	// Initialize branches for leading and trailing h_bb
	TH1D* hist_gen_ld_h_bb_pt = new TH1D("hist_gen_ld_h_bb_pt", "hist_gen_ld_h_bb_pt", 80, 0, 2000);
	TH1D* hist_gen_ld_h_bb_eta = new TH1D("hist_gen_ld_h_bb_eta", "hist_gen_ld_h_bb_eta", 80, -6, 6);
	TH1D* hist_gen_ld_h_bb_phi = new TH1D("hist_gen_ld_h_bb_phi", "hist_gen_ld_h_bb_phi", 80, -6, 6);
	TH1D* hist_gen_ld_h_bb_mass = new TH1D("hist_gen_ld_h_bb_mass", "hist_gen_ld_h_bb_mass", 80, 0, 50);
	TH1D* hist_gen_tr_h_bb_pt = new TH1D("hist_gen_tr_h_bb_pt", "hist_gen_tr_h_bb_pt", 80, 0, 1000);
	TH1D* hist_gen_tr_h_bb_eta = new TH1D("hist_gen_tr_h_bb_eta", "hist_gen_tr_h_bb_eta", 80, -6, 6);
	TH1D* hist_gen_tr_h_bb_phi = new TH1D("hist_gen_tr_h_bb_phi", "hist_gen_tr_h_bb_phi", 80, -6, 6);
	TH1D* hist_gen_tr_h_bb_mass = new TH1D("hist_gen_tr_h_bb_mass", "hist_gen_tr_h_bb_mass", 80, 0, 50);

	// Initialize branches for leading and trailing h_non_bb
	TH1D* hist_gen_ld_h_non_bb_pt = new TH1D("hist_gen_ld_h_non_bb_pt", "hist_gen_ld_h_non_bb_pt", 80, 0, 2000);
	TH1D* hist_gen_ld_h_non_bb_eta = new TH1D("hist_gen_ld_h_non_bb_eta", "hist_gen_ld_h_non_bb_eta", 80, -6, 6);
	TH1D* hist_gen_ld_h_non_bb_phi = new TH1D("hist_gen_ld_h_non_bb_phi", "hist_gen_ld_h_non_bb_phi", 80, -6, 6);
	TH1D* hist_gen_ld_h_non_bb_mass = new TH1D("hist_gen_ld_h_non_bb_mass", "hist_gen_ld_h_non_bb_mass", 80, 0, 100);

	TH1D* hist_gen_tr_h_non_bb_pt = new TH1D("hist_gen_tr_h_non_bb_pt", "hist_gen_tr_h_non_bb_pt", 80, 0, 1000);
	TH1D* hist_gen_tr_h_non_bb_eta = new TH1D("hist_gen_tr_h_non_bb_eta", "hist_gen_tr_h_non_bb_eta", 80, -6, 6);
	TH1D* hist_gen_tr_h_non_bb_phi = new TH1D("hist_gen_tr_h_non_bb_phi", "hist_gen_tr_h_non_bb_phi", 80, -6, 6);
	TH1D* hist_gen_tr_h_non_bb_mass = new TH1D("hist_gen_tr_h_non_bb_mass", "hist_gen_tr_h_non_bb_mass", 80, 0, 100);

	// Initialize branches for pdgId for h_bb and h_non_bb
	TH1D* hist_gen_ld_h_bb_pdgId = new TH1D("hist_gen_ld_h_bb_pdgId", "hist_gen_ld_h_bb_pdgId", 40, -20, 20);
	TH1D* hist_gen_tr_h_bb_pdgId = new TH1D("hist_gen_tr_h_bb_pdgId", "hist_gen_tr_h_bb_pdgId", 40, -20, 20);
	TH1D* hist_gen_ld_h_non_bb_pdgId = new TH1D("hist_gen_ld_h_non_bb_pdgId", "hist_gen_ld_h_non_bb_pdgId", 60, -30, 30);
	TH1D* hist_gen_tr_h_non_bb_pdgId = new TH1D("hist_gen_tr_h_non_bb_pdgId", "hist_gen_tr_h_non_bb_pdgId", 60, -30, 30);

	// Initialize branches for leading and trailing vbf_quarks
	TH1D* hist_gen_ld_vbf_quarks_pt = new TH1D("hist_gen_ld_vbf_quarks_pt", "hist_gen_ld_vbf_quarks_pt", 80, 0, 1000);
	TH1D* hist_gen_ld_vbf_quarks_eta = new TH1D("hist_gen_ld_vbf_quarks_eta", "hist_gen_ld_vbf_quarks_eta", 80, -6, 6);
	TH1D* hist_gen_ld_vbf_quarks_phi = new TH1D("hist_gen_ld_vbf_quarks_phi", "hist_gen_ld_vbf_quarks_phi", 80, -6, 6);
	TH1D* hist_gen_ld_vbf_quarks_mass = new TH1D("hist_gen_ld_vbf_quarks_mass", "hist_gen_ld_vbf_quarks_mass", 80, 0, 50);

	TH1D* hist_gen_tr_vbf_quarks_pt = new TH1D("hist_gen_tr_vbf_quarks_pt", "hist_gen_tr_vbf_quarks_pt", 80, 0, 1000);
	TH1D* hist_gen_tr_vbf_quarks_eta = new TH1D("hist_gen_tr_vbf_quarks_eta", "hist_gen_tr_vbf_quarks_eta", 80, -6, 6);
	TH1D* hist_gen_tr_vbf_quarks_phi = new TH1D("hist_gen_tr_vbf_quarks_phi", "hist_gen_tr_vbf_quarks_phi", 80, -6, 6);
	TH1D* hist_gen_tr_vbf_quarks_mass = new TH1D("hist_gen_tr_vbf_quarks_mass", "hist_gen_tr_vbf_quarks_mass", 80, 0, 50);

	// for the addition of each pair of fermions
	// Initialize histograms for gen_ld_w series
	TH1D* hist_gen_ld_v_mass = new TH1D("hist_gen_ld_v_mass", "hist_gen_ld_v_mass", 80, 0, 100);
	TH1D* hist_gen_ld_v_pt = new TH1D("hist_gen_ld_v_pt", "hist_gen_ld_v_pt", 80, 0, 3000);
	TH1D* hist_gen_ld_v_eta = new TH1D("hist_gen_ld_v_eta", "hist_gen_ld_v_eta", 80, -6, 6);
	TH1D* hist_gen_ld_v_phi = new TH1D("hist_gen_ld_v_phi", "hist_gen_ld_v_phi", 80, -6, 6);

	// Initialize histograms for gen_tr_v series
	TH1D* hist_gen_tr_v_mass = new TH1D("hist_gen_tr_v_mass", "hist_gen_tr_v_mass", 80, 0, 100);
	TH1D* hist_gen_tr_v_pt = new TH1D("hist_gen_tr_v_pt", "hist_gen_tr_v_pt", 80, 0, 3000);
	TH1D* hist_gen_tr_v_eta = new TH1D("hist_gen_tr_v_eta", "hist_gen_tr_v_eta", 80, -6, 6);
	TH1D* hist_gen_tr_v_phi = new TH1D("hist_gen_tr_v_phi", "hist_gen_tr_v_phi", 80, -6, 6);

	// Initialize histograms for gen_h_bb series
	TH1D* hist_gen_h_bb_mass = new TH1D("hist_gen_h_bb_mass", "hist_gen_h_bb_mass", 80, 0, 250);
	TH1D* hist_gen_h_bb_pt = new TH1D("hist_gen_h_bb_pt", "hist_gen_h_bb_pt", 80, 0, 2000);
	TH1D* hist_gen_h_bb_eta = new TH1D("hist_gen_h_bb_eta", "hist_gen_h_bb_eta", 80, -6, 6);
	TH1D* hist_gen_h_bb_phi = new TH1D("hist_gen_h_bb_phi", "hist_gen_h_bb_phi", 80, -6, 6);

	// Initialize histograms for gen_h_non_bb series
	TH1D* hist_gen_h_non_bb_mass = new TH1D("hist_gen_h_non_bb_mass", "hist_gen_h_non_bb_mass", 80, 0, 250);
	TH1D* hist_gen_h_non_bb_pt = new TH1D("hist_gen_h_non_bb_pt", "hist_gen_h_non_bb_pt", 80, 0, 2000);
	TH1D* hist_gen_h_non_bb_eta = new TH1D("hist_gen_h_non_bb_eta", "hist_gen_h_non_bb_eta", 80, -6, 6);
	TH1D* hist_gen_h_non_bb_phi = new TH1D("hist_gen_h_non_bb_phi", "hist_gen_h_non_bb_phi", 80, -6, 6);

	// Initialize histograms for dR and other variables
	TH1D* hist_gen_tr_v_dR_fermmions = new TH1D("hist_gen_tr_v_dR_fermmions", "hist_gen_tr_v_dR_fermmions", 80, 0, 10);
	TH1D* hist_gen_ld_v_dR_fermmions = new TH1D("hist_gen_ld_v_dR_fermmions", "hist_gen_ld_v_dR_fermmions", 80, 0, 10);
	TH1D* hist_gen_h_bb_dR = new TH1D("hist_gen_h_bb_dR", "hist_gen_h_bb_dR", 80, 0, 10);
	TH1D* hist_gen_h_non_bb_dR = new TH1D("hist_gen_h_non_bb_dR", "hist_gen_h_non_bb_dR", 80, 0, 10);

	// Initialize histograms for gen_vbf_quarks series
	TH1D* hist_gen_vbf_quarks_deltaphi = new TH1D("hist_gen_vbf_quarks_deltaphi", "hist_gen_vbf_quarks_deltaphi", 80, -6, 6);
	TH1D* hist_gen_vbf_quarks_deltaeta = new TH1D("hist_gen_vbf_quarks_deltaeta", "hist_gen_vbf_quarks_deltaeta", 80, -10, 10);
	TH1D* hist_gen_vbf_quarks_dijetmass = new TH1D("hist_gen_vbf_quarks_dijetmass", "hist_gen_vbf_quarks_dijetmass", 80, 0, 3000);

	// the pdgid branch to differntiate what is the process of the vvh: wzh wwh zzh oswwh
	TH1D* hist_v1_pdgid = new TH1D("hist_v1_pdgid", "hist_v1_pdgid", 60, -30, 30);
	TH1D* hist_v2_pdgid = new TH1D("hist_v2_pdgid", "hist_v2_pdgid", 60, -30, 30);

	TH1D* hist_hbbfatjet_xbb = new TH1D("hist_hbbfatjet_xbb", "hist_hbbfatjet_xbb", 60, 0, 1);
	TH1D* hist_ld_vqqfatjet_xwqq = new TH1D("hist_ld_vqqfatjet_xwqq", "hist_ld_vqqfatjet_xwqq", 60, 0, 1);
	TH1D* hist_ld_vqqfatjet_xvqq = new TH1D("hist_ld_vqqfatjet_xvqq", "hist_ld_vqqfatjet_xvqq", 60, 0, 1);

	TH1D* hist_M_VVH_semimerged = new TH1D("hist_M_VVH_semimerged", "hist_M_VVH_semimerged", 80, 0, 1600);

	TH1D* hist_abcdnet_score = new TH1D("hist_abcdnet_score", "hist_abcdnet_score", 80, 0, 1);

	TH1D *h = new TH1D("h", "Total Events", 1, 0, 1); // One bin from 0 to 1

	// Histograms for main regions
	TH1D* hist_regionA = new TH1D ("hist_regionA", "Histogram for Region A", 1, 0, 1);
	TH1D* hist_regionB = new TH1D ("hist_regionB", "Histogram for Region B", 1, 0, 1);
	TH1D* hist_regionC = new TH1D ("hist_regionC", "Histogram for Region C", 1, 0, 1);
	TH1D* hist_regionD = new TH1D ("hist_regionD", "Histogram for Region D", 1, 0, 1);

	// Histograms for subregions of D
	TH1D* hist_regionD1 = new TH1D ("hist_regionD1", "Histogram for Region D1", 1, 0, 1);
	TH1D* hist_regionD2 = new TH1D ("hist_regionD2", "Histogram for Region D2", 1, 0, 1);
	TH1D* hist_regionD3 = new TH1D ("hist_regionD3", "Histogram for Region D3", 1, 0, 1);
	TH1D* hist_regionD4 = new TH1D ("hist_regionD4", "Histogram for Region D4", 1, 0, 1);

	// Histograms for subregions of C
	TH1D* hist_regionC1 = new TH1D ("hist_regionC1", "Histogram for Region C1", 1, 0, 1);
	TH1D* hist_regionC2 = new TH1D ("hist_regionC2", "Histogram for Region C2", 1, 0, 1);
	TH1D* hist_regionC3 = new TH1D ("hist_regionC3", "Histogram for Region C3", 1, 0, 1);
	TH1D* hist_regionC4 = new TH1D ("hist_regionC4", "Histogram for Region C4", 1, 0, 1);

	// Histograms for subregions of B
	TH1D* hist_regionB1 = new TH1D ("hist_regionB1", "Histogram for Region B1", 1, 0, 1);
	TH1D* hist_regionB2 = new TH1D ("hist_regionB2", "Histogram for Region B2", 1, 0, 1);


	TH2D* hist_2D = new TH2D("hist_2D", " ", 80, 0, 7000, 80, 0, 1);
	TH2D* hist_2D_flipped = new TH2D("hist_2D_flipped", "correlation2D_abcdnetscore_Mjj_flipped", 80, 0, 1, 80, 0, 7000);

    TH1D* hist_abcdnet_fixedEta = new TH1D("hist_abcdnet_fixedEta", "ABCDNet Score with Fixed Eta", 100, 0.0, 1.0);  // 100 bins between 0 and 1
    TH1D* hist_eta_fixedABCDNet = new TH1D("hist_eta_fixedABCDNet", "Abs Delta Eta with Fixed ABCDNet", 100, 0.0, 10.0);  // 100 bins between 0 and 10

	TH1D* hist_HT_fat = new TH1D ("hist_HT_fat", "hist_HT_fat", 80, 0, 5000);






	for (int ifile = 0; ifile < input_files.size(); ifile++)
	{
		TFile* f1 = new TFile(input_files[ifile]);
		TTree* t1 = (TTree*) f1->Get("tree");
		// std::string rootFile = input_files[ifile].Data(); // Convert TString to std::string
        // std::string datasetType = file_labels[rootFile];
		std::string currentRootFile = input_files[ifile].Data(); // Current ROOT file name
		std::string filename = std::filesystem::path(currentRootFile).filename(); // Extract the filename from the path

		// Check if the currentRootFile exists in the datasetColumns map
		if (datasetColumns.find(filename) == datasetColumns.end()) {
		    std::cerr << "Error: dataset file not recognized: " << filename << std::endl;
		    return 1; // or handle the error appropriately
		}
		// std::string datasetLabel = file_labels[currentRootFile]; // Get the dataset type based on the file name




		t1->SetBranchAddress("HT", &HT);
		t1->SetBranchAddress("n_jets", &n_jets);
		t1->SetBranchAddress("n_loose_b_jets", &n_loose_b_jets);
		t1->SetBranchAddress("n_medium_b_jets", &n_medium_b_jets);
		t1->SetBranchAddress("n_tight_b_jets", &n_tight_b_jets);
		t1->SetBranchAddress("n_fatjets", &n_fatjets);
		t1->SetBranchAddress("ld_vbsjet_pt", &ld_vbsjet_pt);
		t1->SetBranchAddress("ld_vbsjet_eta", &ld_vbsjet_eta);
		t1->SetBranchAddress("ld_vbsjet_phi", &ld_vbsjet_phi);
		t1->SetBranchAddress("tr_vbsjet_pt", &tr_vbsjet_pt);
		t1->SetBranchAddress("tr_vbsjet_eta", &tr_vbsjet_eta);
		t1->SetBranchAddress("tr_vbsjet_phi", &tr_vbsjet_phi);
		t1->SetBranchAddress("M_jj", &M_jj);
		t1->SetBranchAddress("deta_jj", &deta_jj);
		t1->SetBranchAddress("abs_deta_jj", &abs_deta_jj);
		t1->SetBranchAddress("MET", &MET);
		// t1->SetBranchAddress("ld_vqqfatjet_score", &ld_vqqfatjet_score);
		t1->SetBranchAddress("ld_vqqfatjet_pt", &ld_vqqfatjet_pt);
		t1->SetBranchAddress("ld_vqqfatjet_eta", &ld_vqqfatjet_eta);
		t1->SetBranchAddress("ld_vqqfatjet_phi", &ld_vqqfatjet_phi);
		t1->SetBranchAddress("ld_vqqfatjet_mass", &ld_vqqfatjet_mass);
		t1->SetBranchAddress("ld_vqqfatjet_msoftdrop", &ld_vqqfatjet_msoftdrop);
		// t1->SetBranchAddress("tr_vqqfatjet_score", &tr_vqqfatjet_score);
		// t1->SetBranchAddress("tr_vqqfatjet_pt", &tr_vqqfatjet_pt);
		// t1->SetBranchAddress("tr_vqqfatjet_eta", &tr_vqqfatjet_eta);
		// t1->SetBranchAddress("tr_vqqfatjet_phi", &tr_vqqfatjet_phi);
		// t1->SetBranchAddress("tr_vqqfatjet_mass", &tr_vqqfatjet_mass);
		// t1->SetBranchAddress("tr_vqqfatjet_msoftdrop", &tr_vqqfatjet_msoftdrop);
		// t1->SetBranchAddress("hbbfatjet_score", &hbbfatjet_score);
		t1->SetBranchAddress("hbbfatjet_pt", &hbbfatjet_pt);
		t1->SetBranchAddress("hbbfatjet_eta", &hbbfatjet_eta);
		t1->SetBranchAddress("hbbfatjet_phi", &hbbfatjet_phi);
		t1->SetBranchAddress("hbbfatjet_mass", &hbbfatjet_mass);
		t1->SetBranchAddress("hbbfatjet_msoftdrop", &hbbfatjet_msoftdrop);
		t1->SetBranchAddress("ST", &ST);

		// for the semi merged channel (the two ak4 jets)
		t1->SetBranchAddress("ld_vqqjet_pt", &ld_vqqjet_pt);
		t1->SetBranchAddress("ld_vqqjet_eta", &ld_vqqjet_eta);
		t1->SetBranchAddress("ld_vqqjet_phi", &ld_vqqjet_phi);
		t1->SetBranchAddress("ld_vqqjet_mass", &ld_vqqjet_mass);
		t1->SetBranchAddress("tr_vqqjet_pt", &tr_vqqjet_pt);
		t1->SetBranchAddress("tr_vqqjet_eta", &tr_vqqjet_eta);
		t1->SetBranchAddress("tr_vqqjet_phi", &tr_vqqjet_phi);
		t1->SetBranchAddress("tr_vqqjet_mass", &tr_vqqjet_mass);
		t1->SetBranchAddress("vqqjets_Mjj", &vqqjets_Mjj);
		t1->SetBranchAddress("vqqjets_dR", &vqqjets_dR);
		//  combining the two vqq ak4 jets four vectors
		t1->SetBranchAddress("vqqjets_pt", &vqqjets_pt);
		t1->SetBranchAddress("vqqjets_eta", &vqqjets_eta);
		t1->SetBranchAddress("vqqjets_phi", &vqqjets_phi);
		t1->SetBranchAddress("vqqjets_mass", &vqqjets_mass);

		t1->SetBranchAddress("bQuarksInHiggsJet", &bQuarksInHiggsJet);


		t1->SetBranchAddress("ld_w_ld_fermion_pt", &ld_w_ld_fermion_pt);
		t1->SetBranchAddress("ld_w_ld_fermion_eta", &ld_w_ld_fermion_eta);
		t1->SetBranchAddress("ld_w_ld_fermion_phi", &ld_w_ld_fermion_phi);
		t1->SetBranchAddress("ld_w_ld_fermion_mass", &ld_w_ld_fermion_mass);
		t1->SetBranchAddress("ld_w_tr_fermion_pt", &ld_w_tr_fermion_pt);
		t1->SetBranchAddress("ld_w_tr_fermion_eta", &ld_w_tr_fermion_eta);
		t1->SetBranchAddress("ld_w_tr_fermion_phi", &ld_w_tr_fermion_phi);
		t1->SetBranchAddress("ld_w_tr_fermion_mass", &ld_w_tr_fermion_mass);
		t1->SetBranchAddress("tr_w_ld_fermion_pt", &tr_w_ld_fermion_pt);
		t1->SetBranchAddress("tr_w_ld_fermion_eta", &tr_w_ld_fermion_eta);
		t1->SetBranchAddress("tr_w_ld_fermion_phi", &tr_w_ld_fermion_phi);
		t1->SetBranchAddress("tr_w_ld_fermion_mass", &tr_w_ld_fermion_mass);
		t1->SetBranchAddress("tr_w_tr_fermion_pt", &tr_w_tr_fermion_pt);
		t1->SetBranchAddress("tr_w_tr_fermion_eta", &tr_w_tr_fermion_eta);
		t1->SetBranchAddress("tr_w_tr_fermion_phi", &tr_w_tr_fermion_phi);
		t1->SetBranchAddress("tr_w_tr_fermion_mass", &tr_w_tr_fermion_mass);
		t1->SetBranchAddress("ld_w_ld_fermion_pdgId", &ld_w_ld_fermion_pdgId);
		t1->SetBranchAddress("ld_w_tr_fermion_pdgId", &ld_w_tr_fermion_pdgId);
		t1->SetBranchAddress("tr_w_ld_fermion_pdgId", &tr_w_ld_fermion_pdgId);
		t1->SetBranchAddress("tr_w_tr_fermion_pdgId", &tr_w_tr_fermion_pdgId);

		// t1->SetBranchAddress("lhe_M_qq", &lhe_M_qq);

		// gen level
		t1->SetBranchAddress("gen_ld_v_ld_fermion_pt", &gen_ld_v_ld_fermion_pt);
		t1->SetBranchAddress("gen_ld_v_ld_fermion_eta", &gen_ld_v_ld_fermion_eta);
		t1->SetBranchAddress("gen_ld_v_ld_fermion_phi", &gen_ld_v_ld_fermion_phi);
		t1->SetBranchAddress("gen_ld_v_ld_fermion_mass", &gen_ld_v_ld_fermion_mass);

		t1->SetBranchAddress("gen_ld_v_tr_fermion_pt", &gen_ld_v_tr_fermion_pt);
		t1->SetBranchAddress("gen_ld_v_tr_fermion_eta", &gen_ld_v_tr_fermion_eta);
		t1->SetBranchAddress("gen_ld_v_tr_fermion_phi", &gen_ld_v_tr_fermion_phi);
		t1->SetBranchAddress("gen_ld_v_tr_fermion_mass", &gen_ld_v_tr_fermion_mass);

		t1->SetBranchAddress("gen_tr_v_ld_fermion_pt", &gen_tr_v_ld_fermion_pt);
		t1->SetBranchAddress("gen_tr_v_ld_fermion_eta", &gen_tr_v_ld_fermion_eta);
		t1->SetBranchAddress("gen_tr_v_ld_fermion_phi", &gen_tr_v_ld_fermion_phi);
		t1->SetBranchAddress("gen_tr_v_ld_fermion_mass", &gen_tr_v_ld_fermion_mass);

		t1->SetBranchAddress("gen_tr_v_tr_fermion_pt", &gen_tr_v_tr_fermion_pt);
		t1->SetBranchAddress("gen_tr_v_tr_fermion_eta", &gen_tr_v_tr_fermion_eta);
		t1->SetBranchAddress("gen_tr_v_tr_fermion_phi", &gen_tr_v_tr_fermion_phi);
		t1->SetBranchAddress("gen_tr_v_tr_fermion_mass", &gen_tr_v_tr_fermion_mass);

		t1->SetBranchAddress("gen_ld_v_ld_fermion_pdgId", &gen_ld_v_ld_fermion_pdgId);
		t1->SetBranchAddress("gen_ld_v_tr_fermion_pdgId", &gen_ld_v_tr_fermion_pdgId);
		t1->SetBranchAddress("gen_tr_v_ld_fermion_pdgId", &gen_tr_v_ld_fermion_pdgId);
		t1->SetBranchAddress("gen_tr_v_tr_fermion_pdgId", &gen_tr_v_tr_fermion_pdgId);

		t1->SetBranchAddress("gen_ld_h_bb_pt", &gen_ld_h_bb_pt);
		t1->SetBranchAddress("gen_ld_h_bb_eta", &gen_ld_h_bb_eta);
		t1->SetBranchAddress("gen_ld_h_bb_phi", &gen_ld_h_bb_phi);
		t1->SetBranchAddress("gen_ld_h_bb_mass", &gen_ld_h_bb_mass);

		t1->SetBranchAddress("gen_tr_h_bb_pt", &gen_tr_h_bb_pt);
		t1->SetBranchAddress("gen_tr_h_bb_eta", &gen_tr_h_bb_eta);
		t1->SetBranchAddress("gen_tr_h_bb_phi", &gen_tr_h_bb_phi);
		t1->SetBranchAddress("gen_tr_h_bb_mass", &gen_tr_h_bb_mass);

		t1->SetBranchAddress("gen_ld_h_non_bb_pt", &gen_ld_h_non_bb_pt);
		t1->SetBranchAddress("gen_ld_h_non_bb_eta", &gen_ld_h_non_bb_eta);
		t1->SetBranchAddress("gen_ld_h_non_bb_phi", &gen_ld_h_non_bb_phi);
		t1->SetBranchAddress("gen_ld_h_non_bb_mass", &gen_ld_h_non_bb_mass);

		t1->SetBranchAddress("gen_tr_h_non_bb_pt", &gen_tr_h_non_bb_pt);
		t1->SetBranchAddress("gen_tr_h_non_bb_eta", &gen_tr_h_non_bb_eta);
		t1->SetBranchAddress("gen_tr_h_non_bb_phi", &gen_tr_h_non_bb_phi);
		t1->SetBranchAddress("gen_tr_h_non_bb_mass", &gen_tr_h_non_bb_mass);

		t1->SetBranchAddress("gen_ld_h_bb_pdgId", &gen_ld_h_bb_pdgId);
		t1->SetBranchAddress("gen_tr_h_bb_pdgId", &gen_tr_h_bb_pdgId);
		t1->SetBranchAddress("gen_ld_h_non_bb_pdgId", &gen_ld_h_non_bb_pdgId);
		t1->SetBranchAddress("gen_tr_h_non_bb_pdgId", &gen_tr_h_non_bb_pdgId);

		t1->SetBranchAddress("gen_ld_vbf_quarks_pt", &gen_ld_vbf_quarks_pt);
		t1->SetBranchAddress("gen_ld_vbf_quarks_eta", &gen_ld_vbf_quarks_eta);
		t1->SetBranchAddress("gen_ld_vbf_quarks_phi", &gen_ld_vbf_quarks_phi);
		t1->SetBranchAddress("gen_ld_vbf_quarks_mass", &gen_ld_vbf_quarks_mass);

		t1->SetBranchAddress("gen_tr_vbf_quarks_pt", &gen_tr_vbf_quarks_pt);
		t1->SetBranchAddress("gen_tr_vbf_quarks_eta", &gen_tr_vbf_quarks_eta);
		t1->SetBranchAddress("gen_tr_vbf_quarks_phi", &gen_tr_vbf_quarks_phi);
		t1->SetBranchAddress("gen_tr_vbf_quarks_mass", &gen_tr_vbf_quarks_mass);

		// for the addition of each pair of fermions
		// Set branch address for gen_ld_v series
		t1->SetBranchAddress("gen_ld_v_mass", &gen_ld_v_mass);
		t1->SetBranchAddress("gen_ld_v_pt", &gen_ld_v_pt);
		t1->SetBranchAddress("gen_ld_v_eta", &gen_ld_v_eta);
		t1->SetBranchAddress("gen_ld_v_phi", &gen_ld_v_phi);

		// Set branch address for gen_tr_v series
		t1->SetBranchAddress("gen_tr_v_mass", &gen_tr_v_mass);
		t1->SetBranchAddress("gen_tr_v_pt", &gen_tr_v_pt);
		t1->SetBranchAddress("gen_tr_v_eta", &gen_tr_v_eta);
		t1->SetBranchAddress("gen_tr_v_phi", &gen_tr_v_phi);

		// Set branch address for gen_h_bb series
		t1->SetBranchAddress("gen_h_bb_mass", &gen_h_bb_mass);
		t1->SetBranchAddress("gen_h_bb_pt", &gen_h_bb_pt);
		t1->SetBranchAddress("gen_h_bb_eta", &gen_h_bb_eta);
		t1->SetBranchAddress("gen_h_bb_phi", &gen_h_bb_phi);

		// Set branch address for gen_h_non_bb series
		t1->SetBranchAddress("gen_h_non_bb_mass", &gen_h_non_bb_mass);
		t1->SetBranchAddress("gen_h_non_bb_pt", &gen_h_non_bb_pt);
		t1->SetBranchAddress("gen_h_non_bb_eta", &gen_h_non_bb_eta);
		t1->SetBranchAddress("gen_h_non_bb_phi", &gen_h_non_bb_phi);

		// Set branch address for dR and other variables
		t1->SetBranchAddress("gen_tr_v_dR_fermmions", &gen_tr_v_dR_fermmions);
		t1->SetBranchAddress("gen_ld_v_dR_fermmions", &gen_ld_v_dR_fermmions);
		t1->SetBranchAddress("gen_h_bb_dR", &gen_h_bb_dR);
		t1->SetBranchAddress("gen_h_non_bb_dR", &gen_h_non_bb_dR);

		// Set branch address for gen_vbf_quarks series
		t1->SetBranchAddress("gen_vbf_quarks_deltaphi", &gen_vbf_quarks_deltaphi);
		t1->SetBranchAddress("gen_vbf_quarks_deltaeta", &gen_vbf_quarks_deltaeta);
		t1->SetBranchAddress("gen_vbf_quarks_dijetmass", &gen_vbf_quarks_dijetmass);

		t1->SetBranchAddress("v1_pdgid", &v1_pdgid);
		t1->SetBranchAddress("v2_pdgid", &v2_pdgid);

		t1->SetBranchAddress("xsec_sf", &xsec_sf);
		t1->SetBranchAddress("pu_sf", &pu_sf);
		t1->SetBranchAddress("prefire_sf", &prefire_sf);
		t1->SetBranchAddress("puid_sf", &puid_sf);
		t1->SetBranchAddress("xbb_sf", &xbb_sf);
		t1->SetBranchAddress("xwqq_ld_vqq_sf", &xwqq_ld_vqq_sf);

		t1->SetBranchAddress("hbbfatjet_xbb", &hbbfatjet_xbb);
		t1->SetBranchAddress("ld_vqqfatjet_xwqq", &ld_vqqfatjet_xwqq);
		t1->SetBranchAddress("ld_vqqfatjet_xvqq", &ld_vqqfatjet_xvqq);

		t1->SetBranchAddress("M_VVH_semimerged", &M_VVH_semimerged);

		t1->SetBranchAddress("abcdnet_score", &abcdnet_score);

		t1->SetBranchAddress("lhe_muF0p5_muR1p0", &lhe_muF0p5_muR1p0);
		t1->SetBranchAddress("lhe_muF2p0_muR1p0", &lhe_muF2p0_muR1p0);

		t1->SetBranchAddress("ps_isr2p0_fsr1p0", &ps_isr2p0_fsr1p0);
		t1->SetBranchAddress("ps_isr1p0_fsr2p0", &ps_isr1p0_fsr2p0);
		t1->SetBranchAddress("ps_isr0p5_fsr1p0", &ps_isr0p5_fsr1p0);
		t1->SetBranchAddress("ps_isr1p0_fsr0p5", &ps_isr1p0_fsr0p5);

		t1->SetBranchAddress("HT_fat", &HT_fat);


		float deltaup_muF=0;
		float deltadown_muF=0;
		float deltaup_isr=0;
		float deltadown_isr=0;
		float deltaup_fsr=0;
		float deltadown_fsr=0;
		double weight_of_eachevent=1;
		// int weight_of_eachevent=1;


		for (int i1 = 0; i1 < t1->GetEntries(); i1++)
		{
			t1->GetEntry(i1);
			bool conditionA = abcdnet_score > cut_abcdnet_score && abs_deta_jj > cut_abs_deta_jj && M_jj > cut_m_jj ;
			bool conditionB = abcdnet_score > cut_abcdnet_score && !(abs_deta_jj > cut_abs_deta_jj && M_jj > cut_m_jj);
			bool conditionC = !(abcdnet_score > cut_abcdnet_score) && (abs_deta_jj > cut_abs_deta_jj && M_jj > cut_m_jj);
			bool conditionD = !(abcdnet_score > cut_abcdnet_score) && !(abs_deta_jj > cut_abs_deta_jj && M_jj > cut_m_jj ) ;
			// Flags to activate subregions
			bool subregionsActive = true;
			bool conditionD1 = abcdnet_score > 0.49 && abs_deta_jj > 3;
			bool conditionD2 = abcdnet_score > 0.49 && !(abs_deta_jj > 3);
			bool conditionD3 = !(abcdnet_score > 0.49) && abs_deta_jj > 3;
			bool conditionD4 = !(abcdnet_score > 0.49) && !(abs_deta_jj > 3);

			bool conditionC1 = abcdnet_score > 0.49 && abs_deta_jj > 8;
			bool conditionC2 = abcdnet_score > 0.49 && !(abs_deta_jj > 8);
			bool conditionC3 = !(abcdnet_score > 0.49) && abs_deta_jj > 8;
			bool conditionC4 = !(abcdnet_score > 0.49) && !(abs_deta_jj > 8);

			bool conditionB1 = conditionB && abs_deta_jj > 3; // abs_deta_jj between 3 and 6
			bool conditionB2 = conditionB && abs_deta_jj <= 3; // abs_deta_jj 3 or below
			no_events++;
			// std::cout << "abcdnet_score" << abcdnet_score << "abs_deta_jj" << abs_deta_jj << "M_jj" <<M_jj <<endl;
			// std::cout << "conditionA" << conditionA<< "conditionB"<<conditionB << "conditionC"<< conditionC<< "conditionD"<<conditionD <<endl;
			// if ( !(abcdnet_score > 0.96 )  ) continue;
			// if ( !(abs_deta_jj > 6) ) continue;
			// if ( !(M_jj > 1000) ) continue;
			 if ( ld_vqqfatjet_xwqq < 0.6) continue;
			// if ( ST < 1800) continue;

			// if ( ld_vqqfatjet_xvqq < 0.6) continue;
			 if ( hbbfatjet_xbb < 0.8) continue;

			if(i1% 10000 ==0) std::cout<<i1<<std::endl;
			weight_of_eachevent = xsec_sf*pu_sf*prefire_sf*puid_sf*xwqq_ld_vqq_sf*xbb_sf;

            if (abs_deta_jj > cut_abs_deta_jj)  // Example fixed cut for abcdnet_score plot
                   hist_abcdnet_fixedEta->Fill(abcdnet_score, weight_of_eachevent);

               if (abcdnet_score > cut_abcdnet_score)  // Example fixed cut for abs_deta_jj plot
                   hist_eta_fixedABCDNet->Fill(abs_deta_jj, weight_of_eachevent);


	       // Main Region B and Subregions B1, B2
	       if (conditionB) {
			   // std::cout << "bbb:" << endl;
	           hist_regionB->Fill(0.5, weight_of_eachevent);  // Fill main Region B histogram
	           if (conditionB1) {
	               hist_regionB1->Fill(0.5, weight_of_eachevent);  // Fill Region B1 Histogram
	           } else if (conditionB2) {
	               hist_regionB2->Fill(0.5, weight_of_eachevent);  // Fill Region B2 Histogram
	           }
	       }

	       // Main Region C and Subregions C1, C2, C3, C4
	       if (conditionC) {
			   // std::cout << "ccccc:" << endl;

	           hist_regionC->Fill(0.5, weight_of_eachevent);  // Fill main Region C histogram
	           if (conditionC1) {
	               hist_regionC1->Fill(0.5, weight_of_eachevent);  // Fill Region C1 Histogram
	           } else if (conditionC2) {
	               hist_regionC2->Fill(0.5, weight_of_eachevent);  // Fill Region C2 Histogram
	           } else if (conditionC3) {
	               hist_regionC3->Fill(0.5, weight_of_eachevent);  // Fill Region C3 Histogram
	           } else if (conditionC4) {
	               hist_regionC4->Fill(0.5, weight_of_eachevent);  // Fill Region C4 Histogram
	           }
	       }

	       // Main Region D and Subregions D1, D2, D3, D4
	       if (conditionD) {
			   no_eventsD++;
			   // std::cout << "ddd:" << endl;

	           hist_regionD->Fill(0.5, weight_of_eachevent);  // Fill main Region D histogram
	           if (conditionD1) {
	               hist_regionD1->Fill(0.5, weight_of_eachevent);  // Fill Region D1 Histogram
	           } else if (conditionD2) {
	               hist_regionD2->Fill(0.5, weight_of_eachevent);  // Fill Region D2 Histogram
	           } else if (conditionD3) {
	               hist_regionD3->Fill(0.5, weight_of_eachevent);  // Fill Region D3 Histogram
	           } else if (conditionD4) {
	               hist_regionD4->Fill(0.5, weight_of_eachevent);  // Fill Region D4 Histogram
	           }
	       }
	   // Main Region A
            // Always fill region-specific histograms
            if (conditionA) {
                hist_regionA->Fill(0.5, weight_of_eachevent);
            }

	       if (fillAllEvents || conditionA) {
			   // std::cout << "aaaa:" << endl;



				no_events++;
				no_events_weighted = no_events_weighted + weight_of_eachevent;
				deltaup_muF= weight_of_eachevent* lhe_muF2p0_muR1p0 + deltaup_muF ;
				deltadown_muF= weight_of_eachevent* lhe_muF0p5_muR1p0 + deltadown_muF ;

				deltaup_isr= weight_of_eachevent* ps_isr2p0_fsr1p0 + deltaup_isr ;
				deltadown_isr= weight_of_eachevent* ps_isr0p5_fsr1p0 + deltadown_isr ;

				deltaup_fsr= weight_of_eachevent* ps_isr1p0_fsr2p0 + deltaup_fsr ;
				deltadown_fsr= weight_of_eachevent* ps_isr1p0_fsr0p5 + deltadown_fsr ;

				hist_HT->Fill(HT, weight_of_eachevent);
				hist_n_jets->Fill(n_jets, weight_of_eachevent);
				hist_n_loose_b_jets->Fill(n_loose_b_jets, weight_of_eachevent);
				hist_n_medium_b_jets->Fill(n_medium_b_jets, weight_of_eachevent);
				hist_n_tight_b_jets->Fill(n_tight_b_jets, weight_of_eachevent);
				hist_n_fatjets->Fill(n_fatjets, weight_of_eachevent);
				hist_ld_vbsjet_pt->Fill(ld_vbsjet_pt, weight_of_eachevent);
				hist_ld_vbsjet_eta->Fill(ld_vbsjet_eta, weight_of_eachevent);
				hist_ld_vbsjet_phi->Fill(ld_vbsjet_phi, weight_of_eachevent);
				hist_tr_vbsjet_pt->Fill(tr_vbsjet_pt, weight_of_eachevent);
				hist_tr_vbsjet_eta->Fill(tr_vbsjet_eta, weight_of_eachevent);
				hist_tr_vbsjet_phi->Fill(tr_vbsjet_phi, weight_of_eachevent);
				hist_M_jj->Fill(M_jj, weight_of_eachevent);
				hist_M_jj_0_250->Fill(M_jj, weight_of_eachevent);
				hist_deta_jj->Fill(deta_jj, weight_of_eachevent);
				hist_abs_deta_jj->Fill(abs_deta_jj, weight_of_eachevent);
				hist_MET->Fill(MET, weight_of_eachevent);
				// hist_ld_vqqfatjet_score->Fill(ld_vqqfatjet_score, weight_of_eachevent);
				hist_ld_vqqfatjet_pt->Fill(ld_vqqfatjet_pt, weight_of_eachevent);
				hist_ld_vqqfatjet_eta->Fill(ld_vqqfatjet_eta, weight_of_eachevent);
				hist_ld_vqqfatjet_phi->Fill(ld_vqqfatjet_phi, weight_of_eachevent);
				hist_ld_vqqfatjet_mass->Fill(ld_vqqfatjet_mass, weight_of_eachevent);
				hist_ld_vqqfatjet_msoftdrop->Fill(ld_vqqfatjet_msoftdrop, weight_of_eachevent);
				// hist_tr_vqqfatjet_score->Fill(tr_vqqfatjet_score, weight_of_eachevent);
				// hist_tr_vqqfatjet_pt->Fill(tr_vqqfatjet_pt, weight_of_eachevent);
				// hist_tr_vqqfatjet_eta->Fill(tr_vqqfatjet_eta, weight_of_eachevent);
				// hist_tr_vqqfatjet_phi->Fill(tr_vqqfatjet_phi, weight_of_eachevent);
				// hist_tr_vqqfatjet_mass->Fill(tr_vqqfatjet_mass, weight_of_eachevent);
				// hist_tr_vqqfatjet_msoftdrop->Fill(tr_vqqfatjet_msoftdrop, weight_of_eachevent);
				// hist_hbbfatjet_score->Fill(hbbfatjet_score, weight_of_eachevent);
				hist_hbbfatjet_pt->Fill(hbbfatjet_pt, weight_of_eachevent);
				hist_hbbfatjet_eta->Fill(hbbfatjet_eta, weight_of_eachevent);
				hist_hbbfatjet_phi->Fill(hbbfatjet_phi, weight_of_eachevent);
				hist_hbbfatjet_mass->Fill(hbbfatjet_mass, weight_of_eachevent);
				hist_hbbfatjet_msoftdrop->Fill(hbbfatjet_msoftdrop, weight_of_eachevent);
				hist_ST->Fill(ST, weight_of_eachevent);

				// for the semi merged channel (the two ak4 jets)
				hist_ld_vqqjet_pt->Fill(ld_vqqjet_pt, weight_of_eachevent);
				hist_ld_vqqjet_eta->Fill(ld_vqqjet_eta, weight_of_eachevent);
				hist_ld_vqqjet_phi->Fill(ld_vqqjet_phi, weight_of_eachevent);
				hist_ld_vqqjet_mass->Fill(ld_vqqjet_mass, weight_of_eachevent);
				hist_tr_vqqjet_pt->Fill(tr_vqqjet_pt, weight_of_eachevent);
				hist_tr_vqqjet_eta->Fill(tr_vqqjet_eta, weight_of_eachevent);
				hist_tr_vqqjet_phi->Fill(tr_vqqjet_phi, weight_of_eachevent);
				hist_tr_vqqjet_mass->Fill(tr_vqqjet_mass, weight_of_eachevent);
				hist_vqqjets_Mjj->Fill(vqqjets_Mjj, weight_of_eachevent);
				hist_vqqjets_dR->Fill(vqqjets_dR, weight_of_eachevent);

				//  combining the two vqq ak4 jets four vectors
				hist_vqqjets_pt->Fill(vqqjets_pt, weight_of_eachevent);
				hist_vqqjets_eta->Fill(vqqjets_eta, weight_of_eachevent);
				hist_vqqjets_phi->Fill(vqqjets_phi, weight_of_eachevent);
				hist_vqqjets_mass->Fill(vqqjets_mass, weight_of_eachevent);

				hist_bQuarksInHiggsJet->Fill(bQuarksInHiggsJet, weight_of_eachevent);

				hist_ld_w_ld_fermion_pt->Fill(ld_w_ld_fermion_pt, weight_of_eachevent);
				hist_ld_w_ld_fermion_eta->Fill(ld_w_ld_fermion_eta, weight_of_eachevent);
				hist_ld_w_ld_fermion_phi->Fill(ld_w_ld_fermion_phi, weight_of_eachevent);
				hist_ld_w_ld_fermion_mass->Fill(ld_w_ld_fermion_mass, weight_of_eachevent);
				hist_ld_w_tr_fermion_pt->Fill(ld_w_tr_fermion_pt, weight_of_eachevent);
				hist_ld_w_tr_fermion_eta->Fill(ld_w_tr_fermion_eta, weight_of_eachevent);
				hist_ld_w_tr_fermion_phi->Fill(ld_w_tr_fermion_phi, weight_of_eachevent);
				hist_ld_w_tr_fermion_mass->Fill(ld_w_tr_fermion_mass, weight_of_eachevent);
				hist_tr_w_ld_fermion_pt->Fill(tr_w_ld_fermion_pt, weight_of_eachevent);
				hist_tr_w_ld_fermion_eta->Fill(tr_w_ld_fermion_eta, weight_of_eachevent);
				hist_tr_w_ld_fermion_phi->Fill(tr_w_ld_fermion_phi, weight_of_eachevent);
				hist_tr_w_ld_fermion_mass->Fill(tr_w_ld_fermion_mass, weight_of_eachevent);
				hist_tr_w_tr_fermion_pt->Fill(tr_w_tr_fermion_pt, weight_of_eachevent);
				hist_tr_w_tr_fermion_eta->Fill(tr_w_tr_fermion_eta, weight_of_eachevent);
				hist_tr_w_tr_fermion_phi->Fill(tr_w_tr_fermion_phi, weight_of_eachevent);
				hist_tr_w_tr_fermion_mass->Fill(tr_w_tr_fermion_mass, weight_of_eachevent);
				hist_ld_w_ld_fermion_pdgId->Fill(ld_w_ld_fermion_pdgId, weight_of_eachevent);
				hist_ld_w_tr_fermion_pdgId->Fill(ld_w_tr_fermion_pdgId, weight_of_eachevent);
				hist_tr_w_ld_fermion_pdgId->Fill(tr_w_ld_fermion_pdgId, weight_of_eachevent);
				hist_tr_w_tr_fermion_pdgId->Fill(tr_w_tr_fermion_pdgId, weight_of_eachevent);

				// hist_lhe_M_qq->Fill(lhe_M_qq, weight_of_eachevent);

				// gen level
				hist_gen_ld_v_ld_fermion_pt->Fill(gen_ld_v_ld_fermion_pt, weight_of_eachevent);
				hist_gen_ld_v_ld_fermion_eta->Fill(gen_ld_v_ld_fermion_eta, weight_of_eachevent);
				hist_gen_ld_v_ld_fermion_phi->Fill(gen_ld_v_ld_fermion_phi, weight_of_eachevent);
				hist_gen_ld_v_ld_fermion_mass->Fill(gen_ld_v_ld_fermion_mass, weight_of_eachevent);

				hist_gen_ld_v_tr_fermion_pt->Fill(gen_ld_v_tr_fermion_pt, weight_of_eachevent);
				hist_gen_ld_v_tr_fermion_eta->Fill(gen_ld_v_tr_fermion_eta, weight_of_eachevent);
				hist_gen_ld_v_tr_fermion_phi->Fill(gen_ld_v_tr_fermion_phi, weight_of_eachevent);
				hist_gen_ld_v_tr_fermion_mass->Fill(gen_ld_v_tr_fermion_mass, weight_of_eachevent);

				hist_gen_tr_v_ld_fermion_pt->Fill(gen_tr_v_ld_fermion_pt, weight_of_eachevent);
				hist_gen_tr_v_ld_fermion_eta->Fill(gen_tr_v_ld_fermion_eta, weight_of_eachevent);
				hist_gen_tr_v_ld_fermion_phi->Fill(gen_tr_v_ld_fermion_phi, weight_of_eachevent);
				hist_gen_tr_v_ld_fermion_mass->Fill(gen_tr_v_ld_fermion_mass, weight_of_eachevent);

				hist_gen_tr_v_tr_fermion_pt->Fill(gen_tr_v_tr_fermion_pt, weight_of_eachevent);
				hist_gen_tr_v_tr_fermion_eta->Fill(gen_tr_v_tr_fermion_eta, weight_of_eachevent);
				hist_gen_tr_v_tr_fermion_phi->Fill(gen_tr_v_tr_fermion_phi, weight_of_eachevent);
				hist_gen_tr_v_tr_fermion_mass->Fill(gen_tr_v_tr_fermion_mass, weight_of_eachevent);

				hist_gen_ld_v_ld_fermion_pdgId->Fill(gen_ld_v_ld_fermion_pdgId, weight_of_eachevent);
				hist_gen_ld_v_tr_fermion_pdgId->Fill(gen_ld_v_tr_fermion_pdgId, weight_of_eachevent);
				hist_gen_tr_v_ld_fermion_pdgId->Fill(gen_tr_v_ld_fermion_pdgId, weight_of_eachevent);
				hist_gen_tr_v_tr_fermion_pdgId->Fill(gen_tr_v_tr_fermion_pdgId, weight_of_eachevent);

				hist_gen_ld_h_bb_pt->Fill(gen_ld_h_bb_pt, weight_of_eachevent);
				hist_gen_ld_h_bb_eta->Fill(gen_ld_h_bb_eta, weight_of_eachevent);
				hist_gen_ld_h_bb_phi->Fill(gen_ld_h_bb_phi, weight_of_eachevent);
				hist_gen_ld_h_bb_mass->Fill(gen_ld_h_bb_mass, weight_of_eachevent);

				hist_gen_tr_h_bb_pt->Fill(gen_tr_h_bb_pt, weight_of_eachevent);
				hist_gen_tr_h_bb_eta->Fill(gen_tr_h_bb_eta, weight_of_eachevent);
				hist_gen_tr_h_bb_phi->Fill(gen_tr_h_bb_phi, weight_of_eachevent);
				hist_gen_tr_h_bb_mass->Fill(gen_tr_h_bb_mass, weight_of_eachevent);

				hist_gen_ld_h_non_bb_pt->Fill(gen_ld_h_non_bb_pt, weight_of_eachevent);
				hist_gen_ld_h_non_bb_eta->Fill(gen_ld_h_non_bb_eta, weight_of_eachevent);
				hist_gen_ld_h_non_bb_phi->Fill(gen_ld_h_non_bb_phi, weight_of_eachevent);
				hist_gen_ld_h_non_bb_mass->Fill(gen_ld_h_non_bb_mass, weight_of_eachevent);

				hist_gen_tr_h_non_bb_pt->Fill(gen_tr_h_non_bb_pt, weight_of_eachevent);
				hist_gen_tr_h_non_bb_eta->Fill(gen_tr_h_non_bb_eta, weight_of_eachevent);
				hist_gen_tr_h_non_bb_phi->Fill(gen_tr_h_non_bb_phi, weight_of_eachevent);
				hist_gen_tr_h_non_bb_mass->Fill(gen_tr_h_non_bb_mass, weight_of_eachevent);

				hist_gen_ld_h_bb_pdgId->Fill(gen_ld_h_bb_pdgId, weight_of_eachevent);
				hist_gen_tr_h_bb_pdgId->Fill(gen_tr_h_bb_pdgId, weight_of_eachevent);
				hist_gen_ld_h_non_bb_pdgId->Fill(gen_ld_h_non_bb_pdgId, weight_of_eachevent);
				hist_gen_tr_h_non_bb_pdgId->Fill(gen_tr_h_non_bb_pdgId, weight_of_eachevent);

				hist_gen_ld_vbf_quarks_pt->Fill(gen_ld_vbf_quarks_pt, weight_of_eachevent);
				hist_gen_ld_vbf_quarks_eta->Fill(gen_ld_vbf_quarks_eta, weight_of_eachevent);
				hist_gen_ld_vbf_quarks_phi->Fill(gen_ld_vbf_quarks_phi, weight_of_eachevent);
				hist_gen_ld_vbf_quarks_mass->Fill(gen_ld_vbf_quarks_mass, weight_of_eachevent);

				hist_gen_tr_vbf_quarks_pt->Fill(gen_tr_vbf_quarks_pt, weight_of_eachevent);
				hist_gen_tr_vbf_quarks_eta->Fill(gen_tr_vbf_quarks_eta, weight_of_eachevent);
				hist_gen_tr_vbf_quarks_phi->Fill(gen_tr_vbf_quarks_phi, weight_of_eachevent);
				hist_gen_tr_vbf_quarks_mass->Fill(gen_tr_vbf_quarks_mass, weight_of_eachevent);

				// for the addition of each pair of fermions
				// Fill histograms for gen_ld_w series
				hist_gen_ld_v_mass->Fill(gen_ld_v_mass, weight_of_eachevent);
				hist_gen_ld_v_pt->Fill(gen_ld_v_pt, weight_of_eachevent);
				hist_gen_ld_v_eta->Fill(gen_ld_v_eta, weight_of_eachevent);
				hist_gen_ld_v_phi->Fill(gen_ld_v_phi, weight_of_eachevent);

				// Fill histograms for gen_tr_v series
				hist_gen_tr_v_mass->Fill(gen_tr_v_mass, weight_of_eachevent);
				hist_gen_tr_v_pt->Fill(gen_tr_v_pt, weight_of_eachevent);
				hist_gen_tr_v_eta->Fill(gen_tr_v_eta, weight_of_eachevent);
				hist_gen_tr_v_phi->Fill(gen_tr_v_phi, weight_of_eachevent);

				// Fill histograms for gen_h_bb series
				hist_gen_h_bb_mass->Fill(gen_h_bb_mass, weight_of_eachevent);
				hist_gen_h_bb_pt->Fill(gen_h_bb_pt, weight_of_eachevent);
				hist_gen_h_bb_eta->Fill(gen_h_bb_eta, weight_of_eachevent);
				hist_gen_h_bb_phi->Fill(gen_h_bb_phi, weight_of_eachevent);

				// Fill histograms for gen_h_non_bb series
				hist_gen_h_non_bb_mass->Fill(gen_h_non_bb_mass, weight_of_eachevent);
				hist_gen_h_non_bb_pt->Fill(gen_h_non_bb_pt, weight_of_eachevent);
				hist_gen_h_non_bb_eta->Fill(gen_h_non_bb_eta, weight_of_eachevent);
				hist_gen_h_non_bb_phi->Fill(gen_h_non_bb_phi, weight_of_eachevent);

				// Fill histograms for dR and other variables
				hist_gen_tr_v_dR_fermmions->Fill(gen_tr_v_dR_fermmions, weight_of_eachevent);
				hist_gen_ld_v_dR_fermmions->Fill(gen_ld_v_dR_fermmions, weight_of_eachevent);
				hist_gen_h_bb_dR->Fill(gen_h_bb_dR, weight_of_eachevent);
				hist_gen_h_non_bb_dR->Fill(gen_h_non_bb_dR, weight_of_eachevent);

				// Fill histograms for gen_vbf_quarks series
				hist_gen_vbf_quarks_deltaphi->Fill(gen_vbf_quarks_deltaphi, weight_of_eachevent);
				hist_gen_vbf_quarks_deltaeta->Fill(gen_vbf_quarks_deltaeta, weight_of_eachevent);
				hist_gen_vbf_quarks_dijetmass->Fill(gen_vbf_quarks_dijetmass, weight_of_eachevent);

				hist_v1_pdgid->Fill(v1_pdgid, weight_of_eachevent);
				hist_v2_pdgid->Fill(v2_pdgid, weight_of_eachevent);

				hist_hbbfatjet_xbb->Fill(hbbfatjet_xbb, weight_of_eachevent);
				hist_ld_vqqfatjet_xwqq->Fill(ld_vqqfatjet_xwqq, weight_of_eachevent);
				hist_ld_vqqfatjet_xvqq->Fill(ld_vqqfatjet_xvqq, weight_of_eachevent);

				hist_M_VVH_semimerged->Fill(M_VVH_semimerged, weight_of_eachevent);

				hist_abcdnet_score->Fill(abcdnet_score, weight_of_eachevent);
				hist_2D->Fill(M_jj,abcdnet_score, weight_of_eachevent);
				hist_2D_flipped->Fill(abcdnet_score, M_jj, weight_of_eachevent);
				h->Fill(0.5, weight_of_eachevent); // Fill the single bin with the event weight
				hist_HT_fat->Fill(HT_fat, weight_of_eachevent);
				// if (HT_fat < 1500) {
				// 	cout << "Entry " << i1 << ": HT_fat = " << HT_fat 
				// 		<< ", Weight = " << weight_of_eachevent << endl;
				// }

				// cout << "weight_of_eachevent:" << weight_of_eachevent << endl;
				// cout << "abcdnet_score_of_eachevent:" << abcdnet_score << endl;
				// cout << "abseta_of_eachevent:" << abs_deta_jj << endl;
			}

		}

		std::vector<TH1D*> histograms = {hist_regionA, hist_regionB, hist_regionC, hist_regionD, hist_regionD1, hist_regionD2, hist_regionD3, hist_regionD4, hist_regionC1, hist_regionC2, hist_regionC3, hist_regionC4, hist_regionB1, hist_regionB2, hist_regionD1, hist_regionD2 }; // List of all your histograms corresponding to each region
		updateCsvForRegion(outputFilename, datasetColumns, filename, histograms, regions);



		f1->Close();
		float maxdeltas_muF=0;
		maxdeltas_muF = std::max(std::abs(1-deltaup_muF/h->GetBinContent(1)),std::abs(1-deltadown_muF/h->GetBinContent(1)));
		cout << "deltaup_muF:" << deltaup_muF <<endl;
		cout << "deltadown_muF:" << deltadown_muF <<endl;
		cout << "CMS_LHE_weights_scale_muF_vbsvvh:" << 1 + maxdeltas_muF <<endl;

		float maxdeltas_isr=0;
		maxdeltas_isr = std::max(std::abs(1-deltaup_isr/h->GetBinContent(1)),std::abs(1-deltadown_isr/h->GetBinContent(1)));
		cout << "deltaup_isr:" << deltaup_isr <<endl;
		cout << "deltadown_isr:" << deltadown_isr <<endl;
		cout << "CMS_PSWeight_ISR_vbsvvh:" << 1 + maxdeltas_isr <<endl;

		float maxdeltas_fsr=0;
		maxdeltas_fsr = std::max(std::abs(1-deltaup_fsr/h->GetBinContent(1)),std::abs(1-deltadown_fsr/h->GetBinContent(1)));
		cout << "deltaup_fsr:" << deltaup_fsr <<endl;
		cout << "deltadown_fsr:" << deltadown_fsr <<endl;
		cout << "CMS_PSWeight_FSR_vbsvvh:" << 1 + maxdeltas_fsr <<endl;
	}

	cout << "number of events:" << no_events << endl;
	cout << "number of eventsD:" << no_eventsD << endl;
	// cout << "number of events weighted:" << no_events_weighted << endl;
	// cout << "error of events weighted:" << no_events_weighted *(sqrt(no_events)/no_events)<<endl << endl;
	std::vector<TH1D*> histograms = {
	    hist_regionA, hist_regionB, hist_regionC, hist_regionD,
	    hist_regionD1, hist_regionD2, hist_regionD3, hist_regionD4,
	    hist_regionC1, hist_regionC2, hist_regionC3, hist_regionC4,
	    hist_regionB1, hist_regionB2
	};
	for (size_t x = 0; x < histograms.size(); ++x) {
	    std::cout << regions[x] << " - Total number of weighted events: " << histograms[x]->GetBinContent(1);
	    std::cout << ", Error of total weighted events: " << histograms[x]->GetBinError(1) << std::endl;
	}
	cout << "Total number of weighted events: " << h->GetBinContent(1) << endl;
	cout << "Error of total weighted events: " << h->GetBinError(1) << endl;

	// for (int i1 = 0; i1 < input_files.size(); i1++) {
	// 	cout << input_files[i1] <<endl;
	// }

	TProfile* pfx = hist_2D->ProfileX();
	pfx->SetMarkerStyle(20);
	pfx->SetMarkerColor(kBlack);

	// TProfile* pfy = hist_2D_flipped->ProfileY();
	// pfy->SetMarkerStyle(20);
	// pfy->SetMarkerColor(kBlack);

	f_output->cd();
	hist_HT->Write();
	hist_n_jets->Write();
	hist_n_loose_b_jets->Write();
	hist_n_medium_b_jets->Write();
	hist_n_tight_b_jets->Write();
	hist_n_fatjets->Write();
	hist_ld_vbsjet_pt->Write();
	hist_ld_vbsjet_eta->Write();
	hist_ld_vbsjet_phi->Write();
	hist_tr_vbsjet_pt->Write();
	hist_tr_vbsjet_eta->Write();
	hist_tr_vbsjet_phi->Write();
	hist_M_jj->Write();
	hist_M_jj_0_250->Write();
	hist_deta_jj->Write();
	hist_abs_deta_jj->Write();
	hist_MET->Write();
	// hist_ld_vqqfatjet_score->Write();
	hist_ld_vqqfatjet_pt->Write();
	hist_ld_vqqfatjet_eta->Write();
	hist_ld_vqqfatjet_phi->Write();
	hist_ld_vqqfatjet_mass->Write();
	hist_ld_vqqfatjet_msoftdrop->Write();
	// hist_tr_vqqfatjet_score->Write();
	// hist_tr_vqqfatjet_pt->Write();
	// hist_tr_vqqfatjet_eta->Write();
	// hist_tr_vqqfatjet_phi->Write();
	// hist_tr_vqqfatjet_mass->Write();
	// hist_tr_vqqfatjet_msoftdrop->Write();
	// hist_hbbfatjet_score->Write();
	hist_hbbfatjet_pt->Write();
	hist_hbbfatjet_eta->Write();
	hist_hbbfatjet_phi->Write();
	hist_hbbfatjet_mass->Write();
	hist_hbbfatjet_msoftdrop->Write();
	hist_ST->Write();


	hist_ld_vqqjet_pt->Write();
	hist_ld_vqqjet_eta->Write();
	hist_ld_vqqjet_phi->Write();
	hist_ld_vqqjet_mass->Write();
	hist_tr_vqqjet_pt->Write();
	hist_tr_vqqjet_eta->Write();
	hist_tr_vqqjet_phi->Write();
	hist_tr_vqqjet_mass->Write();
	hist_vqqjets_Mjj->Write();
	hist_vqqjets_dR->Write();

	//  combining the two vqq ak4 jets four vectors
	hist_vqqjets_pt->Write();
	hist_vqqjets_eta->Write();
	hist_vqqjets_phi->Write();
	hist_vqqjets_mass->Write();

	hist_bQuarksInHiggsJet->Write();

	// hist_ld_w_ld_fermion_pt->Write();
	// hist_ld_w_ld_fermion_eta->Write();
	// hist_ld_w_ld_fermion_phi->Write();
	// hist_ld_w_ld_fermion_mass->Write();
	// hist_ld_w_tr_fermion_pt->Write();
	// hist_ld_w_tr_fermion_eta->Write();
	// hist_ld_w_tr_fermion_phi->Write();
	// hist_ld_w_tr_fermion_mass->Write();
	// hist_tr_w_ld_fermion_pt->Write();
	// hist_tr_w_ld_fermion_eta->Write();
	// hist_tr_w_ld_fermion_phi->Write();
	// hist_tr_w_ld_fermion_mass->Write();
	// hist_tr_w_tr_fermion_pt->Write();
	// hist_tr_w_tr_fermion_eta->Write();
	// hist_tr_w_tr_fermion_phi->Write();
	// hist_tr_w_tr_fermion_mass->Write();
	// hist_ld_w_ld_fermion_pdgId->Write();
	// hist_ld_w_tr_fermion_pdgId->Write();
	// hist_tr_w_ld_fermion_pdgId->Write();
	// hist_tr_w_tr_fermion_pdgId->Write();

	// hist_lhe_M_qq->Write();

	// gen level

	// hist_gen_ld_v_ld_fermion_pt->Write();
	// hist_gen_ld_v_ld_fermion_eta->Write();
	// hist_gen_ld_v_ld_fermion_phi->Write();
	// hist_gen_ld_v_ld_fermion_mass->Write();

	// hist_gen_ld_v_tr_fermion_pt->Write();
	// hist_gen_ld_v_tr_fermion_eta->Write();
	// hist_gen_ld_v_tr_fermion_phi->Write();
	// hist_gen_ld_v_tr_fermion_mass->Write();

	// hist_gen_tr_v_ld_fermion_pt->Write();
	// hist_gen_tr_v_ld_fermion_eta->Write();
	// hist_gen_tr_v_ld_fermion_phi->Write();
	// hist_gen_tr_v_ld_fermion_mass->Write();

	// hist_gen_tr_v_tr_fermion_pt->Write();
	// hist_gen_tr_v_tr_fermion_eta->Write();
	// hist_gen_tr_v_tr_fermion_phi->Write();
	// hist_gen_tr_v_tr_fermion_mass->Write();

	// hist_gen_ld_v_ld_fermion_pdgId->Write();
	// hist_gen_ld_v_tr_fermion_pdgId->Write();
	// hist_gen_tr_v_ld_fermion_pdgId->Write();
	// hist_gen_tr_v_tr_fermion_pdgId->Write();

	// hist_gen_ld_h_bb_pt->Write();
	// hist_gen_ld_h_bb_eta->Write();
	// hist_gen_ld_h_bb_phi->Write();
	// hist_gen_ld_h_bb_mass->Write();

	// hist_gen_tr_h_bb_pt->Write();
	// hist_gen_tr_h_bb_eta->Write();
	// hist_gen_tr_h_bb_phi->Write();
	// hist_gen_tr_h_bb_mass->Write();

	// hist_gen_ld_h_non_bb_pt->Write();
	// hist_gen_ld_h_non_bb_eta->Write();
	// hist_gen_ld_h_non_bb_phi->Write();
	// hist_gen_ld_h_non_bb_mass->Write();

	// hist_gen_tr_h_non_bb_pt->Write();
	// hist_gen_tr_h_non_bb_eta->Write();
	// hist_gen_tr_h_non_bb_phi->Write();
	// hist_gen_tr_h_non_bb_mass->Write();

	// hist_gen_ld_h_bb_pdgId->Write();
	// hist_gen_tr_h_bb_pdgId->Write();
	// hist_gen_ld_h_non_bb_pdgId->Write();
	// hist_gen_tr_h_non_bb_pdgId->Write();

	// hist_gen_ld_vbf_quarks_pt->Write();
	// hist_gen_ld_vbf_quarks_eta->Write();
	// hist_gen_ld_vbf_quarks_phi->Write();
	// hist_gen_ld_vbf_quarks_mass->Write();

	// hist_gen_tr_vbf_quarks_pt->Write();
	// hist_gen_tr_vbf_quarks_eta->Write();
	// hist_gen_tr_vbf_quarks_phi->Write();
	// hist_gen_tr_vbf_quarks_mass->Write();

	// // for the addition of each pair of fermions
	// // Write histograms for gen_ld_v series
	// hist_gen_ld_v_mass->Write();
	// hist_gen_ld_v_pt->Write();
	// hist_gen_ld_v_eta->Write();
	// hist_gen_ld_v_phi->Write();

	// // Write histograms for gen_tr_v series
	// hist_gen_tr_v_mass->Write();
	// hist_gen_tr_v_pt->Write();
	// hist_gen_tr_v_eta->Write();
	// hist_gen_tr_v_phi->Write();

	// // Write histograms for gen_h_bb series
	// hist_gen_h_bb_mass->Write();
	// hist_gen_h_bb_pt->Write();
	// hist_gen_h_bb_eta->Write();
	// hist_gen_h_bb_phi->Write();

	// // Write histograms for gen_h_non_bb series
	// hist_gen_h_non_bb_mass->Write();
	// hist_gen_h_non_bb_pt->Write();
	// hist_gen_h_non_bb_eta->Write();
	// hist_gen_h_non_bb_phi->Write();

	// // Write histograms for dR and other variables
	// hist_gen_tr_v_dR_fermmions->Write();
	// hist_gen_ld_v_dR_fermmions->Write();
	// hist_gen_h_bb_dR->Write();
	// hist_gen_h_non_bb_dR->Write();

	// // Write histograms for gen_vbf_quarks series
	// hist_gen_vbf_quarks_deltaphi->Write();
	// hist_gen_vbf_quarks_deltaeta->Write();
	// hist_gen_vbf_quarks_dijetmass->Write();

	// hist_v1_pdgid->Write();
	// hist_v2_pdgid->Write();

	hist_hbbfatjet_xbb->Write();
	hist_ld_vqqfatjet_xwqq->Write();
	hist_ld_vqqfatjet_xvqq->Write();

	hist_M_VVH_semimerged->Write();

	hist_abcdnet_score->Write();
	h->Write();
    hist_abcdnet_fixedEta->Write();
    hist_eta_fixedABCDNet->Write();

	hist_HT_fat->Write();
//-------------------------------------------------------------------------------

// 	hist_2D->Write();
// 	pfx->Write("profileX");

// 	TCanvas* c2 = new TCanvas("c2", "2D Correlation with Profile", 800, 600);
// 	c2->SetFillColor(kWhite);
// 	hist_2D->Draw("COLZ"); // Draw the 2D histogram with color scale
// 	hist_2D->GetXaxis()->SetTitle("Mjj");
// 	hist_2D->GetYaxis()->SetTitle("ABCDNet score");
// 	hist_2D->GetXaxis()->SetTitleSize(0.046);
// 	hist_2D->GetYaxis()->SetTitleSize(0.046);
// // And similarly for label sizes

// 	pfx->Draw("same"); // Draw the profile on the same canvas
// 	hist_2D->SetStats(kFALSE);
// 	c2->SaveAs("correlation_with_profile.png"); // Save the canvas to a file
// 	delete c2;


	// TCanvas* c2_flipped = new TCanvas("c2_flipped", "2D Correlation with Profile Flipped", 800, 600);
	// c2_flipped->SetFillColor(kWhite);
	// hist_2D_flipped->Draw("COLZ"); // Draw the 2D histogram with color scale
	// hist_2D_flipped->GetYaxis()->SetTitle("Mjj");
	// hist_2D_flipped->GetXaxis()->SetTitle("ABCDNet score");
	// hist_2D_flipped->GetYaxis()->SetTitleSize(0.04);
	// hist_2D_flipped->GetXaxis()->SetTitleSize(0.04);
	// // And similarly for label sizes
	
	// pfy->Draw("same"); // Draw the profile on the same canvas
	// hist_2D_flipped->SetStats(kFALSE);
	// c2_flipped->SaveAs("correlation_with_profile_flipped.png"); // Save the canvas to a file
	// delete c2_flipped;

	//-------------------------------------------------------------------------------------------

    hist_abcdnet_fixedEta->Draw();
    hist_eta_fixedABCDNet->Draw();  // "same" to overlay, remove it to draw separately


    f_output->Close();



}
