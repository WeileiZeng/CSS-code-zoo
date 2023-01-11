import time

def TicTocGenerator():
    # Generator that returns time differences
    ti = 0           # initial time
    tf = time.time() # final time
    while True:
        ti = tf
        tf = time.time()
        yield tf-ti # returns the time difference

TicToc = TicTocGenerator() # create an instance of the TicTocGen generator

# This will be the main function through which we define both tic() and toc()
def toc(tempBool=True):
    # Prints the time difference yielded by generator instance TicToc
    tempTimeInterval = next(TicToc)
    if tempBool:
        if tempTimeInterval > 1800: # more than half hour 30*60=1800
            print( "Elapsed time: %.2f hours.\n" %(tempTimeInterval/3600.0) )
        elif tempTimeInterval > 300: # more than 5 mins
            print( "Elapsed time: %.2f mins.\n" %(tempTimeInterval/60.0) )
        else:
            print( "Elapsed time: %.2f seconds.\n" %tempTimeInterval )

def tic():
    # Records a time in TicToc, marks the beginning of a time interval
    toc(False)
