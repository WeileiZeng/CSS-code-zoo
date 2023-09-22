#!python3
# processing the json files for CSS codes

import os            #mv files
import fileinput     #read (json) files
import json
import datetime

class Database():
    def __init__(self):
        pass
    def create(self,folder):
        #os.system skip errors just like bash
        print('set up database folder '+folder)
        os.system("mkdir "+folder)
        for i in range(10):#index
            print('mkdir i'+str(i))
            os.system("mkdir "+folder+"/i"+str(i))
            for n in range(4,31):
                os.system("mkdir "+folder+"/i"+str(i)+"/n"+str(n))
                for k in range(1,n-1):
                    os.system("mkdir "+folder+"/i"+str(i)+"/n"+str(n)+"/k"+str(k))
        print("created database for folder: {}, i=0..10, n=4..31,k=1..n-1".format(folder))
    def get_filelist(self):
        pass
    def get_filelist_full(self):
        pass





def dot_row(row):
    s=''
    for i in str(row):
        #replace 0 and , for better display
        if i == '0' or i == ',':
            s += ' '
#        elif i == '9':
#            s += '.'
        else:
            s += i
#    return str(row)
    return s

def pretty_2D_list(table):
    n=0
    string = ''
    for row in table[n:]:
#        print('    n='+str(n)+':\t'+dot_row(row))
        string += 'n='+str(n)+':\t'+dot_row(row) +'\n'
        n+=1
    return string
#        print(row)
#        print(''.join(str(row)))

# not in use. too slow compared to system bash commands
def get_json_list():
    path_of_the_directory = '../data/CSS-Codes/run1'
    ext = ('.json')
    for files in os.scandir(path_of_the_directory):
        if files.path.endswith(ext):
            print(files) 
            print(files.name)
            print(files.path[:-5])
            break

#get_json_list()
#exit()

def main():
    print(r'''    This function extract max [n,k,d] and save d in a 2D array/list with respect to n and k.
    Invalid json will be skipped and reported. They exist due to parallel file writing when generating codes on qlab''')
#    data_folder = "json1"
#    data_folder = "codes" 
#    data_folder = '../data/CSS-Codes/run2'
    trash_folder = '../data/CSS-Codes/trash'
#    filename_list = '../data/CSS-Codes/filelist-run1.txt'
    filename_list = 'filelist-run3.txt'  #./get-filelist.sh
    log_file = 'run.log'

#    print('data_folder:\t'+data_folder)
    print('trash_folder:\t'+trash_folder)
    print('filename_list:\t'+filename_list)
    print('log_file:\t'+log_file)
    print(datetime.datetime.now())

    num_of_codes=0   
    n,k,d=-1,-1,-1
    #save max distance for each (n,k)
    max_distance_table=[[ 0 for k in range(28)] for n in range(31) ]
    max_distance_table[0]=[_ % 10 for _ in range(28)]

    #save index for each (n,k,d), to see if the database is complete
    max_distance_table_index=[[ 0 for k in range(28)] for n in range(31) ]
    max_distance_table_index[0]=[_ % 10 for _ in range(28)]
    # d=table[n][k]

    for line in fileinput.input(filename_list):
        num_of_codes += 1
        if (num_of_codes % 10000 == 0):
            print('finish processing '+str(num_of_codes)+' codes')
        try:
            filename0=line[:-1]
#            with open(data_folder+'/'+ line[:-1]+'','r') as fjson:
            with open(filename0,'r') as fjson:
                js = json.load(fjson)
                n,k,d=js['n'],js['k'],js['d']
                if d > max_distance_table[n][k]:
                    max_distance_table[n][k] = d
                    #os.path.isfile(data_folder+'/'+ line[:-1]+'')
                    #get max index for this particular (n,k,d)
                    for i in range(10):
                        filename1=filename0[:24]+str(9-i)+filename0[25:-6]+str(9-i)+'.json'
#                        print(filename1)
#                        filename=data_folder+'/'+ line[:-7]+str(9-i)+'.json'
                        if os.path.isfile(filename1):
#                            print(filename1, 'is file')
                            max_distance_table_index[n][k] = (9-i+1) %10
                            break

        except:
            # report invalid files
            print('invalid json: '+line)
            return
            # move it to trash; ONLY for this when it is SAFE
            # os.system('mv '+ data_folder+'/'+ line[:-1]+' ' +data_folder+'/'+ line[:-6]+'Gx.mm '+data_folder+'/'+ line[:-6]+'Gz.mm ' +trash_folder)
            # print(line+' moved to trash')

    #count percantage. compare to the case with all index 10/0
    total, actual = 0, 0  #add up indexes
    entry_count, entry_total = 0, 0 #add up entries. 10/0 for completed, otherwise not    
    for n in range(5,31):
        for k in range(1,n-3):
            i=max_distance_table_index[n][k] #0 for complete, 1-9 for incomplete
            total+=10
            entry_total += 1
            if i > 0:
                actual+=i
            else: #entry completed with 10 instances, denoted as 0
                actual+=10
                entry_count += 1 
    percentage=1.0*actual/total
    entry_percentage = 1.0 * entry_count / entry_total

    string = filename_list+", " \
    + str(datetime.datetime.now())+"\n" \
    + 'Completeness table, ("0" omitted), percentage = {}/{} = {:.2f}%, entry_percentage = {}/{} = {:.2f}%\n'.format(actual, total, percentage*100, entry_count, entry_total, entry_percentage*100) \
    + pretty_2D_list(max_distance_table_index)    

    print(string)
    with open(log_file,'a') as f:
        f.write(string)

    string = ('[n,k,d] table\n') \
    + pretty_2D_list(max_distance_table) \
    + "total number of codes: {}\n".format(num_of_codes) \
    + str(datetime.datetime.now())
    print(string)
    with open(log_file,'a') as f:
        f.write(string)


def json_test():
        with open('codes/n15k3d2-x5z7dx3dz2-1.json','r') as fjson:
            print(fjson.read())
            js = json.load(fjson)
            print(js)
            n,k,d=js['n'],js['k'],js['d']
            if d > max_distance_table[n][k]:
                max_distance_table[n][k] = d

def database_test():
    db=Database()
    data_folder = '../data/CSS-Codes/run4'
    data_folder = '../data/CSS-Codes/tmp'
    db.create(data_folder)
    #takes 30 seconds to finish


#database_test()
#recover()
main()

    
