#!/usr/bin/env python

import sys
import plottery_wrapper as p

def main(version_tag, output_suffix=None, withdata=False):
    BASE_DIR = f"/home/users/eslam.zenhom/public_html/uf_work/vbs2/analysis/studies/vbsvvhjets_semimerged/{version_tag}/Run2/inferences/histograms"
    
    # Construct the output directory
    if output_suffix:
        output_dir = f"/home/users/eslam.zenhom/public_html/graphs/{version_tag}_{output_suffix}"
    else:
        output_dir = f"/home/users/eslam.zenhom/public_html/graphs/{version_tag}"


    p.dump_plot(
        fnames = [
            f"{BASE_DIR}/background_qcd.root",
            f"{BASE_DIR}/background_ttbar.root",
            f"{BASE_DIR}/background_wjets.root",
            f"{BASE_DIR}/background_zjets.root",
            f"{BASE_DIR}/background_VV_VH.root",
            f"{BASE_DIR}/background_rare_EWK.root",
            f"{BASE_DIR}/background_rare_top.root",
        ],
        sig_fnames=[
            f"{BASE_DIR}/signal_vbsvvh.root",
        ],
        dirname=output_dir,
        data_fname=f"{BASE_DIR}/data.root" if withdata else None,
		legend_labels=[
            "QCD",
            "ttbar",
            "Wjets",
            "Zjets",
            "VV/VH",
            "Rare EWK",
            "Rare top",
        ],
        signal_labels=[
            "Sig_VBSVVH",
        ],
        extraoptions={
            "nbins": 180,
			#"signal_scale": "auto",
            "print_yield": True,
            "yield_prec": 4,
            "remove_underflow": True,
            # "signal_scale": 1000,
            "yaxis_log": True,
            "lumi_value": 138,
        },
        # _plotter = p.plot_cut_scan,
        # filter_pattern="hist_abcdnet_fixedEta,hist_eta_fixedABCDNet",
    )

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python script.py <version_tag> [output_suffix] [withdata]")
        sys.exit(1)
    
    version_tag = sys.argv[1]
    output_suffix = sys.argv[2] if len(sys.argv) > 2 else None
    withdata = "withdata" in sys.argv
    
    main(version_tag, output_suffix, withdata)
