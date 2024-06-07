awk -F'\t' '{if (split($5, words, " ") > 20) {gsub("_", " ", $2); print $1, $2}}' DataPKM.tsv
