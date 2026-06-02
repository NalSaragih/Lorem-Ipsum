# README : Social Media System

Proyek ini adalah sebuah simulasi sistem Backend Sosial Media yang mengintegrasikan fitur-fitur populer dari platform seperti **WhatsApp** dan **Instagram**. Fokus utama proyek ini adalah implementasi struktur data yang efisien untuk menangani interaksi pengguna.

---

**Nama Kelompok:** Lorem Ipsum Dolor Sit Amet

| Nama Anggota | NIM |
| :--- | :--- |
| Muhammad Haikal Muzhoffar | 140810250082 |
| Nala Ahmad Rayyan Saragih | 140810250061 |
| Dzaky Mahfuzh Fahmi | 140810250037 |

---

Kami memilih struktur data tertentu berdasarkan efisiensi algoritma dan fungsionalitas di dunia nyata:

| Fitur | Struktur Data | Alasan Logis |
| :--- | :--- | :--- |
| **Notifikasi** | `Queue` (Antrean) | Menggunakan prinsip **FIFO** (First-In, First-Out). Notifikasi diproses sesuai urutan waktu masuk. |
| **Pesan (Chat)** | `Linked List` | Memungkinkan penambahan pesan secara dinamis tanpa batas ukuran (seperti *array*) dan memudahkan navigasi riwayat. |
| **Following** | `Graph` | Representasi hubungan antar pengguna (Node & Edge). Sangat efisien untuk fitur *friend suggestion*. |
| **Feed Postingan** | `Doubly Linked List` | Memudahkan pengguna untuk *scrolling* maju ke postingan lama atau kembali ke postingan terbaru ($O(1)$). |
| **Like & Aktivitas** | `Stack` (Tumpukan) | Menggunakan prinsip **LIFO** (Last-In, First-Out). Aksi terbaru berada di puncak tumpukan sebagai riwayat aktivitas pengguna. |
| **Trending Topic** | `Priority Tree` | Menempatkan topik dengan interaksi tertinggi pada urutan prioritas teratas berdasarkan skor. |

---

## 📖 Penjelasan Teknis Sistem

### 1. Sistem Notifikasi (Queue)
Sistem ini memastikan tidak ada notifikasi yang terlewat. Setiap interaksi baru akan masuk ke belakang antrean dan ditampilkan kepada pengguna satu per satu sesuai urutan kronologis.

### 2. Antrean Pesan & Riwayat
Berbeda dengan Stack yang bersifat kaku, kami menggunakan **Linked List** agar sistem bisa menyisipkan pesan di tengah (seperti fitur *reply* atau *edit*) dan menghapus pesan dengan alokasi memori yang dinamis.

### 3. Jaringan Pertemanan (Graph)
Kami menggunakan **Directed Graph** menggunakan *Adjacency List*. Jika User A mengikuti User B, maka ada garis (edge) dari A ke B, namun tidak otomatis sebaliknya (seperti sistem *Follow* di Instagram).

### 4. Feed & Navigasi Postingan
Dengan **Doubly Linked List**, setiap objek postingan memiliki pointer `next` dan `prev`. Ini mensimulasikan pengalaman pengguna saat menggeser layar ke atas dan ke bawah dengan akses memori yang instan.

### 5. Fitur Like & Aktivitas (Stack)
Setiap aksi seperti Like, Unlike, Follow, Unfollow, Post, Delete Post, Comment, dan Delete Comment dimasukkan ke dalam Stack sebagai riwayat aktivitas. Aksi terbaru selalu berada di `top`, sehingga prinsip **LIFO** tetap terlihat tanpa perlu menghapus elemen di tengah.

### 6. Algoritma Trending (Priority Tree)
Topik yang sedang tren dihitung berdasarkan bobot (Post + Like). Pada integrasi Qt, prioritas trending direpresentasikan dengan tree berbasis skor sehingga topik dengan skor tertinggi dapat ditampilkan lebih dulu.

---

## Integrasi ke SocialMediaQt
File `action.hpp/.cpp`, `feed.hpp/.cpp`, `Notifikasi.hpp/.cpp`, dan `chat.hpp/.cpp` dipakai oleh aplikasi Qt melalui `LoremIpsumBridge`.

Bridge ini mengadaptasi struktur data C++ asli ke tipe Qt (`QString`, `QVector`) agar bisa dipakai oleh UI tanpa mengubah prinsip struktur data:

* `feed.cpp` → mirror feed postingan berbasis Doubly Linked List.
* `action.cpp` → mirror Stack aktivitas terbaru.
* `Notifikasi.cpp` → mirror Queue notifikasi FIFO.
* `chat.cpp` → mirror Linked List riwayat pesan.

---

## 💻 Teknologi yang Digunakan
* **Bahasa Pemrograman:** C++
* **Editor:** Visual Studio Code