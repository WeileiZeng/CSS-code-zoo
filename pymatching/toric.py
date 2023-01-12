import numpy as np
#import matplotlib.pyplot as plt
from scipy.sparse import hstack, kron, eye, csr_matrix, block_diag
import json
from pymatching import Matching
import TicToc
#import  multiprocessing
from multiprocessing import Pool
import datetime #datetime.datetime.now()
import time  #time.sleep()


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
#print(args.filename, args.count, args.verbose)
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




################  parameters #################
#NUM_TRIALS_MAX = 1e7 #limit max time per thread, 46 seconds for 1e6
#NUM_TRIALS=int(1e7) #3000
#NUM_ERRORS_MAX=100
#POOL_SIZE = 16-5-1

print(" NUM_TRIALS={}, NUM_ERRORS_MAX={}, POOL_SIZE={}, filename={}".format(NUM_TRIALS, NUM_ERRORS_MAX, POOL_SIZE,filename))

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

def decode(p,H,logicals):
#        matching = Matching.from_check_matrix(H, weights=np.log((1-p)/p), faults_matrix=logicals) #moved to init
        noise = rng.binomial(1, p, H.shape[1])
        syndrome = H@noise % 2
        predicted_logicals_flipped = matching.decode(syndrome)
        actual_logicals_flipped = logicals@noise % 2
        if not np.array_equal(predicted_logicals_flipped, actual_logicals_flipped):
            return 1 #error
        return 0 #good


#import itertools
#import math

def init(H,p,logicals):#to initialize Pool
    global rng
    rng = np.random.default_rng()
    global matching
    matching = Matching.from_check_matrix(H, weights=np.log((1-p)/p), faults_matrix=logicals) 

class Worker():
    def __init__(self, workers, initializer, initargs):
        self.num_errors_total=0 #accumulated error
        self.num_trials_total=0 #actual trials being simualted
        self.num_errors_max=NUM_ERRORS_MAX
        self.pool = Pool(processes=workers, #POOL_SIZE
                         initializer=initializer, 
                         initargs=initargs)
#,
#                         maxtasksperchild=16)

    def callback(self,is_error): #is_error could be 1 or 0
        if is_error:
            self.num_errors_total += 1
        self.num_trials_total += 1
        if (self.num_errors_total >= self.num_errors_max):
            self.pool.terminate() 

#    while (num_trials_actual < num_trials or num_errors < num_errors_min) and num_trials_actual < num_trials_max:        

    def do_job(self,num_trials,p,H,logicals):
        _flag=100
        for i in range(num_trials):
            try:
                while len(self.pool._cache) > 1e5:
#                    print("waiting for cache to clear...")
                    time.sleep(1)
                if i % _flag == 0 : 
                    _flag = _flag*10
                    print(i,end=',')
                self.pool.apply_async(decode, args=(p,H,logicals), callback=self.callback)
            except:#ValueException('Pool closed')
                print('exit when i = ',i)
                break
        print('Finish adding jobs')
        self.pool.close()
        self.pool.join()


def parallel_num_decoding_failures(H, logicals, p):
#def parallel_num_decoding_failures(H, logicals, p, num_trials, num_errors_min, pool_size):
    w = Worker(POOL_SIZE, init, (H,p,logicals))
    w.do_job(NUM_TRIALS,p,H,logicals)
    print('w.num_errors_total={}, w.num_trials_total={}'.format(w.num_errors_total,w.num_trials_total))
    log_error = w.num_errors_total/w.num_trials_total
    return log_error

def simulate(L:int):
    print("Simulating L={}...".format(L))
    Hx = toric_code_x_stabilisers(L)
    logX = toric_code_x_logicals(L)
    log_errors = []
    TicToc.tic()
    for p in ps:
        log_error = parallel_num_decoding_failures(Hx, logX, p)
        log_errors.append(log_error)
        print('p={:f}, log_error={:f}={:.3e}'.format(p,log_error,log_error), end=', ')
        TicToc.toc()
    print("Finish simulating L={}...".format(L))
    return log_errors





#seed=int(np.random.default_rng().random()*1e8)
#np.random.seed(seed) # seed=2
log_errors_all_L = []

for L in Ls:
    print(datetime.datetime.now())
    log_errors_all_L.append(simulate(L))
print(log_errors_all_L)

#save simulation result into json file
#dictionary={L:{'p_qubit':ps.tolist(),'p_block':logical_errors.tolist()} for L, logical_errors in zip(Ls, log_errors_all_L)}
dictionary={L:{'p_qubit':ps,'p_block':logical_errors} for L, logical_errors in zip(Ls, log_errors_all_L)}


#exit() #no file writing

# Serializing json
json_object = json.dumps(dictionary, indent=4)
# print(json_object)
with open(filename, "w") as outfile:
    outfile.write(json_object)

print("data saved to {}".format(filename))
