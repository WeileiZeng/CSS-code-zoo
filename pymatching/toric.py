import numpy as np
#import matplotlib.pyplot as plt
from scipy.sparse import hstack, kron, eye, csr_matrix, block_diag
import json
from pymatching import Matching
import TicToc

import itertools
import math

import argparse

parser = argparse.ArgumentParser(
                    prog = 'toric',
                    description = 'Threshold simulation for toric codes with various size LxL. Author: Weilei Zeng',
                    epilog = 'Package Used: PyMatching')

parser.add_argument('--num_trials', default=1000, type=int, help="max number of trials for each (p_qubit,p_logical,L) data point")
parser.add_argument('--num_errors_max', default=10, type=int,help="max number of errors accumulated for single data point to avoid over running")
parser.add_argument('--pool_size', default=2, type=int, help="number of workers to work in parallel")

args = parser.parse_args()
print(args)

NUM_TRIALS=args.num_trials
NUM_ERRORS_MAX=args.num_errors_max
POOL_SIZE = args.pool_size
filename="toric-trials{}-errors{}.json".format(NUM_TRIALS,NUM_ERRORS_MAX)
filename='tmp.json'

Ls = range(9,21,1) #change from 8 to 9
#Ls = range(8,21,1) #change from 8 to 9
print("Ls=",Ls)
#ps = np.linspace(0.01, 0.2, 9)
#ps = np.linspace(0.03, 0.15, 5)
#use ps same as SPC simulation in C++ code
ps=[0.1,
 0.07142857142857144,
 0.051020408163265314,
 0.036443148688046656,
 0.0260308204914619,
 0.018593443208187073,
 0.013281030862990767,
 0.009486450616421976]
print("ps=",ps)




def repetition_code(n):
    """
    Parity check matrix of a repetition code with length n.
    """
    row_ind, col_ind = zip(*((i, j) for i in range(n) for j in (i, (i+1)%n)))
    data = np.ones(2*n, dtype=np.uint8)
    return csr_matrix((data, (row_ind, col_ind)))


def toric_code_x_stabilisers(L):
    """
    Sparse check matrix for the X stabilisers of a toric code with 
    lattice size L, constructed as the hypergraph product of 
    two repetition codes.
    """
    Hr = repetition_code(L)
    H = hstack(
            [kron(Hr, eye(Hr.shape[1])), kron(eye(Hr.shape[0]), Hr.T)],
            dtype=np.uint8
        )
    H.data = H.data % 2
    H.eliminate_zeros()
    return csr_matrix(H)

def toric_code_x_logicals(L):
    """
    Sparse binary matrix with each row corresponding to an X logical operator 
    of a toric code with lattice size L. Constructed from the 
    homology groups of the repetition codes using the Kunneth 
    theorem.
    """
    H1 = csr_matrix(([1], ([0],[0])), shape=(1,L), dtype=np.uint8)
    H0 = csr_matrix(np.ones((1, L), dtype=np.uint8))
    x_logicals = block_diag([kron(H1, H0), kron(H0, H1)])
    x_logicals.data = x_logicals.data % 2
    x_logicals.eliminate_zeros()
    return csr_matrix(x_logicals)


"""
# not in use. replaced by num_decoding_failures()
def num_decoding_failures_via_physical_frame_changes(H, logicals, p, num_trials):    
    matching = Matching.from_check_matrix(H, weights=np.log((1-p)/p))
    num_errors = 0
    for i in range(num_trials):
        noise = np.random.binomial(1, p, H.shape[1])
        syndrome = H@noise % 2
        prediction = matching.decode(syndrome)
        predicted_logicals_flipped = logicals@prediction % 2
        actual_logicals_flipped = logicals@noise % 2
        if not np.array_equal(predicted_logicals_flipped, actual_logicals_flipped):
            num_errors += 1
    return num_errors
"""
"""
# not in use after reorgnize Pool
def decode(p,H,matching,logicals):
#def decode(_):
        matching = Matching.from_check_matrix(H, weights=np.log((1-p)/p), faults_matrix=logicals)
        print('p={}'.format(p))
        noise = np.random.binomial(1, p, H.shape[1])
        syndrome = H@noise % 2
        predicted_logicals_flipped = matching.decode(syndrome)
        actual_logicals_flipped = logicals@noise % 2
        if not np.array_equal(predicted_logicals_flipped, actual_logicals_flipped):
            return 1 #error
        return 0 #good
#            num_errors += 1
"""

#def num_decoding_failures(H, logicals, p, num_trials, num_errors_min):
def num_decoding_failures(H, logicals, p, num_trials):
    L=int(math.sqrt(H.shape[1]/2)) # size as well as distance
    t=int(L/2) #good errors with weight < t
    num_errors = 0
    for i in range(num_trials):
        noise = rng.binomial(1, p, H.shape[1]) #rng from init()
        noise_weight=0 #count weight for preprocessing
        for bit in noise:
            if bit == 1:
                noise_weight += 1
        if noise_weight < t: # pass for good errors
            continue
        #otherwise decode it
        syndrome = H@noise % 2
        predicted_logicals_flipped = matching.decode(syndrome)
        actual_logicals_flipped = logicals@noise % 2
        if not np.array_equal(predicted_logicals_flipped, actual_logicals_flipped):
            num_errors += 1

#    print('num_errors_min={}, num_errors={}, num_trails_actual={}'.format( num_errors_min, num_errors, num_trials_actual))
    return num_errors


def init(H,p,logicals):#to initialize Pool
    global rng
    rng = np.random.default_rng()
    global matching
    matching = Matching.from_check_matrix(H, weights=np.log((1-p)/p), faults_matrix=logicals) #no need to put it here but anyway

from multiprocessing import Pool
def parallel_num_decoding_failures(H, logicals, p, num_trials, pool_size):
    num_jobs=pool_size*10 #to avoid very long job blocking the program
    num_trials_per_job=int(num_trials/num_jobs)

    with Pool(processes=pool_size, initializer=init, initargs=(H,p,logicals)) as pool:
        result_list = pool.starmap(
            num_decoding_failures,
            itertools.repeat(
                (H, logicals, p, num_trials_per_job), 
                num_jobs)
        )
    num_errors_total=0
    num_trials_total=num_trials_per_job*num_jobs
    for num_errors in result_list:
        num_errors_total += num_errors
    log_error = num_errors_total/num_trials_total
    return log_error

#use pre results to estimate how many trials is needed for each data
def get_pre_result(L:int):
    result_filename="toric-trials100000-errors100.json"
    result = json.load(open(result_filename,'r'))
    log_errors=result[str(L)]["p_block"]
#    print(result[str(L)]["p_block"])
    return log_errors

def get_num_trials_list(L:int,num_errors_max:int, num_trials_max:int):
    log_errors_estimate = get_pre_result(L)
    num_trials_list=[]
    p_min = num_errors_max/num_trials_max
    for p in log_errors_estimate:
        if p < p_min: #may not get a single error in this case
            num_trials=num_trials_max
        else:  #get approximately num_erros_max of errors
            num_trials = int(num_errors_max/p)
        if num_trials < 500: #num_of_trails_min 
            num_trials = 500
        num_trials_list.append(num_trials)
    return num_trials_list


def simulate(L:int):
    print("Simulating L={}...".format(L), end = ' ')
    Hx = toric_code_x_stabilisers(L)
    logX = toric_code_x_logicals(L)
    log_errors = []
    log_errors_estimate = get_pre_result(L)

    num_trials_list = get_num_trials_list(L,num_errors_max=NUM_ERRORS_MAX, num_trials_max=NUM_TRIALS)
    print("num_trials_list:",num_trials_list)

    TicToc.tic()
    for i in range(len(ps)):
        p=ps[i]
        num_trials=num_trials_list[i]
        log_error = parallel_num_decoding_failures(Hx, logX, p, num_trials, pool_size=POOL_SIZE)
        log_errors.append(log_error)
        print('p={:f}'.format(p),end=', ')
        print('log_error={:f}={:.3e}'.format(log_error,log_error), end=', ')
        TicToc.toc()
    print("Finish simulating L={}...".format(L))
    return log_errors



def main():
    log_errors_all_L = []

    for L in Ls:
        log_errors_all_L.append(simulate(L))
        print(log_errors_all_L)

    #save simulation result into json file
    #dictionary={L:{'p_qubit':ps.tolist(),'p_block':logical_errors.tolist()} for L, logical_errors in zip(Ls, log_errors_all_L)}
    dictionary={L:{'p_qubit':ps,'p_block':logical_errors} for L, logical_errors in zip(Ls, log_errors_all_L)}

    # Serializing json
    json_object = json.dumps(dictionary, indent=4)
    #print(json_object)
    with open(filename, "w") as outfile:
        outfile.write(json_object)

    print("data saved to {}".format(filename))


main()
