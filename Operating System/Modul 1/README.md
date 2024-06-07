### Daftar Soal _(Task List)_

- [Task 1 - Tim Kawal Bubu _(Bubu's Monitoring Team)_](task-1/)
- [Task 4 - LDR Isabel _(Isabel Sad Relationship)_](task-4/)

### Laporan Resmi Praktikum Modul 1 _(Module 1 Lab Work Report)_


<h2>Task 1</h2>
<p>Setelah masa upload proposal PKM 2024, Bubu sebagai sebagai anggota tim kawal melakukan scraping data upload dan mendapat file dalam format comma separated value (.csv). Bubu Pun mengirimkan <a href="https://github.com/sisop-its-s24/praktikum-modul-1-a15/blob/main/resources/DataPKM.tsv">file</a> tersebut ke komandan PKM, namun komandan memiliki kesulitan untuk membaca data tersebut. Beliau mendelegasikan Bubu untuk menganalisis data tersebut. Karena kamu adalah Suhu dalam perlinux-an menurut Bubu maka ia spontan membuat challenge untukmu mengerjakan task tersebut seluruhnya menggunakan shell scripting. Sebagai teman yang baik kamu pun menerima challenge itu. Adapun analisis yang diminta oleh komandan adalah sebagai berikut:</p>
<!-- =============== 1a =============== -->
<h3>Point A</h3>
<p>Karena Belmawa menetapkan judul maksimum proposal 20 kata, maka komandan ingin mencari data siapa saja tim yang tidak memenuhi ketentuan ini. Tampilkan nama pengusul, beserta departemennya yang judulnya lebih dari 20 kata. Pisahkan spasi dan hapus underscore "_" pada nama pengusul.</p>
<p>Input: </p>

```bash
awk -F'\t' '{if (split($5, words, " ") > 20) {gsub("_", " ", $2); print $1, $2}}' DataPKM.tsv
```
<p>Output: </p>
<img src="https://github.com/sisop-its-s24/praktikum-modul-1-a15/blob/main/resources/1a.png">

<p>Penjelasan: </p>
<ul>
  <li>
    <code>awk</code> salah satu command yang dimiliki linux, yang digunakan untuk memanipulasi data dan menghasilkan laporan.
  </li>
  
  <li>
    <code>-F'\t'</code> Option '-F' merupakan salah satu command option dari awk yang digunakan untuk menentukan pemisah kolom dalam file input. Kasus disini '\t' sebagai pemisah antar kolom.
  </li>
<li>
  <code>if (split($5, words, " ") > 20)</code> fungsi Awk yang digunakan untuk membagi teks dalam ($5) menjadi beberapa kata, menggunakan spasi sebagai pemisah. Hasilnya disimpan dalam array words. Jika jumlah kata > 20 maka akan mengembalikan nilai true, jika tidak mengembalikan nilai false. Jika true lanjut ke proses selajutnya.
</li>
<li>
  <code>{gsub("_", " ", $2); print $1, $2}</code> fungsi gsub mengubah underscore ('_') pada kolom $2 menjadi spasi. Lalu akan melakukan print kolom pertama dan kedua.  
</li>
<li>
  <code>DataPKM.tsv</code> input file, merupakan file yang akan diproses oleh awk.
</li>
  
</ul>
<!-- =============== 1b =============== -->
<h3>Point B</h3>
<p>Komandan PKM juga tertarik ingin tahu antusiasme dan partisipasi mahasiswa sehingga meminta Bubu menampilkan bidang paling banyak diminati oleh mahasiswa. Tampilkan nama skema saja.</p>
<p>Input: </p>

```bash
awk -F'\t' '{count[$NF]++} END {max = 0; for (i in count) {if (count[i] > max) {max = count[i]; most_frequent = i}} print most_frequent}' DataPKM.tsv
```

<p>Output: </p>
<img src="https://github.com/sisop-its-s24/praktikum-modul-1-a15/blob/main/resources/1b.png">


<p>Penjelasan: </p>
<ul>
  <li>
    <code>awk</code> salah satu command yang dimiliki linux, yang digunakan untuk memanipulasi data dan menghasilkan laporan.
  </li>
  
  <li>
    <code>-F'\t'</code> Option '-F' merupakan salah satu command option dari awk yang digunakan untuk menentukan pemisah kolom dalam file input. Kasus disini '\t' sebagai pemisah antar kolom.
  </li>
  
  <li>
    <code>$NF</code> merupakan salah satu variabel dari awk yang merepresentasikan nilai terakhir (kolom terakhir) dari setiap baris.
  </li>
  
  <li>
<code>{count[$NF]++}</code> bertugas untuk menghitung berapa kali setiap nilai dari kolom terakhir (skema) muncul dalam data yang diproses.Dengan menggunakan array asosiatif 'count' dan menggunakan $NF sebagai kunci. Setiap kali nilai tersebut muncul, jumlahnya akan ditambahkan satu dalam array count. Jika nilai tersebut belum ada dalam array, awk akan membuat entri baru dengan nilai awal 0, kemudian nilai tersebut akan diinkremenkan saat nilai tersebut muncul pertama kali.
  </li>
  
  <li>
<code>END</code> merupakan blok yang dijalankan setelah semua baris file telah dibaca. 
  </li>
  
  <li>
    (<code>max = 0</code>) untuk menampung nilai maksimum yang akan ditemukan.
  <li>
    
(<code>count[i]</code>) di dalam blok END, dilakukan loop melalui setiap kunci dalam array 'count'. 
  </li>
  
  <li>
    <code>if (count[i] > max</code> pada setiap iterasi, dilakukan pengecekan apakah jumlah kemunculan nilai saat ini lebih besar dari nilai maksimum yang telah ditemukan
  </li>
  
  <li>
    <code>{max = count[i]; ; most_frequent = i}</code> Jika ya, nilai maksimum dan most_frequent diperbarui dengan jumlah kemunculan nilai terakhir yang baru ditemukan.
  </li> 
  
  <li> 
    <code>print most_frequent</code> setelah selesai loop, nilai yang paling sering muncul dicetak.
  </li>
  
  <li>
    <code>DataPKM.tsv</code> input file, merupakan file yang akan diproses oleh awk.
  </li>
</ul>
<!-- =============== 1c =============== -->
<h3>Point C</h3>
<p>Karena ada aturan baru dimana 1 mahasiswa hanya boleh submit 1 proposal, maka komandan juga meminta Bubu untuk memberikan list mahasiswa yang mengajukan 2 proposal. Tampilkan data pembimbingnya karena ingin di kontak komandan.</p>
<p>Input: </p>

```bash
awk -F"\t" 'NR==FNR{a[$2]++; next} a[$2]>1 {print $6}' 'DataPKM.tsv' 'DataPKM.tsv' | sort
```
<p>Output: </p>
<img src="https://github.com/sisop-its-s24/praktikum-modul-1-a15/blob/main/resources/1c.png">


<p>Penjelasan: </p>
<ul>
    <li>
    <code>awk</code> salah satu command yang dimiliki linux, yang digunakan untuk memanipulasi data dan menghasilkan laporan.
  </li>
  
  <li>
    <code>-F'\t'</code> Option '-F' merupakan salah satu command option dari awk yang digunakan untuk menentukan pemisah kolom dalam file input. Kasus disini '\t' sebagai pemisah antar kolom.
  </li>
  <li>
    <code>NR</code> (Nomor Record) sebuah variabel yang mengacu pada jumlah total baris yang telah diproses oleh awk.
  </li>
  
  <li>
    <code>FNR</code> (Nomor Record File) adalah nomor baris relatif dalam satu file.
  </li>
  <li>
    <code>NR==FNR</code> awk memproses file pertama, jika NR == FNR sudah tidak terpenuhi lanjut ke file ke-2.
  </li>
  
  <li>
    <code>{a[$2]++; next}}</code> menghitung jumlah proposal yang di ajukan oleh setiap mahasiswa dengan menggunakan array a[$2] kolom kedua. setelah itu next digunakan untuk melanjutkan ke baris berikutnya.
  </li>
  
  <li>
    <code>a[$2]>1 {print $6}</code> awk memproses file kedua. Memeriksa apakah jumlah yang diajukan oleh mahasiswa tersebut lebih dari satu. Jika lebih dari satu maka nama pembimbing (kolom ke-enam, '$6') akan dicetak.
  </li>
  
  <li>
    <code>'DataPKM.tsv' 'DataPKM.tsv'</code> merupakan file pertama dan ke-dua.
  </li>
  <li>
    <code>sort</code> akan mengurutkan output
  </li>
</ul>

<h2>Task 4</h2>
<p>Isabel sedang LDR dengan pacarnya dan sangat rindu. Isabel ingin menyimpan semua foto-foto yang dikirim oleh pacarnya. Bantulah Isabel menyimpan gambar "Mingyu Seventeen”.</p>

<!-- =============== 4a =============== -->
<h3>Point A</h3>
<p>Berikut adalah ketentuan yang diinginkan Isabel:</p>
<ul>
  <li>
    Gambarnya di download setiap 5 jam sekali mulai dari saat script dijalankan dan memperhatikan waktu sekarang. Jika waktu genap, maka simpan foto sebanyak 5x. Jika waktu ganjil, maka simpan foto sebanyak 3x. (Contoh : pukul sekarang 12.38, maka pukul 12 adalah genap sehingga simpan foto sebanyak 5x).
  </li>
  <li>
    Jika pukul menunjukkan 00.00, maka simpan foto sebanyak 10x.
  </li>
  <li>
    File yang didownload memiliki format nama foto_NOMOR.FILE dengan NOMOR.FILE adalah urutan file yang download (foto_1, foto_2, dst)
  </li>
  <li>
    File batch yang didownload akan dimasukkan ke dalam folder dengan format nama folder_NOMOR.FOLDER dengan NOMOR.FOLDER adalah urutan folder saat dibuat (folder_1, folder_2, dst)
  </li>
</ul>
<p>Input: </p>

```bash
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
```

<p>Output: </p>
<img src="https://github.com/sisop-its-s24/praktikum-modul-1-a15/blob/main/resources/folder.png">
<img src="https://github.com/sisop-its-s24/praktikum-modul-1-a15/blob/main/resources/foto.png">

<p>Penjelasan: </p>
<ul>
  <li>
    <code>image_counter=1</code> untuk nama dari file image </code> <code>folder_counter=1</code> untuk nama folder <code>current_time=$(date +"%H.%M")</code> mengambil jam dan menit <code>hour=$(date +"%H")</code> mengambil jam <code>isEvenHour=$((hour % 2))</code> mengambil jam genap
  </li>

  <li>
    <code>if [ "$current_time" == "00.00" ]; then download_count=10 </code>  pada kondisi pertama, jika jam sekarang adalah 00.00 maka counter = 10.
    <code>elif [ "$isEvenHour" -eq 0 ]; then download_count=5</code>  kondisi ke dua jika jamnya genap maka counter = 5.
    <code>else download_count=3 fi</code> selain itu counter akan dibuat = 3
  </li>

  <li>
    <code>folder_name="folder_${folder_counter}" </code> membuat thamplate nama folder.
     <code>mkdir -p "$folder_name"</code> membuat folder sesuai thamplate
  </li>

  <li>
    <code> for ((i=1; i<=$download_count; i++)); do </code> for loop dimulai dari 1 sampai download_count.
    <code>image_name="foto_${image_counter}.jpg"</code> thamplate nama img.
    <code>image_path="${folder_name}/${image_name}"</code> path atau lokasi dari img.
    <code>wget -q -O "$image_path" "https://its.id/m/kucing-kalem"</code> download gambar
    <code>((image_counter++)) done</code> image_counter di increment.
  </li>
  
  <li>
    <code>wget</code> mengunduh file dari internet.
  </li>
  <li>
    <code>1</code> menjalankan wget secara quiet, tidak menampikal pesan pada konsol.
  </li>
  <li>
    <code>-O</code>  menentukan lokasi dan nama file yang diunduh.
  </li>
  <li>
    <code>((folder_counter++))</code> increment folder counter
  </li>
</ul>

<!-- =============== 4b =============== -->
<h3>Point B</h3>
<p>Karena memory laptop Isabel penuh, maka bantulah Isabel untuk zip foto-foto tersebut dengan ketentuan: <br> Isabel harus melakukan zip setiap 1 jam dengan nama zip ayang_NOMOR.ZIP dengan NOMOR.ZIP adalah urutan folder saat dibuat (ayang_1, ayang_2, dst). Yang di ZIP hanyalah folder dari soal di atas.</p>

<p>Input: </p>

```bash
create_zip(){
    folders=(folder_*)
    zip_counter=1

    for folder in "${folders[@]}"; do
        zip_folder="ayang_${zip_counter}.zip"
        zip -r "$zip_folder" "$folder"
        ((zip_counter++))
    done
}
```

<p>Output: </p>

<img src="https://github.com/sisop-its-s24/praktikum-modul-1-a15/blob/main/resources/zip1.png">
<img src="https://github.com/sisop-its-s24/praktikum-modul-1-a15/blob/main/resources/isiZip.png">

<p>Penjelasan: </p>
<ul>
  <li>
    <code>folders=(folder_*)</code> mengambil semua folder pada direktori yang sama dan disimpan pada folders.
    <code>zip_counter=1</code> counter untuk zip
  </li>
  <li>
    <code>for folder in "${folders[@]}"; do</code> loop dengan menyimpan setiap array pada folders ke folder.
    <code></code>zip_folder="ayang_${zip_counter}.zip"</code> thamplate nama zip.
    <code>zip -r "$zip_folder" "$folder"</code> melakukan zip.
    <code>((zip_counter++)) done</code> zip counternya di increment.
  </li>
  <li>
    <code>${folders[@]}</code> mengembalikan semua elemen pada array folders
  </li>
  <li>
    <code>zip</code> perintah untuk membuat arsip ZIP.
  </li>
  <li>
    <code> -r </code>  melakukan kompresi secara rekursif, mencakup semua file dan subfolder di dalam folder.
  </li>
  <li>
    <code> "$zip_folder" </code>  variabel yang berisi nama file ZIP yang akan dibuat
  </li>
  <li>
    <code> "$folder" </code>  variabel yang berisi nama folder yang akan di-zip.
  </li>
  
</ul>

<!-- =============== 4c =============== -->
<h3>Point C</h3>
<p>
  Ternyata laptop Isabel masih penuh, bantulah dia untuk delete semua folder dan zip setiap hari pada pukul 02.00!
</p>

<p>Input: </p>

```bash
cleanup(){
    rm -rf folder_*
    rm -f ayang_*.zip
}

```

<p>Output: </p>
<img src="https://github.com/sisop-its-s24/praktikum-modul-1-a15/blob/main/resources/4c1.png">


<p>Penjelasan: </p>
<ul>
  <li>
    <code>rm -rf folder_*</code> remove atau menghapus semua folder pada direktori yang sama.
  </li>
  <li>
    <code>rm -f ayang_*.zip</code> remove atau menghapus semua zip pada direktori yang sama.
  </li>
</ul>

<!-- =============== 4d =============== -->
<h3>Point D</h3>
<p>Untuk mengisi laptopnya kembali, Isabel ingin mendownload gambar Levi dan Eren secara bergantian setiap harinya dengan ketentuan nama file:</p>
<ul>
  <li>
    Jika Levi, maka nama file nya menjadi levi_YYYYMMDD (Dengan YYYYMMDD adalah tahun, bulan, dan tanggal gambar tersebut di download)
  </li>

  <li>
    ika Eren, maka nama file nya menjadi eren_YYYYMMDD (Dengan YYYYMMDD adalah tahun, bulan, dan tanggal gambar tersebut di download)
  </li>
</ul>

<p>Input: </p>

```bash

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
```

<p>Output: </p>
<img src="https://github.com/sisop-its-s24/praktikum-modul-1-a15/blob/main/resources/isabel.png">

<p>Penjelasan: </p>
<ul>
  <li>
    <code>download_levi() {current_date=$(date +"%Y%m%d") wget -q -O "levi_${current_date}.jpg" "https://its.id/m/levi"}</code> fungsi untuk download gambar levi
  </li>
  <li>
    <code>download_eren() {current_date=$(date +"%Y%m%d") wget -q -O "eren_${current_date}.jpg" "https://its.id/m/eren"}</code> fungsi untuk download gambar levi
  </li>
  <li>
    <code> if ls -l | grep -q "levi_$(date -d "yesterday" +"%Y%m%d").jpg"; then download_eren </code> jika dihari sebelumnya (kemaren) yang di download adalah levi, maka sekarang donwload eren.
    <code>else download_levi fi</code> selain itu download levi.
  </li>
</ul>
