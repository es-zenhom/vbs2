#!/bin/env python

import plottery_wrapper as p

p.dump_plot(
        fnames = [
            "background_qcd.root",
            "background_ttbar.root",
            "background_wjets.root",
            "background_zjets.root",
            "background_VV_VH.root",
            "background_rare_EWK.root",
            "background_rare_top.root",

            ],
        sig_fnames=[
            "signal_vbsvvh.root",
            ],
        dirname="plots",
	    # data_fname="data.root",
        legend_labels=[
            "background_qcd",
            "background_ttbar",
            "background_Wjets",
            "background_Zjets",
            "background_VV_VH",
            "background_rare_EWK",
            "background_rare_top",
            ],
        signal_labels=[
            "signal_vbsvvh",
            ],

        extraoptions={
            "nbins":180,
            #"signal_scale": "auto",
             "print_yield": True,
             "yield_prec": 4,
             "remove_underflow": True,
             "signal_scale": 1000,

             "yaxis_log": True,
            },
         _plotter = p.plot_cut_scan,
         # filter_pattern="hist_abcdnet_fixedEta,hist_eta_fixedABCDNet",

        )
