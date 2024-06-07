download_images(){
    image_counter=1
    folder_counter=1
    current_time=$(date +"%H.%M")
    hour=$(date +"%H")
    isEvenHour=$((hour % 2))

    if [ "$current_time" == "00.00" ]; then
        download_count=10
    elif [ "$isEvenHour" -eq 0 ]; then
        download_count=5
    else
        download_count=3
    fi

    folder_name="folder_${folder_counter}"
    mkdir -p "$folder_name"

    for ((i=1; i<=$download_count; i++)); do
        image_name="foto_${image_counter}.jpg"
        image_path="${folder_name}/${image_name}"
        wget -q -O "$image_path" "https://its.id/m/kucing-kalem"
        ((image_counter++))
    done

    ((folder_counter++))
}

create_zip(){
    folders=(folder_*)
    zip_counter=1

    for folder in "${folders[@]}"; do
        zip_folder="ayang_${zip_counter}.zip"
        zip -r "$zip_folder" "$folder"
        ((zip_counter++))
    done
}

cleanup(){
    rm -rf folder_*
    rm -f ayang_*.zip
}

wibu(){
    download_levi() {
        current_date=$(date +"%Y%m%d")
        wget -q -O "levi_${current_date}.jpg" "https://its.id/m/levi"
    }

    download_eren() {
        current_date=$(date +"%Y%m%d")
        wget -q -O "eren_${current_date}.jpg" "https://its.id/m/eren"
    }

    if ls -l | grep -q "levi_$(date -d "yesterday" +"%Y%m%d").jpg"; then
        download_eren
    else
        download_levi
    fi
}


if [ "$1" == "download_images" ]; then
    download_images
elif [ "$1" == "create_zip" ]; then
    create_zip
elif [ "$1" == "cleanup" ]; then
    cleanup
elif [ "$1" == "wibu" ]; then
    wibu
fi
