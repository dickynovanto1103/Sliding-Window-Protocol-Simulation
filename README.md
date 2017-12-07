# Sliding-Window-Protocol-Simulation
Implementing Sliding Window Protocol. Simulating sending packets to another port

## Done by:
* [Dicky Novanto](https://github.com/dickynovanto1103)
* [Devin Alvaro](https://github.com/devinalvaro)
* [Francisco Kenandi](https://github.com/FranciscoKen)

## Petunjuk Penggunaan Program

- Jalankan `make`
- Run program dengan format :
	- untuk program sendfile:
		`./sendfile <filename> <windowsize> <buffersize> <destination_ip> <destination_port>`
	- untuk program recvfile:
		`./recvfile <filename> <windowsize> <buffersize> <port>`

## Cara Kerja Sliding Window pada program

 Program ini menggunakan 3 jenis struktur data: SenderWindow, ReceiverWindow, dan ReceiverBuffer. Pertama-tama, program akan membuat koneksi socket dengan port tertentu, kemudian program sendfile akan mengakses file yang akan dikirim. 

 Program sendfile akan memasukkan secara terurut informasi pada file ke SenderWindow sampai SenderWindow penuh. Sender akan terus mengirimkan paket sampai mendapat ACK untuk paket bersangkutan. Bila sebuah paket pada SenderWindow sudah mendapat ACK, maka sender akan menghapus paket tersebut dari SenderWindow agar dapat diisi dengan lanjutan informasi dari file eksternal. Proses ini terus berlanjut sampai end of file dan SenderWindow kosong.

 Di sisi lain program receiver setelah membuat koneksi socket dengan sender akan terus menerima paket dari sender dan memasukannya ke dalam ReceiverWindow. Bila ada kerusakan pada paket (nilai checksum rusak), maka receiver akan mengirimkan NAK kepada sender dan paket akan dikirim ulang. Bila nilai paket yang sudah diterima merupakan nilai valid, maka receiver akan mengirim ACK dan memasukkan paket ke dalam ReceiverWindow. Receiver akan memindahkan packet dari ReceiverWindow ke receiver buffer ketika ReceiverWindow sudah penuh. Receiver buffer bertindak sebagai perantara antara ReceiverWindow dengan file eksternal, yang mana isi dari receiver buffer akan dituliskan ke dalam file eksternal output.

 Program sender dan receiver sama-sama menggunakan konsep multithreading, yakni child thread untuk menerima repond dari receiver pada program sender dan child thread untuk memindahkan paket dari receive buffer ke file eksternal pada program receiver.

## Pembagian Tugas

- Devin : membuat program receiver dan sender, membuat tipe data response dan Receiver buffer, debugging
- Dicky : membuat program receiver, membuat fungsi checksum, membuat tipe data message dan window, debugging 
- Francisco : membuat program sender, membuat connection socket, debugging

## Jawaban Pertanyaan

1. Bila advertised window size mencapai nilai 0, maka sender tidak dapat lagi mengirimkan paket karena receiver sudah tidak mungkin menerima paket lagi. client atau server harus saling menunggu pengosongan buffer dari lawannya.

2. TCP Header:

---------------------------------------------------------------------------------------------
|			source port number (2 byte)			 	    ||destination port number (2 byte) |
---------------------------------------------------------------------------------------------
|								sequence number (4 bytes)									|
---------------------------------------------------------------------------------------------
|								acknowledge number (4 byte)									|
---------------------------------------------------------------------------------------------
|dataoffset(4bits) | reserved 3bits | ctrlflags(9bits) || windowsize(2bytes) 				|
---------------------------------------------------------------------------------------------
|					checksum (2 bytes)				   || urgent pointer (2 bytes)			|
---------------------------------------------------------------------------------------------
|								optional data (0-40 bytes)									|
---------------------------------------------------------------------------------------------

- source Port Number : untuk tahu sumber komunikasi
- destination port number : untuk tahi tujuan komunikasi
- sequence number : bila data yang dipertukarkan harus disusun.
- acknowledge number : menandakan kode ack.
- data offset : menyimpan ukuran total tcp header dalam kelipatan 4 byte
- reserved bits : umunya diisi dengan value 0
- control flags : untuk mekanisme flag (ada 6 jenis)
- window size : angka agar sender dapat tahu berapa besar data yang harus dikirim ke receiver
- checksum : hasil checksum, mekanisme error detection
- urgent pointer : kebanyakan diisi dengan nilai 0
- optional data : dapat digunakan untuk special ack dan algoritma window scaling
