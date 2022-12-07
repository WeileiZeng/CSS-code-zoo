echo "this display the last lines in log file"
for i in gen*; do tail -n 3 $i |head -n 1; done
