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
| **Like & Undo** | `Stack` (Tumpukan) | Menggunakan prinsip **LIFO** (Last-In, First-Out). Aksi terakhir (Like) berada di puncak tumpukan sehingga mudah untuk di-`pop` (Undo). |
| **Trending Topic** | `Max-Heap Tree` | Menempatkan konten dengan interaksi tertinggi di posisi *root*. Operasi pengambilan data terpopuler sangat cepat ($O(1)$). |

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

### 5. Fitur Like & Batal (Stack)
Logika "Batal Like" bekerja seperti fitur *Undo*. Aksi Like terakhir disimpan di paling atas. Jika pengguna menekan tombol batal, sistem hanya perlu melakukan operasi `pop()` pada tumpukan tersebut.

### 6. Algoritma Trending (Priority Queue/Heap)
Topik yang sedang tren dihitung berdasarkan bobot (Like + Komen). Kami menggunakan **Max-Heap** agar sistem selalu bisa menampilkan konten paling viral di halaman utama tanpa harus mengurutkan ulang seluruh data secara manual ($O(\log n)$ untuk setiap perubahan data).

---

## 💻 Teknologi yang Digunakan
* **Bahasa Pemrograman:** C++
* **Editor:** Visual Studio Code