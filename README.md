# Mini Web Server

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/bdd338d821af48f28649dff690b8ac9d)](https://app.codacy.com/app/berviantoleo/mini-web-server?utm_source=github.com&utm_medium=referral&utm_content=berv-uni-project/mini-web-server&utm_campaign=Badge_Grade_Settings)
[![Build Status](https://travis-ci.org/berv-uni-project/mini-web-server.svg?branch=master)](https://travis-ci.org/berv-uni-project/mini-web-server)

Mini Web Server build with C using libevent.

# Daftar Isi

* [Percobaan dengan Web Server Apache dan NGINX](#percobaan-dengan-web-server-apache-dan-nginx)
	* [Server yang Digunakan](#server-yang-digunakan)
		* [Tools Benchmark](#tools-benchmark)
			* [Gobench](#gobench)
			* [Monit](#monit)
			* [htop](#htop)
		* [Hasil Percobaan](#hasil-percobaan)
			* [Apache](#apache)
			* [Nginx](#nginx)
		* [Simpulan Hasil Benchmark](#simpulan-hasil-benchmark)
* [Petunjuk Penggunaan Program](#petunjuk-penggunaan-program)
	* [Dependency Program](#dependency-program)
	* [Compile Program](#compile-program)
	* [Instalasi](#instalasi-untuk-dapat-digunakan-pada-terminal)
	* [Menghapus instalasi](#menghapus-instalasi)		
	* [Konfigurasi](#konfigurasi)
	* [Hasil Test](#hasil-tes-menggunakan-tools-benchmark)
* [Program Kecil Lain](#tanpa-menggunakan-build-in-evhttp)
* [Tentang Pembuat](#about)
* [Lisensi](#license)

## Percobaan dengan Web Server Apache dan NGINX

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

	```bash
		GOPATH=/tmp/ go get github.com/valyala/fasthttp
		GOPATH=/tmp/ go get github.com/cmpxchg16/gobench
	```

	* Jalankan web server
	* Jalankan gobench (dapat dalam GET atau POST), dalam percobaan ini cukup dalam GET. (Jika diinstall ke dalam /tmp/)
	```bash
		$>/tmp/bin/gobench -u http://localhost:80 -k=true -c 500 -t 10
	```

* Benchmark akan dilakukan dengan 10000 koneksi konkuren dan 150 kali request.

#### Monit

* Dipasang pada perangkat server. Untuk memonitor penggunaan memori, cpu, dan uptime.
* Ada pada perangkat linux biasanya.

#### htop

* Melihat process yang berlangsung dan penggunaan memori.
* Diinstall pada server.

### Hasil Percobaan

#### Apache

Percobaan apache dilakukan dengan 500 bytes dan 20 KB.

Adapun perintah yang digunakan yaitu :

```bash
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

```bash
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

Pada dasarnya penggunaan memori pada Apache dan Nginx sama tidak jauh berbeda. Berbeda sekitar 0,1 GB saja.

Sedangkan respons time untuk nginx lebih baik dibandingkan apache. Pada nginx dapat lebih baik walaupun perbedaan file yang dikembalikan cukup besar.

## Petunjuk Penggunaan Program

### Dependency Program

* libevent

Instalasi lihat di : http://libevent.org/

### Compile program

Untuk mengcompile program cukup lakukan :

```bash
	make all
```

### Instalasi untuk dapat digunakan pada terminal

```bash
	sudo make install
```

Program akan ditaruh pada `/opt/mws`.

### Menghapus instalasi

Secara default di install pada direktori `opt/mws`, jika demikian lakukan perintah berikut.
```bash
	sudo make remove
```

### Konfigurasi

Pastikan terdapat direktori config di lokasi program dan terdapat `config.json`.

Format yang digunakan yaitu:
```json
{
	"directory": "lokasi root directory server",
	"name_server": "ip atau name server yang akan di bind",
	"port": "nomor_port"
}
```

Contoh:
```json
{
	"directory": "htdocs",
	"name_server": "localhost",
	"port": 8080
}
```

### Jalankan program

Sangat simpel. Menjalankannya cukup dengan `./mini-web-server`. Pastikan konfigurasi sudah benar!

### Hasil Tes Menggunakan Tools Benchmark

Perintah yang digunakan :

```bash
	$>/tmp/bin/gobench -u http://localhost:8080/500bytes.html -k=true -c 10000 -r 150
	$>/tmp/bin/gobench -u http://localhost:8080/20kb.html -k=true -c 10000 -r 150

```

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

## Tanpa Menggunakan Build in Evhttp

### Compile

Jika hanya ingin meng-compile dapat menggunakan perintah berikut.

```bash
	make small
```

### Installasi

Dapat menggunakan:

```bash
	make install
```

Hal ini akan memberikan instalasi mini-web-server dan mws.

### Jalankan

```bash
 ./mws
```

### Konfigurasi

File konfigurasi yang digunakan sama dengan mini-web-server.

# About

Bervianto Leo P - 13514047

# Reference

[1] https://github.com/libevent/libevent

[2] http://www.wangafu.net/~nickm/libevent-book/

[3] https://github.com/libevent/libevent/blob/master/sample/http-server.c

[4] https://www.ibm.com/developerworks/aix/library/au-libev/

# LICENSE

```
Copyright (C) 2017  berviantoleo

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
```
