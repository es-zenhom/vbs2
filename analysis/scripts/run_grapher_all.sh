#!/bin/bash

VERSION_TAG="output_abcdnet_v17"
# Define your cut values here
CUT_ABCDNET_SCORE=0.8
CUT_ABS_DETA_JJ=6
CUT_M_JJ=1300.0
OUTPUT_CSV="${HISTOGRAMS_DIR}/output_mjj${CUT_M_JJ}_abcdnet${CUT_ABCDNET_SCORE}_deta${CUT_ABS_DETA_JJ}_${VERSION_TAG}_150lamda_final.csv"

# New variable for the all events flag
ALL_EVENTS_FLAG=${1:-""}  # Use the first argument if provided, otherwise empty string
# Grab the second argument => MODE to pick how we run
MODE=${2:-""}

# Base directory for inferences
BASE_DIR="/home/users/eslam.zenhom/public_html/uf_work/vbs2/analysis/studies/vbsvvhjets_semimerged/${VERSION_TAG}/Run2/inferences"

# Create histograms directory if it doesn't exist
HISTOGRAMS_DIR="${BASE_DIR}/histograms"
mkdir -p "$HISTOGRAMS_DIR"

# Compile the program
g++ -std=c++17 grapher.C -c -o program.o $(root-config --cflags --libs)
g++ -std=c++17 program.o -o myProgram $(root-config --ldflags --libs) -lGenVector

# Function to run myProgram with correct paths
run_my_program() {
    local input_file="$1"
    local output_file="$2"
    ./myProgram "$BASE_DIR" "$input_file" "${HISTOGRAMS_DIR}/${output_file}" $CUT_ABCDNET_SCORE $CUT_ABS_DETA_JJ $CUT_M_JJ "$OUTPUT_CSV" $ALL_EVENTS_FLAG
}

# Run myProgram for each input file
# run_my_program "QCD.root" "background_qcd.root"
# run_my_program "VV_VH.root" "background_VV_VH.root"
# run_my_program "rare_top.root" "background_rare_top.root"
# run_my_program "wjets.root" "background_wjets.root"
# run_my_program "zjets.root" "background_zjets.root"
# run_my_program "rare_EWK.root" "background_rare_EWK.root"
# run_my_program "ttbar.root" "background_ttbar.root"
# run_my_program "VBSVVH.root" "signal_vbsvvh.root"
# run_my_program "data.root" "data.root"
if [ "$2" = "c2v" ]; then

    echo "[INFO] Mode c2v => run grapher on c2v reweighted files only"
    # Suppose you have c2v1p0.root and c2v1p5.root in $BASE_DIR
    # Produce outputs named signal_c2v1p0.root, signal_c2v1p5.root
    run_my_program "c2v1p0.root"  "signal_c2v1p0.root"
    run_my_program "c2v1p3.root"  "signal_c2v1p3.root"
    run_my_program "c2v1p5.root"  "signal_c2v1p5.root"

    echo "[INFO] Done with c2v mode"

else
    echo "[INFO] Default => run grapher on usual backgrounds + nominal signal"

    # Backgrounds
    run_my_program "QCD.root"      "background_qcd.root"
    run_my_program "VV_VH.root"    "background_VV_VH.root"
    run_my_program "rare_top.root" "background_rare_top.root"
    run_my_program "wjets.root"    "background_wjets.root"
    run_my_program "zjets.root"    "background_zjets.root"
    run_my_program "rare_EWK.root" "background_rare_EWK.root"
    run_my_program "ttbar.root"    "background_ttbar.root"

    # Signal
    run_my_program "VBSVVH.root"   "signal_vbsvvh.root"

    # Data
    run_my_program "data.root"     "data.root"

    echo "[INFO] Done with default mode"
fi
