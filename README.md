# SoalShift_modul3_C04

##  No. 1
1.  Buatlah program C yang bisa menghitung faktorial secara parallel lalu menampilkan hasilnya secara berurutan
Contoh:
    ./faktorial 5 3 4
    3! = 6
    4! = 24
    5! = 120

PENJELASAN:
-   Pertama kita buat array untuk menampung nilai-nilai yg akan di input dan membuat tid dengan maksimal 100 thread
```
int array[100];
pthread_t tid[100];
```

-   Selanjutnya dibuat sebuah struct yang dapat menampung nilai yang akan di-faktorial-kan
```
struct Factorial{
	int value;
};
```

-   Kemudian dibuat fungsi untuk menghitung nilai faktorial dan mem-printkan hasilnya, disni variable fctr akan menyimpan nilai akhir dari faktorial, dengan input berupa pointer yang berisi suatu nilai. adapun kodingan fungsinya seperti dibawah ini:
```
void* tulis(void* args){
	unsigned long long fctr=1;
    	struct Factorial*extract =(struct Factorial*)args;
 	for(int i=1; i<=extract->value; ++i)
        {
         	fctr = i*fctr;          
        }
   	printf("\n");
    	printf("%d! = %llu\n", extract->value, fctr);
}
```

-   Untuk program utamanya, pertama kita buat variable n yang akan menyimpan banyaknya input nilai, variable i dan j sebagai iterator, variable urutan sebagai iterator urutan dari thread, tmp digunakan pada sorting, dan blank untuk membantu proses input dari user. Seperti berikut:
```
int n=0, i=0, j=0, urutan, tmp;
char blank;
```

-   Untuk mengambil input dari user, karena belum tentu banyaknya input, maka diambil nilai-nilai yang dipisahkan oleh spasi sampai pada akhir line (\n). Seperti berikut:
```
do {
    scanf("%d%c", &array[i], &blank);
    i++;
		n++;
} while(blank != '\n');
```

-   Untuk proses sorting, kami menggunakan bubble sort seperti berikut:
```
for (i = 0; i<n-1; ++i) 
{
        for (j = i + 1; j<n; ++j)
        {
            if (array[i] > array[j]) 
            {
                tmp =  array[i];
                array[i] = array[j];
                array[j] = tmp;
            }
        }     
}
```

-   Untuk proses threading, kami menggunakan loop untuk mengcreate thread yang akan menhitung nilai faktorial dan melakukan join thread setelahnya seperti berikut:
```
while(i<n) {
 		factorial.value = array[i];
 		urutan=i;
 		pthread_create(&tid[urutan], NULL, &tulis, (void *)&factorial);
    pthread_join(tid[urutan], NULL);
    i++;
}
```


##  No. 2
2.  Pada suatu hari ada orang yang ingin berjualan 1 jenis barang secara private, dia memintamu membuat program C dengan spesifikasi sebagai berikut:
- Terdapat 2 server: server penjual dan server pembeli
- 1 server hanya bisa terkoneksi dengan 1 client
- Server penjual dan server pembeli memiliki stok barang yang selalu sama
- Client yang terkoneksi ke server penjual hanya bisa menambah stok
    - Cara menambah stok: client yang terkoneksi ke server penjual mengirim string “tambah” ke server lalu stok bertambah 1
- Client yang terkoneksi ke server pembeli hanya bisa mengurangi stok
    - Cara mengurangi stok: client yang terkoneksi ke server pembeli mengirim string “beli” ke server lalu stok berkurang 1
- Server pembeli akan mengirimkan info ke client yang terhubung dengannya apakah transaksi berhasil atau tidak berdasarkan ketersediaan stok
    - Jika stok habis maka client yang terkoneksi ke server pembeli akan mencetak “transaksi gagal”
    - Jika stok masih ada maka client yang terkoneksi ke server pembeli akan mencetak “transaksi berhasil”
- Server penjual akan mencetak stok saat ini setiap 5 detik sekali
- Menggunakan thread, socket, shared memory

PENJELASAN

### Server Pembeli
- Untuk Server Pembeli akan menggunakan port 8080 maka di-define seperti berikut
```
#define PORT 8080
```

- Selanjutnya dibuatkan program server seperti berikut:
```
int server_fd, new_socket, valread;
    	struct sockaddr_in address;
    	int opt = 1;
    	int addrlen = sizeof(address);
    	char buffer[1024] = {0};    	
      
    	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        	perror("socket failed");
        	exit(EXIT_FAILURE);
    	}
      
    	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        	perror("setsockopt");
        	exit(EXIT_FAILURE);
    	}

    	address.sin_family = AF_INET;
    	address.sin_addr.s_addr = INADDR_ANY;
    	address.sin_port = htons( PORT );
      
    	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        	perror("bind failed");
        	exit(EXIT_FAILURE);
    	}

    	if (listen(server_fd, 3) < 0) {
        	perror("listen");
        	exit(EXIT_FAILURE);
    	}

    	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        	perror("accept");
        	exit(EXIT_FAILURE);
    	}
```

- Karena stok sama antara server pembeli dan server penjual, perlu dibuat shared memory, dalam program ini, variable value akan menunjukkan banyak stok.
```
key_t key = 1234;
int *value;

int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666); 
value = shmat(shmid, NULL, 0);
```

- Jika Client pembeli mengirimkan pesan beli ke server, maka nilai value akan berkurang jika terdapat stok (value > 0) dan mengirimkan pesan transaksi berhasil ke client, namun jika stok sudah habis, maka akan mengirimkan pesan transaksi gagal ke client. jika pesan yang dikirimkan oleh client merupakan invalid query, dalam kasus ini bukan beli, maka server akan mengirimkan pesan Invalid Query
```
char *invalid = "Invalid Query";
char *success = "Transaksi Berhasil";
char *fail = "Transaksi Gagal";

valread = read( new_socket , buffer, 1024);
if(strcmp(buffer,"beli")==0){
    if(*value>0){
        *value-=1;
        send(new_socket, success, strlen(success), 0);
    }
    else{
        send(new_socket, fail, strlen(fail) , 0);
    }
}
else{
    send(new_socket, invalid, strlen(invalid), 0);
}
```

### Server Penjual
- Untuk Server Penjual akan menggunakan port 8000 maka di-define seperti berikut:
```
#define PORT 8000
```

- Selanjutnya dibuat fungsi yang akan memprint nilai stok setiap 5 detik seperti berikut:
```
void* tulis(void *arg)
{
    	int i = 0;
    	while(1){
       	 	printf("stok saat ini : %d\n", *value);
        	sleep(5);
    	}
}
```

- Agar Fungsi tersebut terus berjalan, maka dicreate thread dengan cara seperti berikut:
```
pthread_create(&(tid1), NULL, tulis, NULL);
```

- Sama seperti Server Pembeli, dibuat program server terlebih dahulu
- Karena menggunakan shared memory, maka digunakan kodingan yang sama persis pada bagian shared memory Server Pembeli.
- Jika Server Penjual menerima pesan tambah dari client, maka nilai value akan bertambah dan server mengirimkan pesan berhasil menambah ke client. adapun untuk query yang salah maka server akan mengirimkan pesan Invalid Query kemudian clear buffer
```
valread = read( new_socket , buffer, 1024);
if(strcmp(buffer,"tambah")==0){
      *value+=1;
      send(new_socket, success, strlen(success), 0);
}
else{
      send(new_socket, invalid, strlen(invalid), 0);
}
memset(buffer,0,1024); 
```

### Client Pembeli
- Agar terhubung dengan server pembeli, maka port yang digunakan adalah port 8080
```
#define PORT 8080
```

- Selanjutnya dibuat program client seperti berikut:
```
      struct sockaddr_in address;
    	int sock = 0, valread;
    	struct sockaddr_in serv_addr;
    	char query[9];
    	char buffer[1024] = {0};
    	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        	printf("\n Socket creation error \n");
        	return -1;
    	}
  
    	memset(&serv_addr, '0', sizeof(serv_addr));
  
    	serv_addr.sin_family = AF_INET;
    	serv_addr.sin_port = htons(PORT);
      
    	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        	printf("\nInvalid address/ Address not supported \n");
        	return -1;
    	}
  
    	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        	printf("\nConnection Failed \n");
        	return -1;
    	}
```

- Kemudian client akan terus meminta input pesan yang akan dikirimkan ke server sehingga dimasukkan ke loop while proses mengirim dan menerimanya seperti berikut:
```
      while(1){
        	scanf("%s", query);
        	send(sock, query, strlen(query), 0);
        	valread = read(sock, buffer, 1024);
        	printf("%s\n",buffer );
        	memset(buffer,0,1024);
    	}  
```

### Client Penjual
- Agar terhubung dengan server penjual, maka port yang digunakan adalah port 8000
```
#define PORT 8000
```
- Sama seperti client pembeli, dibuat program client terlebih dahulu yang sama persis, bedanya hanya port yang terkoneksi dimana client pembeli terhubung dengan server pembeli pada port 8080, client penjual terhubung dengan server pembeli pada port 8000


##  No. 3
3.  Agmal dan Iraj merupakan 2 sahabat yang sedang kuliah dan hidup satu kostan, sayangnya mereka mempunyai gaya hidup yang berkebalikan, dimana Iraj merupakan laki-laki yang sangat sehat,rajin berolahraga dan bangun tidak pernah kesiangan sedangkan Agmal hampir menghabiskan setengah umur hidupnya hanya untuk tidur dan ‘ngoding’. Dikarenakan mereka sahabat yang baik, Agmal dan iraj sama-sama ingin membuat satu sama lain mengikuti gaya hidup mereka dengan cara membuat Iraj sering tidur seperti Agmal, atau membuat Agmal selalu bangun pagi seperti Iraj. Buatlah suatu program C untuk menggambarkan kehidupan mereka dengan spesifikasi sebagai berikut:
- Terdapat 2 karakter Agmal dan Iraj
- Kedua karakter memiliki status yang unik
    - Agmal mempunyai WakeUp_Status, di awal program memiliki status 0
    - Iraj memiliki Spirit_Status, di awal program memiliki status 100
    - Terdapat 3 Fitur utama
        - All Status, yaitu menampilkan status kedua sahabat
          Ex: Agmal WakeUp_Status = 75 
          Iraj Spirit_Status = 30
        - “Agmal Ayo Bangun” menambah WakeUp_Status Agmal sebesar 15 point
        - “Iraj Ayo Tidur” mengurangi Spirit_Status Iraj sebanyak 20 point
- Terdapat Kasus yang unik dimana:
    - Jika Fitur “Agmal Ayo Bangun” dijalankan sebanyak 3 kali, maka Fitur “Iraj Ayo Tidur” Tidak bisa dijalankan selama 10 detik (Dengan mengirim pesan ke sistem “Fitur Iraj Ayo Tidur disabled 10 s”)
    -Jika Fitur  “Iraj Ayo Tidur” dijalankan sebanyak 3 kali, maka Fitur “Agmal Ayo Bangun” Tidak bisa dijalankan selama 10 detik (Dengan mengirim pesan ke sistem “Agmal Ayo Bangun disabled 10 s”)
- Program akan berhenti jika Salah Satu :
    -WakeUp_Status Agmal >= 100 (Tampilkan Pesan “Agmal Terbangun,mereka bangun pagi dan berolahraga”)
    -Spirit_Status Iraj <= 0 (Tampilkan Pesan “Iraj ikut tidur, dan bangun kesiangan bersama Agmal”)
- Syarat Menggunakan Lebih dari 1 Thread

PENJELASAN
-   Pertama kami membuat 2 thread dan beberapa variable, WakeUp_Status untuk status Agmal, Spirit_Status untuk status Iraj, flag untuk menandai apakah program sudah atau belum berakhir, j untuk counter Iraj, i untuk counter Agmal, flag_agmal untuk menunjukkan apakah Agmal Ayo Bangun dapat digunakan, flag_Iraj untuk menunjukkan apakah Iraj Ayo Tidur dapat digunakan.
```
pthread_t tid1, tid2;
int WakeUp_Status = 0;
int Spirit_Status = 100;
int flag=0, i=0, j=0, flag_agmal=0, flag_iraj=0;
```

-   Selanjutnya membuat fungsi Agmal dan Iraj untuk membuat fiturnya tidak dapat dijalankan selama 10 detik seperti berikut:
```
void* Agmal(void *arg)
{
    	while(1){
        	if(flag_agmal==1){
            		sleep(10);
			flag_agmal=0;
        	}
    	}
}

void* Iraj(void *arg)
{
	while(1){
        	if(flag_iraj==1){
           		sleep(10);
            		flag_iraj=0;
        	}
    	}
}
```

-   Pada program utama, dicreate 2 thread agar 2 fungsi diatas tadi dapat terus berjalan
```
pthread_create(&(tid1), NULL, Agmal, NULL);
pthread_create(&(tid2), NULL, Iraj, NULL);
```

-   Jika query yang digunakan adalah "Agmal Ayo Bangun" maka dicek terlebih dahulu apakah flag_akmal bernilai 0, jika benar maka WakeUp_Status akan bertambah 15 dan i akan bertambah. jika flag_agmal 1, maka statusnya tidak akan bertambah karena tidak dapat digunakan, kemudian print "Agmal Ayo Bangun disabled 10 s"
```
if(strcmp(query,"Agmal Ayo Bangun") == 0){
            		if(flag_agmal==0){
                		WakeUp_Status += 15;
                		i++;
            		}else{
                		printf("Agmal Ayo Bangun disabled 10 s \n");
            		}
}
```

-   Sama seperti tadi, namun querynya adalah "Iraj Ayo Tidur", dicek dan berjalan sesuai flag_iraj
```
else if(strcmp(query,"Iraj Ayo Tidur") == 0){
            		if(flag_iraj==0){
                		Spirit_Status -= 20;
                		j++;
            		}else{
                	printf("Fitur Iraj Ayo Tidur disabled 10 s \n");
            		}
}

-   Jika querynya adalah "All Status" maka akan memprint kedua statusnya seperti berikut:
```
else if(strcmp(query,"All Status") == 0){
			printf("Agmal WakeUp_Status = %d\n",WakeUp_Status);
        		printf("Iraj Spirit_Status = %d\n",Spirit_Status);
			//printf("i=%d j=%d \n", i, j);
}

-   Jika counter sudah sampai 3, maka fitur ayo bangun dan ayo tidur tadi tidak akan dapat dijalankan dan mereset counter kembali ke 0 seperti berikut:
```
if(j==3){
      flag_agmal=1;
      j=0;
}
if(i==3){
      flag_iraj=1;
      i=0;
}
```

-   Ketika WakeUp_Status sudah sampai 100 atau Spirit_Status sudah sampai 0 maka program akan memprint pesan dan program akan berakhir
```
if(WakeUp_Status >= 100){
      printf("Agmal Terbangun,mereka bangun pagi dan berolahraga\n");
			flag = 1;
}
if(Spirit_Status <= 0){
      printf("Iraj ikut tidur, dan bangun kesiangan bersama Agmal\n");
			flag = 1;
}
```

##  No. 4
4.	Buatlah sebuah program C dimana dapat menyimpan list proses yang sedang berjalan (ps -aux) maksimal 10 list proses. Dimana awalnya list proses disimpan dalam di 2 file ekstensi .txt yaitu  SimpanProses1.txt di direktori /home/Document/FolderProses1 dan SimpanProses2.txt di direktori /home/Document/FolderProses2 , setelah itu masing2 file di  kompres zip dengan format nama file KompresProses1.zip dan KompresProses2.zip dan file SimpanProses1.txt dan SimpanProses2.txt akan otomatis terhapus, setelah itu program akan menunggu selama 15 detik lalu program akan mengekstrak kembali file KompresProses1.zip dan KompresProses2.zip.
	Dengan Syarat : 
	- Setiap list proses yang di simpan dalam masing-masing file .txt harus berjalan bersama-sama
	- Ketika mengkompres masing-masing file .txt harus berjalan bersama-sama
	- Ketika Mengekstrak file .zip juga harus secara bersama-sama
	- Ketika Telah Selesai melakukan kompress file .zip masing-masing file, maka program akan memberi pesan “Menunggu 15 detik untuk mengekstrak kembali”
	- Wajib Menggunakan Multithreading
	- Boleh menggunakan system
##  Penjelasan
```
void* membuat(void *arg)
{
    pthread_t id=pthread_self();
    iter=0;
    if(pthread_equal(id,tid[0]))//thread untuk menjalankan counter
    {
        system("ps aux --no-heading | head -10 > ~/Documents/FolderProses1/SimpanProses1.txt");   
    }
    else if(pthread_equal(id,tid[1]))
    {
        system("ps aux --no-heading | head -10 > ~/Documents/FolderProses2/SimpanProses2.txt");
    }
    iter=1;
    return NULL;
}
```
fungsi "membuat" digunakan untuk menyimpan list proses yang sedang berjalan (ps -aux) maksimal 10 list proses. Dimana awalnya list proses disimpan dalam di 2 file ekstensi .txt yaitu SimpanProses1.txt di direktori /home/Document/FolderProses1 dan SimpanProses2.txt di direktori /home/Document/FolderProses2
```
void* mengkompres(void *arg)
{
    while(iter!=1)
    {

    }
    pthread_t id=pthread_self();
    if(pthread_equal(id,tid[2]))//thread untuk menjalankan counter
    {
        system("zip -qmj ~/Documents/FolderProses1/KompresProses1 ~/Documents/FolderProses1/SimpanProses1.txt");
    }
    else if(pthread_equal(id,tid[3]))
    {
        system("zip -qmj ~/Documents/FolderProses2/KompresProses2 ~/Documents/FolderProses2/SimpanProses2.txt");
    }
    iter=2;
    return NULL;
}
```
funsi "mengkompres" digunakan untuk kompres zip dengan format nama file KompresProses1.zip dan KompresProses2.zip dan file SimpanProses1.txt dan SimpanProses2.txt akan otomatis terhapus
```
void* mengekstrak(void *arg)
{
    while(iter!=2)
    {

    }
    sleep(15);
    pthread_t id=pthread_self();
    if(pthread_equal(id,tid[4]))//thread untuk menjalankan counter
    {
        system("unzip -qd ~/Documents/FolderProses1 ~/Documents/FolderProses1/KompresProses1.zip");
    }
    else if(pthread_equal(id,tid[5]))
    {
        system("unzip -qd ~/Documents/FolderProses2 ~/Documents/FolderProses2/KompresProses2.zip");  
    }
    return NULL;
}
```
fungsi "mengekstrak" digunakan untuk mengekstrak kembali file KompresProses1.zip dan KompresProses2.zip
```
int main(void)
{
    int i=0;
    int err;
    while(i<2)//looping membuat thread 2x
    {
        err=pthread_create(&(tid[i]),NULL,&membuat,NULL);//membuat thread
        if(err!=0)//cek error
        {
            printf("\n can't create thread : [%s]",strerror(err));
        }
        else 
        {
            printf("\n create thread success");
        }
        i++;
    }
    while(i<4)//looping membuat thread 2x
    {
        err=pthread_create(&(tid[i]),NULL,&mengkompres,NULL);//membuat thread
        if(err!=0)//cek error
        {
            printf("\n can't create thread : [%s]",strerror(err));
        }
        else
        {
            printf("\n create thread success");
        }
        i++;
    }
    printf("\n Menunggu 15 detik untuk mengekstrak kembali");
    while(i<6)//looping membuat thread 2x
    {
        err=pthread_create(&(tid[i]),NULL,&mengekstrak,NULL);//membuat thread
        if(err!=0)//cek error
        {
            printf("\n can't create thread : [%s]",strerror(err));
        }
        else
        {
            // printf("\n create thread success");
        }
        i++;
    }
    printf("\n DONE!");
    pthread_join(tid[0],NULL);
    pthread_join(tid[1],NULL);
    pthread_join(tid[2],NULL);
    pthread_join(tid[3],NULL);
    pthread_join(tid[4],NULL);
    pthread_join(tid[5],NULL);
    return 0;
}
```
Pada fungsi utama, program membuat thread yang dapat berjalan bersamaan. 2 proses akan berjalan bersama pada fungsi "membuat". Saat kedua proses tersebut selesai menjalankan tugasnya maka 2 proses berikutnya akan menjalankan fungsi "mengkompres". Untuk menjalankan fungsi "mengekstrak", harus menunggu sampai fungsi "mengkompres" selesai dijalankan dan menunggu 15 detik. 


##  No. 5
5.	Angga, adik Jiwang akan berulang tahun yang ke sembilan pada tanggal 6 April besok. Karena lupa menabung, Jiwang tidak mempunyai uang sepeserpun untuk membelikan Angga kado. Kamu sebagai sahabat Jiwang ingin membantu Jiwang membahagiakan adiknya sehingga kamu menawarkan bantuan membuatkan permainan komputer sederhana menggunakan program C. Jiwang sangat menyukai idemu tersebut. Berikut permainan yang Jiwang minta. 
	- Pemain memelihara seekor monster lucu dalam permainan. Pemain dapat  memberi nama pada monsternya.
	- Monster pemain memiliki hunger status yang berawal dengan nilai 200 (maksimalnya) dan nanti akan berkurang 5 tiap 10 detik.Ketika hunger status mencapai angka nol, pemain akan kalah. Hunger status dapat bertambah 15 apabila pemain memberi makan kepada monster, tetapi banyak makanan terbatas dan harus beli di Market.
	- Monster pemain memiliki hygiene status yang berawal dari 100 dan nanti berkurang 10 tiap 30 detik. Ketika hygiene status mencapai angka nol, pemain akan kalah. Hygiene status' dapat bertambah 30 hanya dengan memandikan monster. Pemain dapat memandikannya setiap 20 detik(cooldownnya 20 detik).
	- Monster pemain memiliki health status yang berawal dengan nilai 300. Variabel ini bertambah (regenerasi)daa 5 setiap 10 detik ketika monster dalam keadaan standby.
	- Monster pemain dapat memasuki keadaan battle. Dalam keadaan ini, food status(fitur b), hygiene status'(fitur c), dan ‘regenerasi’(fitur d) tidak akan berjalan. Health status dari monster dimulai dari darah saat monster pemain memasuki battle. Monster pemain akan bertarung dengan monster NPC yang memiliki darah 100. Baik monster pemain maupun NPC memiliki serangan sebesar 20. Monster pemain dengan monster musuh akan menyerang secara bergantian. 
	- Fitur shop, pemain dapat membeli makanan sepuas-puasnya selama stok di toko masih tersedia.
	
##  Penjelasan
```
char getch() 
{
    char ch;
    tcgetattr(0, &old); /* grab old terminal i/o settings */
    new = old; /* make new settings same as old settings */
    new.c_lflag &= ~ICANON; /* disable buffered i/o */
    // new.c_lflag |= ECHO; /* set echo mode */
    new.c_lflag &= ~ECHO; /* set no echo mode */
    tcsetattr(0, TCSANOW, &new); /* use these new terminal i/o settings now */
    ch = getchar();
    tcsetattr(0, TCSANOW, &old);
    return ch;
}
```
fungsi "getch" digunakan untuk mendeteksi input berupa key press. (Program bisa berjalan tanpa perlu menekan tombol enter)
```
void* input()
{
    while ((a = getch())) 
    {
        if(aksi == standby)
        {
            if(a=='1')
            {
                if(food_stock>0)
                {
                    eat = 1;
                    food_stock--;
                }
                else
                    printf("Tidak punya makanan");
            }
            else if(a=='2' && !cd)
                bath=1;
            else if(a=='3')
            {
                system("clear");
                turn=0;
                aksi=battle;
            }
            else if(a=='4')
            {
                system("clear");
                aksi=shop;
            }
            else if(a=='5')
                stop=1;
        }
        else if(aksi==battle)
        {
            if(a=='1')
            {
                if(turn==0&&opponent>=20)
                {
                	opponent=opponent-ATT;
                	sleep(1);
                	turn=1;
                }
                else 
                	printf("Your monster has already won\n");
            }
            else if(a=='2')
            {
                system("clear");
                turn=0;
                opponent=100;
                aksi=standby;
            }
        }
        else if(aksi==shop)
        {
            if(a=='1')
            {
                if(*value>0)
                {
                    food_stock++;
                    *value=*value-1;
                }
                else
                    printf("Stock is empty");
            }
            else if(a=='2')
            {
                system("clear");
                aksi=standby;
            }
        }
    }
}
```
fungsi "input" digunakan untuk mendeteksi input lalu diarahkan ke menu yang sesuai dengan angka yang diinputkan
```
void* interface()
{
    while(1)
    {
        if(aksi==standby)
        {
            printf("Standby Mode\nHealth : %d\nHunger : %d\nHygiene : %d\n", health, hunger, hyg);
            printf("Food left : %d\n",food_stock);
            if(cd>0)
            	printf("Bath will be ready in %ds\n",cd);
            else 
            	printf("Bath is ready\n");
            printf("Choices\n1. Eat\n2. Bath\n3. Battle\n4. Shop\n5. Exit\n");
            sleep(1);
            system("clear");
        }
        else if(aksi==battle)
        {
            printf("Battle Mode\nMonster's Health : %d\nOpponent's Health : %d\n", health, opponent);
            printf("Choices\n1. Attack\n2. Run\n");
            sleep(1);
            system("clear");
            if(turn==1 && opponent>0)
            {
                turn=0;
            	health=health-ATT;
            }
        }
        else if(aksi == shop)
        {
            printf("Shop Mode\nShop Food Stock : %d\nYour Food Stock : %d\n", *value, food_stock);
            printf("Choices\n1. Buy\n2. Back\n");
            sleep(1);
            system("clear");
        }
    }
}
```
fungsi "interface" digunakan untuk menampilkan tampilan saat kita menginputkan angka
```
void* hunger_status()
{
    int i=0;
    while(i<10)
    {
        if(aksi!=battle)
        {
            sleep(1);
            if(eat)
            {
                i=0;
                eat=0;
                hunger=hunger+15;
                continue;
            }
            i++;
            if(i==10)
            	hunger=hunger-5;
            i=i%10;
        }
    }
}
```
fungsi "hunger_status" digunakan untuk menghitung hunger status setiap detiknya. Hunger status tidak akan berjalan saat kondisi battle. Hunger status akan bertambah 15 setiap makan dan akan berkurang 5 setiap 10s
```
void* hygiene_status()
{
    int i=0;
    while(i<30)
    {
        if(aksi!=battle)
        {
            sleep(1);
            if(bath)
            {
                i=0;
                cd=20;
                bath=0;
                hyg=hyg+30;
                continue;
            }
            i++;
            if(cd>0)
            	cd--;
            if(i==30) 
            	hyg=hyg-10;
            i%=30;
        }
    }
}
```
fungsi "hygiene_status" digunakan untuk menghitung hygiene status setiap detiknya. Hygiene status tidak akan berjalan saat kondisi battle. Hygiene status akan bertambah 30 setiap mandi (mandi harus menunggu 20s) dan akan berkurang 10 setiap 30s
```
int main(int argc, char const *argv[]) 
{
    mem = shmget(key,sizeof(int),IPC_CREAT|0666);
    value = shmat(mem,NULL,0);

    pthread_create(&t[0],NULL,input,NULL);
    pthread_create(&t[1],NULL,hunger_status,NULL);
    pthread_create(&t[2],NULL,health_status,NULL);
    pthread_create(&t[3],NULL,hygiene_status,NULL);
    pthread_create(&t[4],NULL,interface,NULL);

    while(hunger>0&&hyg>0&&health>0&&!stop);
    pthread_cancel(t[0]);
    tcsetattr(0, TCSANOW, &old);
    printf("game has ended\n");
    return 0;
}
```
Pada fungsi utama semua thread akan berjalan bersamaan dan permainan akan berhenti ketika hunger, hygiene, health mencapai angka 0
variable value digunakan sebagai shared memory yang nilainya dapat berubah-ubah saat digunakan bersama-sama oleh 2 program.
```
void* food_stock()
{
    while(1)
    {
        printf("Shop\nFood Stock : %d\nChoices\n1.Restock\n2.Exit\n",*value);
        sleep(1);
        system("clear");
    }
}
```
fungsi food_stock digunakan untuk menampilkan keadaan shop
```
int main(int argc, char const *argv[]) 
{
    mem = shmget(key,sizeof(int),IPC_CREAT|0666);
    value = shmat(mem,NULL,0);
    
    pthread_create(&t,NULL,food_stock,NULL);
    while ((a = getch())) 
    {
        if(a=='1') 
            *value=*value+1;
        else if(a=='2') 
            break;
    }
    if(a=='2') 
        exit;
    return 0;
}
```
pada fungsi utama program makanan, nilai value digunakan bersama-sama dengan program utama. apabila angka 1 ditekan maka nilai value akan bertambah di program makanan dan di program utama. apabila angka 2 ditekan maka program akan berhenti


