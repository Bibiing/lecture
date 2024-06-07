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
