import os
import glob
import json
import signal
import hashlib
import argparse
import itertools
from multiprocessing import Pool
from multiprocessing import Manager
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import warnings
warnings.simplefilter(action="ignore", category=pd.errors.PerformanceWarning)
from tqdm import tqdm

from utils.analysis import Optimization

class BruteForceScan:
    def __init__(self):
        self.results = []
        self.df = None

    @staticmethod
    def get_cuts(cut_config, ineq=">", invert=False):
        cuts = {}
        for var, linspace_args in cut_config.items():
            cuts[var] = []
            for wp in np.linspace(*linspace_args):
                if invert:
                    cuts[var].append(f"(not {var} {ineq} {wp})")
                else:
                    cuts[var].append(f"{var} {ineq} {wp}")
        return cuts

    @staticmethod
    def fom(sig, bkg):
        return sig/np.sqrt(bkg)

    def run_job(self, args):
        job_i, A, B, C, D = args
        A_df = self.df.query(A)
        B_df = self.df.query(B)
        C_df = self.df.query(C)
        D_df = self.df.query(D)

        A_sig = A_df[ A_df.is_signal & ~A_df.is_data].event_weight.sum()
        A_bkg = A_df[~A_df.is_signal & ~A_df.is_data].event_weight.sum()

        B_sig = B_df[ B_df.is_signal & ~B_df.is_data].event_weight.sum()
        B_bkg = B_df[~B_df.is_signal & ~B_df.is_data].event_weight.sum()

        C_sig = C_df[ C_df.is_signal & ~C_df.is_data].event_weight.sum()
        C_bkg = C_df[~C_df.is_signal & ~C_df.is_data].event_weight.sum()

        D_sig = D_df[ D_df.is_signal & ~D_df.is_data].event_weight.sum()
        D_bkg = D_df[~D_df.is_signal & ~D_df.is_data].event_weight.sum()

        B_data = len(B_df[~B_df.is_signal & B_df.is_data])
        C_data = len(C_df[~C_df.is_signal & C_df.is_data])
        D_data = len(D_df[~D_df.is_signal & D_df.is_data])

        A_pred = B_data*C_data/D_data
        # print(f"Job {job_i}: A_bkg={A_bkg}, B_bkg={B_bkg}, C_bkg={C_bkg}, D_bkg={D_bkg}, A_sig={A_sig}, B_sig={B_sig}, C_sig={C_sig}, D_sig={D_sig}, A_pred={A_pred}, Sens={A_sig/np.sqrt(A_pred)}")
        # print(f"Job {job_i} Results: {self.results[job_i]}")

        self.results[job_i] = {
            # Signal region definition
            "region_A": A,
            # Signal yields
            "A_sig": A_sig,
            "B_sig": B_sig,
            "C_sig": C_sig,
            "D_sig": D_sig,
            # Background yields
            "A_bkg": A_bkg,
            "B_bkg": B_bkg,
            "C_bkg": C_bkg,
            "D_bkg": D_bkg,
            # Data yields
            "B_data": B_data,
            "C_data": C_data,
            "D_data": D_data,
            # Background yield in Region A predicted from data
            "A_pred": A_pred,
            # Figure of merit (FOM)
            "fom": self.fom(A_sig, A_pred)
        }

    def run(self, config_json, n_workers=32):

        base_dir = "/".join(config_json.split("/")[:-1])
        with open(config_json) as f:
            config = json.load(f)
            config_str = "".join(json.dumps(config).split())
            config_hash = hashlib.sha1(config_str.encode("utf-8"))
            if config.get("output_dir", None) is None:
                results_csv = f"{base_dir}/results_{config_hash.hexdigest()[:12]}.csv"
            else:
                results_csv = f"{config['output_dir']}/results_{config_hash.hexdigest()[:12]}.csv"

        if os.path.exists(results_csv):
            print(f"This config has already been processed: {results_csv}")
            return

        if config.get("input_dir", None) is None:
            sig_root_files  = [f"{base_dir}/Run2/inferences/{f}" for f in config["sig_files"]]
            bkg_root_files  = [f"{base_dir}/Run2/inferences/{f}" for f in config["bkg_files"]]
            data_root_files = [f"{base_dir}/Run2/inferences/{f}" for f in config["data_files"]]
        else:
            sig_root_files  = [f"{config['input_dir']}/{f}" for f in config["sig_files"]]
            bkg_root_files  = [f"{config['input_dir']}/{f}" for f in config["bkg_files"]]
            data_root_files = [f"{config['input_dir']}/{f}" for f in config["data_files"]]

        analysis = Optimization(
            sig_root_files=sig_root_files,
            bkg_root_files=bkg_root_files,
            data_root_files=data_root_files,
            ttree_name=config["ttree_name"],
            weight_columns=config["weights"]
        )
        self.df = analysis.df

        x_arm = config["abcd_arms"][0]
        y_arm = config["abcd_arms"][1]
        presel = config["presel"]

        presel_cuts = self.get_cuts(presel)

        A_cuts = {**self.get_cuts(x_arm), **self.get_cuts(y_arm), **presel_cuts}
        B_cuts = {**self.get_cuts(x_arm), **self.get_cuts(y_arm, invert=True), **presel_cuts}
        C_cuts = {**self.get_cuts(x_arm, invert=True), **self.get_cuts(y_arm), **presel_cuts}
        D_cuts = {**self.get_cuts(x_arm, invert=True), **self.get_cuts(y_arm, invert=True), **presel_cuts}

        A_regions = [" and ".join(cuts) for cuts in itertools.product(*A_cuts.values())] # signal regions
        B_regions = [" and ".join(cuts) for cuts in itertools.product(*B_cuts.values())] # anchor regions
        C_regions = [" and ".join(cuts) for cuts in itertools.product(*C_cuts.values())] # control region
        D_regions = [" and ".join(cuts) for cuts in itertools.product(*D_cuts.values())] # control region

        n_regions = len(A_regions)
             # Initialize a Manager for shared state management
        manager = Manager()
        self.results = manager.list([manager.dict() for _ in range(len(A_regions))])

        # print(f"Initialized self.results with {n_regions} empty dictionaries.")  # Add this line to verify initialization
        jobs = [(job_i, *abcd) for job_i, abcd in enumerate(zip(A_regions, B_regions, C_regions, D_regions))]

        # Execute jobs
        with Pool(processes=n_workers) as pool:
            with tqdm(total=len(jobs), desc="Executing jobs") as pbar:
                for _ in pool.imap_unordered(self.run_job, jobs):
                    pbar.update()
        # Add these lines to print the results
        # print("Results:")
        # print(self.results)  # Print the results to check if they are populated
        normal_results = [dict(result) for result in self.results]
        results_df = pd.DataFrame(normal_results)
        print(results_df)
        results_df.sort_values(by="fom", inplace=True)

        print_cols = ["A_sig", "A_bkg", "B_bkg", "C_bkg", "D_bkg", "A_pred", "B_data", "C_data", "D_data", "fom"]
        print(results_df[(results_df.B_data >= 4) & (results_df.A_pred >= 0.8)][print_cols])

        results_df.to_csv(results_csv, index=False)
        print(f"Wrote {results_csv}")

if __name__ == "__main__":
    cli = argparse.ArgumentParser(description="Run brute force scan")
    cli.add_argument(
        "config_json", type=str,
        help="path to configuration .json file"
    )
    cli.add_argument(
        "-n", "--n_workers", type=int, default=1,
        help="number of workers"
    )
    args = cli.parse_args()

    bfscan = BruteForceScan()
    bfscan.run(args.config_json, n_workers=args.n_workers)
