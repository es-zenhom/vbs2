import os
import sys
import json
import signal
import logging
import concurrent.futures as futures
from subprocess import Popen, PIPE
from tqdm import tqdm

class Job:
    def __init__(self, cmd, stdout_file, stderr_file):
        self.cmd = cmd
        self.stdout_file = stdout_file
        self.stderr_file = stderr_file

    def unpack(self):
        return (self.cmd, self.stdout_file, self.stderr_file)

class Orchestrator:
    def __init__(self, executable, input_files, n_workers=8):
        self.executable = executable
        self.input_files = sorted(
            input_files, 
            key=lambda f: os.stat(f).st_size, 
            reverse=True
        )
        self.n_workers = n_workers
        self.submitted_futures = None

        signal.signal(signal.SIGINT, self.stop())

    def run_job(self, args):
        cmd, stdout_file, stderr_file = args
        with open(stdout_file,"wb") as stdout:
            stdout.write(f"{' '.join(cmd)}\n".encode("utf-8"))
        with open(stdout_file,"ab") as stdout, open(stderr_file,"wb") as stderr:
            process = Popen(cmd, stdout=stdout, stderr=stderr)
            process.wait()
        return

    def prepare_job(self, args):
        input_file = args[0]
        cmd = self._get_job(input_file)
        stdout_file, stderr_file = self._get_log_files(input_file)
        return cmd, stdout_file, stderr_file

    def run(self):
        # Get path to logfile
        logfile = logging.getLoggerClass().root.handlers[0].baseFilename

        # Prepare jobs
        jobs = []
        stderr_files = []
        with tqdm(total=len(self.input_files), desc="Preparing jobs") as pbar:
            with futures.ThreadPoolExecutor(max_workers=self.n_workers) as executor:
                self.submitted_futures = {
                    executor.submit(self.prepare_job, (f,)): f for f in self.input_files
                }
                for future in futures.as_completed(self.submitted_futures):
                    job = Job(*future.result())
                    jobs.append(job)
                    stderr_files.append(job.stderr_file)
                    pbar.update(1)

        # Execute jobs
        n_errors = 0
        with tqdm(total=len(jobs), desc="Executing jobs") as pbar:
            with futures.ThreadPoolExecutor(max_workers=self.n_workers) as executor:
                self.submitted_futures = {
                    executor.submit(self.run_job, job.unpack()): job for job in jobs
                }
                for future in futures.as_completed(self.submitted_futures):
                    # Check for errors
                    job = self.submitted_futures[future]
                    stderr_file = job.stderr_file
                    if os.stat(stderr_file).st_size > 0:
                        n_errors += 1
                        job_name = stderr_file.split("/")[-1].replace(".err", "")
                        logging.error(f"{job_name} failed; check logs: {stderr_file}")
                        pbar.set_description(f"Executing jobs ({n_errors} errors)")
                        pbar.refresh()
                    # Update progress bar
                    pbar.update(1)

        # Print warning about errors thrown if any
        if n_errors > 0:
            n_jobs = f"{n_errors} job{'s' if n_errors > 1 else ''}"
            print(f"WARNING: {n_jobs} did not run successfully; check {logfile}")

    def stop(self):
        def sigint_handler(sig, frame):
            logging.info("Killing jobs (received SIGINT)")
            for future in self.submitted_futures:
                future.cancel()
            sys.exit(0)
        return sigint_handler

    def _get_job(self, input_file):
        raise NotImplementedError

    def _get_log_files(self, input_file):
        raise NotImplementedError
