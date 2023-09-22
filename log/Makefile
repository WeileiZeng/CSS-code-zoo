clean:
	mv nkd*.log trash
#show-codes:
#echo "this display the last lines in log file, and one can check number of codes saved in each run."
#for i in nkd*; do tail -n 3 $i |head -n 1; done
show-save:
	echo "this count the lines with 'save', which is the same as number of codes saved during this trial"
	grep Saved *.log | wc -l
print-codes:
	grep Saved *.log
show-save-log:
	bash show-save.sh >>clean-log.txt
	date >> clean-log.txt

