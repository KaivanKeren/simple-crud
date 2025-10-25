# 🎓 Student CRUD App (C++ + PostgreSQL + Docker)

A simple console-based CRUD application written in **C++** using **libpq** (the PostgreSQL C API).  
The database runs via **Docker Compose**, and the project builds using **CMake**.

---

## 🚀 Features

- Create, Read, Update, and Delete student data  
- Uses PostgreSQL as the database  
- Environment configuration via `.env` file  
- Quick setup with Docker Compose  
- Input validation and simple error handling  

---

## 📁 Project Structure

```

.
├── .env.example            # Example environment variables
├── .gitignore              # Git ignore file
├── CMakeLists.txt          # CMake build configuration
├── crud_data_mahasiswa.cpp # Main C++ source code
└── docker-compose.yml      # PostgreSQL Docker setup

````

---

## ⚙️ Setup Instructions

### 1️⃣ Clone the Repository
```bash
git clone https://github.com/KaivanKeren/simple-crud.git
cd simple-crud
````

---

### 2️⃣ Create a `.env` File

Copy the example file and update it:

```bash
cp .env.example .env
```

Example `.env`:

```env
DB_HOST=localhost
DB_PORT=5439
DB_NAME=cpp_db
DB_USER=user_cpp
DB_PASS=password_cpp
```

---

### 3️⃣ Start PostgreSQL with Docker Compose

```bash
docker-compose up -d
```

Check containers:

```bash
docker ps
```

Connect to PostgreSQL:

```bash
docker exec -it cpp_db psql -U user_cpp -d cpp_db
```

Create the `mahasiswa` table:

```sql
CREATE TABLE mahasiswa (
    id SERIAL PRIMARY KEY,
    nim VARCHAR(20) NOT NULL,
    nama VARCHAR(100) NOT NULL,
    jurusan VARCHAR(100),
    umur INT
);
```

---

### 4️⃣ Build & Run the Program (with CMake)

**Linux/macOS:**

```bash
cmake -S . -B build && cmake --build build && ./build/crud_data_mahasiswa
```

---

## 🧠 Program Menu

```
=== MENU CRUD Data Mahasiswa ===
1. Tambah Data
2. Tampilkan Data
3. Ubah Data
4. Hapus Data
5. Keluar
```

Example:

```
Masukkan Nama: Ismail
Masukkan NIM: 230102011
Masukkan Jurusan: Information System
Masukkan Umur: 18
✅ Data berhasil ditambahkan!
```

---

## 🧰 Dependencies

Make sure PostgreSQL client library is installed.

**Ubuntu/Debian:**

```bash
sudo apt install libpq-dev
```

**Fedora:**

```bash
sudo dnf install postgresql-devel
```

---

## 🧾 Example `.gitignore`

Here’s a recommended `.gitignore` for this setup:

```
# CMake build files
/build/
CMakeCache.txt
CMakeFiles/
Makefile

# Environment file
.env

# Logs and temp files
*.log
*.out
```

---

## 🧩 Tips

* Commit `.env.example`, but **don’t** commit the actual `.env` file.
* You can create a `docker-compose.override.yml` for local testing (add it to `.gitignore`).
* If you can’t connect to PostgreSQL, check the `.env` values and port mapping.

---

## 🪪 License

MIT License — free to use, modify, and learn from.

---