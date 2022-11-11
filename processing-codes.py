#!python3
# processing the json files for CSS codes

import fileinput
import json

def main():
    data_folder = "json1"
#    data_folder = "codes"
    filename_list = "filename-list.txt"
    count=0
    max_distance_table=[[ -1 for k in range(20)] for n in range(35) ]

    n,k,d=-1,-1,-1
    for line in fileinput.input(filename_list):
#        print(line)
      count +=1
      try:
        with open(data_folder+'/'+ line[:-1],'r') as fjson:
            js = json.load(fjson)
#            print(js)
            n,k,d=js['n'],js['k'],js['d']
            if d > max_distance_table[n][k]:
                max_distance_table[n][k] = d
#            for _key in ['n','k','d']:
#                print([_key,js[_key]])
                
#            print jj['n']
#        break
      except:
          print('invalid json: '+line)
    print('Done')
    print(max_distance_table[8:15])
    print(["total number of files:", count])



def test():
        with open('codes/n15k3d2-x5z7dx3dz2-1.json','r') as fjson:
            print(fjson.read())
            js = json.load(fjson)
            print(js)
            n,k,d=js['n'],js['k'],js['d']
            if d > max_distance_table[n][k]:
                max_distance_table[n][k] = d

#test()
#exit()

main()

    
