create_zip(){
    folders=(folder_*)
    zip_counter=1

    for folder in "${folders[@]}"; do
        zip_folder="ayang_${zip_counter}.zip"
        zip -r "$zip_folder" "$folder"
        ((zip_counter++))
    done
}
