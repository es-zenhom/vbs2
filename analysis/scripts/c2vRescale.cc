/****************************************************************************
  c2vRescaleClone_debug.cc

  A verbose, debug-heavy version of the "c2vRescaleClone" script.
  It:
    1) Prints all CLI arguments + the entire reweight list.
    2) Opens the "nominal" file => prints f_nom->ls(), tries to find "rwgt_tree"
       => prints TTree->Print()
    3) Opens the "inference" file => prints f_inf->ls(), tries to find "tree"
       => prints TTree->Print()
    4) Clones the inference TTree, locates a "xsec_sf" branch to override
       with a scaled version from reweightsPtr->at(reweightIndex)
    5) Full debug output in the event loop: prints i up to 20, then every 10k

  Usage example:
    ./c2vRescaleClone_debug \
      --inference /path/to/inferences/VBSVVH.root \
      --original  /path/to/Run2/VBSVVH.root \
      --reweight-list data/VBSVVH_reweights.txt \
      --reweight-name scan_CV_1p0_C2V_1p5_C3_1p0 \
      --out VBSVVH_c2v1p5.root
****************************************************************************/

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm> // for std::find
#include <cstdlib>   // for EXIT_SUCCESS/FAILURE

// Simple command-line option parser
static std::string getCmdOption(char** begin, char** end, const std::string &option) {
    char** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end) {
        return std::string(*itr);
    }
    return "";
}
static bool cmdOptionExists(char** begin, char** end, const std::string& option) {
    return std::find(begin, end, option) != end;
}

int main(int argc, char* argv[]){
    std::cout << "\n[DEBUG] Starting c2vRescaleClone_debug\n";
    // 1) Parse command-line
    if (argc<2 || cmdOptionExists(argv, argv+argc, "--help")){
        std::cout << "Usage: " << argv[0] << "\n"
                  << "  --inference      <path/to/inferred_signal.root>\n"
                  << "  --original       <path/to/nominal_signal.root>\n"
                  << "  --reweight-list  <path/to/VBSVVH_reweights.txt>\n"
                  << "  --reweight-name  <scan_CV_1p0_C2V_1p5_C3_1p0>\n"
                  << "  --out            <output.root>\n"
                  << "\nExample:\n"
                  << argv[0] << " --inference inferences/VBSVVH.root --original Run2/VBSVVH.root "
                  << "--reweight-list data/VBSVVH_reweights.txt "
                  << "--reweight-name scan_CV_1p0_C2V_1p5_C3_1p0 "
                  << "--out VBSVVH_c2v1p5.root\n";
        return EXIT_FAILURE;
    }

    // Print all CLI args
    std::cout << "[DEBUG] Command-line args:\n";
    for (int i=0; i<argc; i++){
        std::cout << " arg["<< i <<"] = '" << argv[i] << "'\n";
    }

    std::string inferenceFile  = getCmdOption(argv, argv+argc, "--inference");
    std::string originalFile   = getCmdOption(argv, argv+argc, "--original");
    std::string reweightList   = getCmdOption(argv, argv+argc, "--reweight-list");
    std::string reweightName   = getCmdOption(argv, argv+argc, "--reweight-name");
    std::string outFileName    = getCmdOption(argv, argv+argc, "--out");

    std::cout << "[DEBUG] inferenceFile  = " << inferenceFile  << "\n";
    std::cout << "[DEBUG] originalFile   = " << originalFile   << "\n";
    std::cout << "[DEBUG] reweightList   = " << reweightList   << "\n";
    std::cout << "[DEBUG] reweightName   = " << reweightName   << "\n";
    std::cout << "[DEBUG] outFileName    = " << outFileName    << "\n";

    if(inferenceFile.empty() || originalFile.empty() || reweightList.empty()
       || reweightName.empty() || outFileName.empty()){
        std::cerr << "[ERROR] Missing required arguments. Use --help for usage.\n";
        return EXIT_FAILURE;
    }

    // 2) Load reweight_names from text file
    std::vector<std::string> reweightNames;
    {
        std::ifstream fin(reweightList);
        if(!fin.good()){
            std::cerr << "Cannot open reweight-list: " << reweightList << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "[DEBUG] Successfully opened reweight list: " << reweightList << "\n";
        std::string line;
        while(std::getline(fin, line)){
            if(!line.empty()){
                reweightNames.push_back(line);
            }
        }
        fin.close();
    }
    std::cout << "[DEBUG] Found " << reweightNames.size() << " lines in " << reweightList << ":\n";
    for (size_t i=0; i<reweightNames.size(); i++){
        std::cout << "   line["<< i << "] = '" << reweightNames[i] << "'\n";
    }

    // find index
    int reweightIndex = -1;
    for(size_t i=0; i<reweightNames.size(); i++){
        if(reweightNames[i] == reweightName){
            reweightIndex = i;
            break;
        }
    }
    if(reweightIndex<0){
        std::cerr << "[ERROR] Could not find reweightName '"<< reweightName
                  <<"' in file "<< reweightList << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "[INFO] reweightIndex=" << reweightIndex
              << " for '" << reweightName << "'" << std::endl;

    // 3) Open nominal => rwgt_tree
    std::cout << "[DEBUG] Opening nominal file: " << originalFile << "\n";
    TFile *f_nom = TFile::Open(originalFile.c_str(),"READ");
    if(!f_nom || f_nom->IsZombie()){
        std::cerr << "[ERROR] Could not open " << originalFile << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "[DEBUG] f_nom->ls():\n";
    f_nom->ls(); // Print all keys
    TTree *trw = (TTree*)f_nom->Get("rwgt_tree");
    if(!trw){
        std::cerr << "[ERROR] No 'rwgt_tree' in " << originalFile << std::endl;
        return EXIT_FAILURE;
    }
    // std::cout << "[DEBUG] trw->Print():\n";
    // trw->Print();

    std::vector<double> *reweightsPtr = nullptr;
    trw->SetBranchAddress("reweights", &reweightsPtr);
    std::cout << "[DEBUG] SetBranchAddress for reweights done.\n";

    // 4) Open inference => "tree"
    std::cout << "[DEBUG] Opening inference file: " << inferenceFile << "\n";
    TFile *f_inf = TFile::Open(inferenceFile.c_str(),"READ");
    if(!f_inf || f_inf->IsZombie()){
        std::cerr << "[ERROR] Could not open " << inferenceFile << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "[DEBUG] f_inf->ls():\n";
    f_inf->ls(); // Print all keys
    TTree *tin = (TTree*)f_inf->Get("tree");
    if(!tin){
        std::cerr << "[ERROR] No 'tree' in " << inferenceFile << std::endl;
        return EXIT_FAILURE;
    }
    // std::cout << "[DEBUG] tin->Print():\n";
    // tin->Print();

    // 5) Create output => new TTree with the same structure
    std::cout << "[DEBUG] Creating output file: " << outFileName << "\n";
    TFile *f_out = TFile::Open(outFileName.c_str(), "RECREATE");
    if(!f_out || f_out->IsZombie()){
        std::cerr << "[ERROR] Could not create output file " << outFileName << std::endl;
        return EXIT_FAILURE;
    }

    // Clone the structure but not the contents
    std::cout << "[DEBUG] Cloning TTree structure with CloneTree(0)...\n";
    TTree *tout = tin->CloneTree(0);  // copy structure only
    if(!tout){
        std::cerr << "[ERROR] CloneTree(0) failed.\n";
        return EXIT_FAILURE;
    }
    // We'll read xsec_sf_in from tin
    double xsec_sf_in = 0.f;
    TBranch *b_in = tin->GetBranch("xsec_sf");
    if(!b_in){
        std::cerr << "[ERROR] 'xsec_sf' branch not found in inference TTree.\n";
        return EXIT_FAILURE;
    }
    std::cout << "[DEBUG] Found xsec_sf branch in tin. Setting address...\n";
    b_in->SetAddress(&xsec_sf_in);

    // We'll override the address in tout for the same branch
    double xsec_sf_out = 0.f;
    TBranch *b_out = tout->GetBranch("xsec_sf");
    if(!b_out){
        std::cerr << "[ERROR] 'xsec_sf' branch not found in cloned TTree.\n";
        return EXIT_FAILURE;
    }
    b_out->SetAddress(&xsec_sf_out);
    std::cout << "[DEBUG] Overrode xsec_sf branch in tout.\n";

    // 6) Event loop
    Long64_t nEvtInf = tin->GetEntries();
    Long64_t nEvtNom = trw->GetEntries();
    std::cout << "[INFO] Inference tree entries = " << nEvtInf
              << ", rwgt_tree entries = " << nEvtNom << std::endl;

    Long64_t nMin = (nEvtInf < nEvtNom) ? nEvtInf : nEvtNom;
    std::cout << "[INFO] Will loop over nMin=" << nMin << " events.\n";

    for(Long64_t i=0; i<nMin; i++){
        // Print progress
        if(i<20 || (i%10000==0)){
            std::cout << "[DEBUG loop] i=" << i << " out of " << nMin << std::endl;
        }

        // Get entries
        Long64_t nbInf = tin->GetEntry(i);
        if(nbInf<=0){
            std::cerr << "[WARN] tin->GetEntry("<< i <<") returned " << nbInf << ", break.\n";
            break;
        }

        Long64_t nbNom = trw->GetEntry(i);
        if(nbNom<=0){
            std::cerr << "[WARN] trw->GetEntry("<< i <<") returned " << nbNom << ", break.\n";
            break;
        }

        // Check pointer
        if(!reweightsPtr){
            std::cerr << "[WARN] reweightsPtr is null at i="<< i <<", skipping.\n";
            continue;
        }

        // check size
        size_t vecSize = reweightsPtr->size();
        if(vecSize <= (size_t)reweightIndex){
            // shouldn't happen unless reweights is smaller than expected
            std::cerr << "[WARN] reweightIndex="<< reweightIndex
                      << " out of range at i="<< i
                      <<". reweightsPtr->size()=" << vecSize <<"\n";
            continue;
        }
        double factor = (*reweightsPtr)[reweightIndex];

        // Debug printing xsec_sf_in
        if(i<20 || (i%10000==0)){
            std::cout << "  i="<< i
                      << " xsec_sf_in=" << xsec_sf_in
                      << " factor=" << factor
                      << " => xsec_sf_out=" << (xsec_sf_in * factor)
                      << std::endl;
        }

        // scale xsec_sf
        xsec_sf_out = xsec_sf_in * factor;

        // fill the rest of the branches from tin exactly as is
        tout->Fill();
    }

    std::cout << "[DEBUG] Done loop. Writing output...\n";
    f_out->cd();
    tout->Write();
    f_out->Close();

    f_inf->Close();
    f_nom->Close();

    std::cout << "[DONE] Wrote new file with 'xsec_sf' scaled by reweight "
              << reweightName << ": " << outFileName << std::endl;
    return EXIT_SUCCESS;
}
