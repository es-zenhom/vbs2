// myProgram.cpp

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

// ROOT includes
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TLorentzVector.h>

int main(int argc, char** argv) {
    // Check for correct number of arguments
    if (argc < 2) {
        std::cout << "Usage: ./myProgram <input_root_file> [<output_root_file>]" << std::endl;
        return 1;
    }

    std::string inputFileName = argv[1];
    std::string outputFileName = "output_histograms.root";
    if (argc >= 3) {
        outputFileName = argv[2];
    }

    // Open the input ROOT file
    TFile* inputFile = TFile::Open(inputFileName.c_str(), "READ");
    if (!inputFile || inputFile->IsZombie()) {
        std::cerr << "Error: Cannot open input file " << inputFileName << std::endl;
        return 1;
    }

    // Get the TTree from the file
    TTree* tree = dynamic_cast<TTree*>(inputFile->Get("Events"));
    if (!tree) {
        std::cerr << "Error: Cannot find TTree 'Events' in file " << inputFileName << std::endl;
        inputFile->Close();
        return 1;
    }

    // Set up variables to read branches
    UInt_t nFatJet;
    std::vector<Float_t>* FatJet_pt = nullptr;
    std::vector<Float_t>* FatJet_eta = nullptr;
    std::vector<Float_t>* FatJet_phi = nullptr;

    UInt_t nJet;
    std::vector<Float_t>* Jet_pt = nullptr;
    std::vector<Float_t>* Jet_eta = nullptr;
    std::vector<Float_t>* Jet_phi = nullptr;

    UInt_t nGenPart;
    std::vector<Int_t>* GenPart_pdgId = nullptr;
    std::vector<Float_t>* GenPart_pt = nullptr;
    std::vector<Float_t>* GenPart_eta = nullptr;
    std::vector<Float_t>* GenPart_phi = nullptr;
    std::vector<Int_t>* GenPart_genPartIdxMother = nullptr;
    std::vector<Int_t>* GenPart_status = nullptr;

    // Set branch addresses for FatJets (AK8 jets)
    tree->SetBranchAddress("nFatJet", &nFatJet);
    tree->SetBranchAddress("FatJet_pt", &FatJet_pt);
    tree->SetBranchAddress("FatJet_eta", &FatJet_eta);
    tree->SetBranchAddress("FatJet_phi", &FatJet_phi);

    // Set branch addresses for AK4 Jets
    tree->SetBranchAddress("nJet", &nJet);
    tree->SetBranchAddress("Jet_pt", &Jet_pt);
    tree->SetBranchAddress("Jet_eta", &Jet_eta);
    tree->SetBranchAddress("Jet_phi", &Jet_phi);

    // Set branch addresses for GenParticles
    tree->SetBranchAddress("nGenPart", &nGenPart);
    tree->SetBranchAddress("GenPart_pdgId", &GenPart_pdgId);
    tree->SetBranchAddress("GenPart_pt", &GenPart_pt);
    tree->SetBranchAddress("GenPart_eta", &GenPart_eta);
    tree->SetBranchAddress("GenPart_phi", &GenPart_phi);
    tree->SetBranchAddress("GenPart_genPartIdxMother", &GenPart_genPartIdxMother);
    tree->SetBranchAddress("GenPart_status", &GenPart_status);

    // Create histograms
    TH1D* h_dR_genQuarks = new TH1D("h_dR_genQuarks", "Delta R between Gen-Level Quarks from Subleading V;#Delta R;Events", 50, 0, 5);
    TH1D* h_dR_recoJets = new TH1D("h_dR_recoJets", "Delta R between Reco-Level AK4 Jets Assigned to Subleading V;#Delta R;Events", 50, 0, 5);

    // Loop over events
    Long64_t nEntries = tree->GetEntries();
    for (Long64_t entry = 0; entry < nEntries; ++entry) {
        tree->GetEntry(entry);

        // Progress indicator
        if (entry % 10000 == 0) {
            std::cout << "Processing event " << entry << " / " << nEntries << std::endl;
        }

        // Event selection: exactly 2 fat jets (AK8 jets)
        if (nFatJet != 2) continue;

        // Structure to hold vector bosons
        struct Boson {
            int index;      // Index in GenPart array
            float pt;
            float eta;
            float phi;
            int pdgId;
        };
        std::vector<Boson> vectorBosons;

        // Identify vector bosons at gen level
        for (unsigned int i = 0; i < nGenPart; ++i) {
            int pdgId = GenPart_pdgId->at(i);
            // Check for W and Z bosons
            if (abs(pdgId) == 23 || abs(pdgId) == 24) {
                // Optional: Check for final state bosons
                Boson boson;
                boson.index = i;
                boson.pt = GenPart_pt->at(i);
                boson.eta = GenPart_eta->at(i);
                boson.phi = GenPart_phi->at(i);
                boson.pdgId = pdgId;
                vectorBosons.push_back(boson);
            }
        }

        // Require at least two vector bosons
        if (vectorBosons.size() < 2) continue;

        // Sort the vector bosons by pt to find the subleading V boson (second highest pt)
        std::sort(vectorBosons.begin(), vectorBosons.end(), [](const Boson& a, const Boson& b) {
            return a.pt > b.pt;
        });

        Boson subleadingV = vectorBosons[1]; // Second highest pt

        // Check that the subleading V has pt < 350 GeV
        if (subleadingV.pt >= 350) continue;

        // Find decay products of the subleading V
        std::vector<int> decayProductIndices;
        for (unsigned int i = 0; i < nGenPart; ++i) {
            if (GenPart_genPartIdxMother->at(i) == subleadingV.index) {
                decayProductIndices.push_back(i);
            }
        }

        // Collect quark decay products
        std::vector<int> decayQuarkIndices;
        bool hasLeptonicDecay = false;
        for (int idx : decayProductIndices) {
            int pdgId = GenPart_pdgId->at(idx);
            int absPdgId = abs(pdgId);
            // Quarks have PDG IDs 1-5
            if (absPdgId >= 1 && absPdgId <= 5) {
                decayQuarkIndices.push_back(idx);
            } else if (absPdgId == 11 || absPdgId == 13 || absPdgId == 15) {
                // Electron, muon, tau
                hasLeptonicDecay = true;
                break;
            }
        }

        // Exclude events where the subleading V decays leptonically
        if (hasLeptonicDecay) continue;

        // Require exactly two quark decay products
        if (decayQuarkIndices.size() != 2) continue;

        // Get quark momenta
        TLorentzVector quark1, quark2;
        quark1.SetPtEtaPhiM(
            GenPart_pt->at(decayQuarkIndices[0]),
            GenPart_eta->at(decayQuarkIndices[0]),
            GenPart_phi->at(decayQuarkIndices[0]),
            0); // Massless approximation
        quark2.SetPtEtaPhiM(
            GenPart_pt->at(decayQuarkIndices[1]),
            GenPart_eta->at(decayQuarkIndices[1]),
            GenPart_phi->at(decayQuarkIndices[1]),
            0);

        // Calculate Delta R between quarks
        double deltaR_gen = quark1.DeltaR(quark2);
        h_dR_genQuarks->Fill(deltaR_gen);

        // Reco-level selection of two AK4 jets assigned to the subleading V
        // Use your method based on Delta R
        // First, build a collection of AK4 jets
        std::vector<TLorentzVector> ak4Jets;
        for (unsigned int i = 0; i < nJet; ++i) {
            TLorentzVector jet;
            jet.SetPtEtaPhiM(Jet_pt->at(i), Jet_eta->at(i), Jet_phi->at(i), 0); // Assume massless for simplicity
            ak4Jets.push_back(jet);
        }

        // Now, select the pair of AK4 jets that are closest in Delta R
        // Alternatively, apply your specific selection criteria here

        // Find the pair of AK4 jets with minimal Delta R between them
        double minDeltaR = 9999;
        int idxJet1 = -1;
        int idxJet2 = -1;
        for (unsigned int i = 0; i < ak4Jets.size(); ++i) {
            for (unsigned int j = i + 1; j < ak4Jets.size(); ++j) {
                double deltaR = ak4Jets[i].DeltaR(ak4Jets[j]);
                if (deltaR < minDeltaR) {
                    minDeltaR = deltaR;
                    idxJet1 = i;
                    idxJet2 = j;
                }
            }
        }

        // Ensure that we have found a valid pair
        if (idxJet1 == -1 || idxJet2 == -1) continue;

        // Use your actual selection criteria here
        // For example, you might select jets based on proximity to the subleading V

        // Build the two selected jets
        TLorentzVector jet1 = ak4Jets[idxJet1];
        TLorentzVector jet2 = ak4Jets[idxJet2];

        // Calculate Delta R between reco-level jets
        double deltaR_reco = jet1.DeltaR(jet2);
        h_dR_recoJets->Fill(deltaR_reco);

        // Optionally, you might match reco jets to gen quarks
        // For more precise comparison, you could compute Delta R between reco jets and gen quarks
    }

    // Save histograms to output file
    TFile* outputFile = new TFile(outputFileName.c_str(), "RECREATE");
    h_dR_genQuarks->Write();
    h_dR_recoJets->Write();
    outputFile->Close();

    // Clean up
    inputFile->Close();
    delete inputFile;
    delete outputFile;

    std::cout << "Processing completed. Histograms saved to " << outputFileName << std::endl;

    return 0;
}
