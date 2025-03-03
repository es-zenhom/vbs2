#!/bin/bash

g++ grapher.C -c -o program.o $(root-config --cflags --libs) 

g++ program.o -o myProgram $(root-config --ldflags --libs) -lGenVector 

./myProgram /home/users/eslam.zenhom/public_html/uf_work/vbs2/analysis/studies/vbsvvhjets_semimerged/output_abcdnet_v3/Run2/inferences QCD.root histograms/background_qcd.root 

./myProgram /home/users/eslam.zenhom/public_html/uf_work/vbs2/analysis/studies/vbsvvhjets_semimerged/output_abcdnet_v3/Run2/inferences VV_VH.root histograms/background_VV_VH.root 

./myProgram /home/users/eslam.zenhom/public_html/uf_work/vbs2/analysis/studies/vbsvvhjets_semimerged/output_abcdnet_v3/Run2/inferences rare_top.root histograms/background_rare_top.root 

./myProgram /home/users/eslam.zenhom/public_html/uf_work/vbs2/analysis/studies/vbsvvhjets_semimerged/output_abcdnet_v3/Run2/inferences wjets.root histograms/background_wjets.root 

./myProgram /home/users/eslam.zenhom/public_html/uf_work/vbs2/analysis/studies/vbsvvhjets_semimerged/output_abcdnet_v3/Run2/inferences zjets.root histograms/background_zjets.root 

./myProgram /home/users/eslam.zenhom/public_html/uf_work/vbs2/analysis/studies/vbsvvhjets_semimerged/output_abcdnet_v3/Run2/inferences rare_EWK.root histograms/background_rare_EWK.root 

./myProgram /home/users/eslam.zenhom/public_html/uf_work/vbs2/analysis/studies/vbsvvhjets_semimerged/output_abcdnet_v3/Run2/inferences ttbar.root histograms/background_ttbar.root 

./myProgram /home/users/eslam.zenhom/public_html/uf_work/vbs2/analysis/studies/vbsvvhjets_semimerged/output_abcdnet_v3/Run2/inferences VBSVVH.root histograms/signal_vbsvvh.root 

./myProgram /home/users/eslam.zenhom/public_html/uf_work/vbs2/analysis/studies/vbsvvhjets_semimerged/output_abcdnet_v3/Run2/inferences data.root histograms/data.root 
