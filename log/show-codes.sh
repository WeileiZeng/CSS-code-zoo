echo "this display the last lines in log file, and one can check number of codes saved in each run."
for i in gen*; do tail -n 3 $i |head -n 1; done
