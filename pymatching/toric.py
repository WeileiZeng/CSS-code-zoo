import numpy as np
import matplotlib.pyplot as plt
from scipy.sparse import hstack, kron, eye, csr_matrix, block_diag


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

from pymatching import Matching

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

import itertools
def num_decoding_failures(H, logicals, p, num_trials):
    matching = Matching.from_check_matrix(H, weights=np.log((1-p)/p), faults_matrix=logicals)
    num_errors = 0
#    print(num_trials)
#    error_list=[0 for _ in range(num_trials)]


    for i in range(num_trials):
        noise = np.random.binomial(1, p, H.shape[1])
        syndrome = H@noise % 2
        predicted_logicals_flipped = matching.decode(syndrome)
        actual_logicals_flipped = logicals@noise % 2
        if not np.array_equal(predicted_logicals_flipped, actual_logicals_flipped):
            num_errors += 1
    return num_errors

def parallel_num_decoding_failures(H, logicals, p, num_trials, pool_size):
    num_errors_total=0
    num_trials_list = [ int(1+num_trials/pool_size) for _ in range(pool_size)]
    num_trials_list[-1] = int( num_trials - (num_trials/pool_size+1)*(pool_size-1) )
#    print("num_trials_list = ", num_trials_list)
    from multiprocessing import Pool
    with Pool(processes=pool_size) as pool:
        num_errors_list = pool.starmap(num_decoding_failures,
                                       [(H, logicals, p, num_trials_list[_]) for _ in range(pool_size)])
    for _ in num_errors_list:
        num_errors_total += _
    return num_errors_total


def simulate(L:int):
    print("Simulating L={}...".format(L))
    Hx = toric_code_x_stabilisers(L)
    logX = toric_code_x_logicals(L)
    log_errors = []
    for p in ps:
        #print(p,)
        num_errors = parallel_num_decoding_failures(Hx, logX, p, num_trials, pool_size=30)
        log_errors.append(num_errors/num_trials)
#    log_errors_all_L.append(np.array(log_errors))
    print("Finish simulating L={}...".format(L))
    return log_errors

num_trials = 3000000
Ls = range(9,21,2) #change from 8 to 9
ps = np.linspace(0.01, 0.2, 9)
ps = np.linspace(0.03, 0.15, 5)
ps=[0.1,
 0.07142857142857144,
 0.051020408163265314,
 0.036443148688046656,
 0.0260308204914619,
 0.018593443208187073,
 0.013281030862990767,
 0.009486450616421976]
np.random.seed(2)
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
import json
filename="toric2.json"
#filename='tmp.json'
with open(filename, "w") as outfile:
    outfile.write(json_object)

print("data saved to {}".format(filename))
