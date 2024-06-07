awk -F'\t' '{count[$NF]++} END {max = 0; for (i in count) {if (count[i] > max) {max = count[i]; most_frequent = i}} print most_frequent}' DataPKM.tsv
