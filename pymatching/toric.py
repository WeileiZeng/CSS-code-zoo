import numpy as np
#import matplotlib.pyplot as plt
from scipy.sparse import hstack, kron, eye, csr_matrix, block_diag
import json
from pymatching import Matching

NUM_TRIALS_MAX = 1e7 #limit max time per thread, 46 seconds for 1e6
NUM_TRIALS=30000

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

import itertools
import math
def num_decoding_failures(H, logicals, p, num_trials, num_errors_min):
    L=int(math.sqrt(H.shape[1]/2)) # size as well as distance
    t=int(L/2) #good errors with weight < t
    num_errors = 0
    num_trials_actual=0
    num_trials_max = NUM_TRIALS_MAX #limit max time per thread, 46 seconds for 1e6
    while (num_trials_actual < num_trials or num_errors < num_errors_min) and num_trials_actual < num_trials_max:
        num_trials_actual += 1
#    for i in range(num_trials):
        noise = rng.binomial(1, p, H.shape[1]) #rng from init()
#        noise = np.random.binomial(1, p, H.shape[1])
        noise_weight=0 #count weight for preprocessing
        for bit in noise:
            if bit == 1:
                noise_weight += 1
        if noise_weight < t: # pass for good errors
            continue
        syndrome = H@noise % 2
        predicted_logicals_flipped = matching.decode(syndrome)
        actual_logicals_flipped = logicals@noise % 2
        if not np.array_equal(predicted_logicals_flipped, actual_logicals_flipped):
            num_errors += 1

#    print('num_errors_min={}, num_errors={}, num_trails_actual={}'.format( num_errors_min, num_errors, num_trials_actual))
    return num_errors, num_trials_actual


def init(H,p,logicals):#to initialize Pool
    global rng
    rng = np.random.default_rng()
    global matching
    matching = Matching.from_check_matrix(H, weights=np.log((1-p)/p), faults_matrix=logicals) #no need to put it here but anyway

from multiprocessing import Pool
def parallel_num_decoding_failures(H, logicals, p, num_trials, num_errors_min, pool_size):
    num_trials_list = [ int(1+num_trials/pool_size) for _ in range(pool_size)]
    num_trials_list[-1] = int( num_trials - (num_trials/pool_size+1)*(pool_size-1) )

    with Pool(processes=pool_size, initializer=init, initargs=(H,p,logicals)) as pool:
        result_list = pool.starmap(num_decoding_failures,
                                       [(H, logicals, p, num_trials_list[_], int(num_errors_min/pool_size)) for _ in range(pool_size)])
    num_errors_total=0
    num_trials_total=0
    for num_errors, num_trials_actual in result_list:
        num_errors_total += num_errors
        num_trials_total += num_trials_actual
#    return num_errors_total
    log_error = num_errors_total/num_trials_total
    return log_error

import TicToc

def simulate(L:int):
    print("Simulating L={}...".format(L))
    Hx = toric_code_x_stabilisers(L)
    logX = toric_code_x_logicals(L)
    log_errors = []
    TicToc.tic()
    for p in ps:
        log_error = parallel_num_decoding_failures(Hx, logX, p, num_trials, num_errors_min, pool_size)
        log_errors.append(log_error)
        print('p={:f}'.format(p),end=', ')
        print('log_error={:f}={:.3e}'.format(log_error,log_error), end=', ')
        TicToc.toc()
    print("Finish simulating L={}...".format(L))
    return log_errors

num_trials = NUM_TRIALS #30000 #3000000
pool_size=5
num_errors_min = pool_size*1

Ls = range(9,21,2) #change from 8 to 9
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

#seed=int(np.random.default_rng().random()*1e8)
#np.random.seed(seed) # seed=2
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
filename="toric-{}-{}.json".format(num_trials,NUM_TRIALS_MAX) 
#filename='tmp.json'
with open(filename, "w") as outfile:
    outfile.write(json_object)

print("data saved to {}".format(filename))
