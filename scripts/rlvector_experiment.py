from subprocess import check_output
import datetime
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import argparse
import re, sys
import os, glob
import shutil

rlvector_benchmark="benchmark/"
build_dir = "benchmark/build/"

def exe(cmd):
    try:
        return check_output(cmd)
    except Exception, e:
        print 'Error while running `%s`: %s' % (' '.join(cmd), e)
        raise
    

def grep(s,pattern):
    return '\n'.join(re.findall(r'^.*%s.*?$'%pattern,s,flags=re.M))

def execute_rlvector_benchmark(benchmark):
    cmd = ['./executer/rlvector_experiment',benchmark,build_dir]
    res = exe(cmd)
    return grep(res,'RESULT').split('\n')


def get_rlvector_stats(out):
    res = [str(out.split('Vector=')[1].split()[0])]
    res += [str(out.split('Benchmark=')[1].split()[0])]
    res += [float(out.split('ConstructionTime=')[1].split()[0])]
    res += [float(out.split('SpaceBitsPerElement=')[1].split()[0])]
    res += [float(out.split('RandomAccessTimePerElement=')[1].split()[0])]
    res += [float(out.split('SequentialAccessTimePerElement=')[1].split()[0])]   
    return res


def delete_folder_content(experiment_dir):
    for root, dirs, files in os.walk(experiment_dir, topdown=False):
        for name in files:
            os.remove(os.path.join(root, name))
        for name in dirs:
            os.rmdir(os.path.join(root, name))

def seperator(N):
    sep = []
    for i in range(0,N):
        sep += ['--------']
    return sep


def experiment(dirname):
    print 'Run-Length Compressed Vector-Experiment\n============'
    rlvector_res = []
    cols_rlvector = ["Vector","Benchmark","ConstructionTime","SpaceBitsPerElement",
                    "RandomAccessTimePerElement","SequentialAccessTimePerElement"]
    
    #Create HTML-Folder for Memory-Usage
    try: os.stat("HTML/");
    except: os.mkdir("HTML/");

    benchmarks = glob.glob(rlvector_benchmark+"*")
    for benchmark in benchmarks:
        if os.path.isdir(benchmark) or os.path.basename(benchmark) == "download.sh":
            continue
        print("Execute benchmark " + os.path.basename(benchmark) + "...")
        benchmark_res = execute_rlvector_benchmark(benchmark)
        benchmark_rlvector_res = []
        for r in benchmark_res:
            rlvector_res.append(get_rlvector_stats(r));    
            benchmark_rlvector_res.append(get_rlvector_stats(r));
        df_benchmark_rlvector = pd.DataFrame(benchmark_rlvector_res,columns=cols_rlvector)
        print(df_benchmark_rlvector)
        
    htmls = glob.glob("HTML/*");
    for html in htmls:
        shutil.move(html,dirname + "/HTML/" + os.path.basename(html))

    cols_rlvector = ["Vector","Benchmark","ConstructionTime","SpaceBitsPerElement",
                     "RandomAccessTimePerElement","SequentialAccessTimePerElement"]
    df_rlvector = pd.DataFrame(rlvector_res,columns=cols_rlvector)
    df_rlvector.to_csv(dirname + '/rlvector_result.csv')
    

def setup_experiment_environment():
    idx = 0;
    dirname = "results/"+str(datetime.datetime.now().date())+"_rl_vector_experiment";
    while True:
        try: 
            os.stat(dirname + "_" + str(idx))
            idx = idx + 1;
        except: 
            os.mkdir(dirname + "_" + str(idx))
            break;
    os.mkdir(dirname + "_" + str(idx) + "/HTML")
    return dirname + "_" + str(idx)


if __name__ == '__main__':

    dirname = setup_experiment_environment()
    experiment(dirname)
    