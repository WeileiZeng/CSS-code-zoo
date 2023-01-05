# copy all json files to another folder. not in use any more
date

trial=run3
folder=../data/CSS-Codes/${trial}

#ls ${folder} |wc -l
echo start generating json file list for folder ${folder}
echo "currently only counting file with index 0, *-0.json"
echo this may take ~1 minute


#find ${folder} -name "*json" | cut -c 24- > filelist-${trial}.txt 
#reduce file amount to 1/10

#find ${folder} -name "*-0.json" | cut -c 24- > filelist-${trial}.txt 
find ${folder} -name "*-0.json" > filelist-${trial}.txt 

echo "number of files: `wc -l filelist-${trial}.txt`" 
date

#exit
echo "full list"
#find ${folder} -name "*.json" | cut -c 24- > filelist-${trial}-full.txt 
find ${folder} -name "*.json"  > filelist-${trial}-full.txt 
echo "number of files: `wc -l filelist-${trial}-full.txt`" 
date

echo finish


# Sample run
# run1 contains 214948 files with 71647 json files 
# takes 30 secs to generate the list
