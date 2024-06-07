awk -F"\t" 'NR==FNR{a[$2]++; next} a[$2]==2 {print $6}' 'DataPKM.tsv' 'DataPKM.tsv' | sort
