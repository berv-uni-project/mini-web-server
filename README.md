# Mini Web Server

Mini Web Server build with C using libevent.

## Percobaan dengan Web Server Apache & NGINX

### Server yang digunakan

Server yang digunakan dengan spesifikasi berikut: 
* Intel Core i3-5010U 2.1 GHz (dual core)
* Memori 6 GB
* Apache2 port 10000
* NGINX port 80
* Web Server buatan pada port 8080
* Dijalankan satu per satu saat percobaan


### Tools Benchmark

#### Gobench

Gobench diinstall pada perangkat yang akan melakukan request.

Buka : [cmpxchg16/gobench](https://github.com/cmpxchg16/gobench)

* Lakukan sesuai petunjuk penggunaan pada repositori tersebut.
* Secara singkat tahapannya sebagai berikut :

	* Install Go
	* Lakukan perintah
	
	```
		GOPATH=/tmp/ go get github.com/valyala/fasthttp
		GOPATH=/tmp/ go get github.com/cmpxchg16/gobench
	```
	
	* Jalankan web server
	* Jalankan gobench (dapat dalam GET atau POST), dalam percobaan ini cukup dalam GET. (Jika diinstall ke dalam /tmp/)
	```
		$>/tmp/bin/gobench -u http://localhost:80 -k=true -c 500 -t 10
	```
	
* Benchmark akan dilakukan dengan 10000 koneksi konkuren dan 150 kali request.

#### Monit

* Dipasang pada perangkat server. Untuk memonitor penggunaan memori, cpu, dan uptime.
* Ada pada perangkat linux biasanya.

#### htop

* Melihat process yang berlangsung dan penggunaan memori

### Hasil Percobaan

#### Apache

Percobaan apache dilakukan dengan 500 bytes dan 20 KB.

Adapun perintah yang digunakan yaitu :

```
	$>/tmp/bin/gobench -u http://localhost:10000/500bytes.html -k=true -c 10000 -r 10
	$>/tmp/bin/gobench -u http://localhost:10000/20kb.html -k=true -c 10000 -r 10
```

Hasil yang didapatkan untuk 500 bytes :

```
Dispatching 10000 clients
Waiting for results...

Requests:                          1500000 hits
Successful requests:                101364 hits
Network failed:                    1398636 hits
Bad requests failed (!2xx):              0 hits
Successful requests rate:             7797 hits/sec
Read throughput:                   6315740 bytes/sec
Write throughput:                   853684 bytes/sec
Test time:                              13 sec
```

Hasil yang didapatkan untuk 20 KB :

```
Dispatching 10000 clients
Waiting for results...

Requests:                          1500000 hits
Successful requests:                118616 hits
Network failed:                    1381384 hits
Bad requests failed (!2xx):              0 hits
Successful requests rate:             7413 hits/sec
Read throughput:                 150837375 bytes/sec
Write throughput:                   762912 bytes/sec
Test time:                              16 sec
```

Memori yang digunakan sekitar `195 MB`.

#### NGINX

Percobaan nginx dilakukan dengan 500 bytes dan 20 KB.

Adapun perintah yang digunakan yaitu :

```
	$>/tmp/bin/gobench -u http://localhost:80/500bytes.html -k=true -c 10000 -r 150
	$>/tmp/bin/gobench -u http://localhost:80/20kb.html -k=true -c 10000 -r 150
```

Hasil yang didapatkan untuk 500 bytes :

```
Dispatching 10000 clients
Waiting for results...

Requests:                          1500000 hits
Successful requests:                109968 hits
Network failed:                    1390032 hits
Bad requests failed (!2xx):              0 hits
Successful requests rate:            10996 hits/sec
Read throughput:                   8247072 bytes/sec
Write throughput:                  1070414 bytes/sec
Test time:                              10 sec
```

Hasil yang didapatkan untuk 20 KB :

```
Dispatching 10000 clients
Waiting for results...

Requests:                          1500000 hits
Successful requests:                129447 hits
Network failed:                    1370553 hits
Bad requests failed (!2xx):              0 hits
Successful requests rate:            11767 hits/sec
Read throughput:                 238723222 bytes/sec
Write throughput:                  1098744 bytes/sec
Test time:                              11 sec
```

Memori yand digunakan sekitar `184 MB`.

### Simpulan hasil benchmark

Pada dasarnya penggunaan memori pada Apache dan Nginx sama tidak jauh berbeda. Berbeda sekitar 0,1 GB saja. Sedangkan respons time untuk nginx lebih baik dibandingkan apache.

## Petunjuk Penggunaan Program

Untuk mengcompile program cukup lakukan :

```
	make all
```	

Sangat simpel. Sedangkan menjalankannya cukup dengan `./mini-web-server`.

### Hasil Tes Menggunakan Tools Benchmark

#### 500 bytes file

```
Dispatching 10000 clients
Waiting for results...

Requests:                          1500000 hits
Successful requests:                 87131 hits
Network failed:                    1401557 hits
Bad requests failed (!2xx):          11312 hits
Successful requests rate:             6702 hits/sec
Read throughput:                   4278572 bytes/sec
Write throughput:                   749681 bytes/sec
Test time:                              13 sec
```

#### 20 KB File

```
Dispatching 10000 clients
Waiting for results...

Requests:                          1500000 hits
Successful requests:                 93669 hits
Network failed:                    1391112 hits
Bad requests failed (!2xx):          15219 hits
Successful requests rate:             4929 hits/sec
Read throughput:                  99494997 bytes/sec
Write throughput:                   544440 bytes/sec
Test time:                              19 sec
```

Memori yang digunakan sekitar `215 MB`.

Respons time yang lebih kecil dibandingkan dengan apache dan nginx, penggunaan memori pun masih lebih besar.
