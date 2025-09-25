#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <libpq-fe.h>

using namespace std;

map<string, string> loadEnv(const string &filename)
{
    map<string, string> env;
    ifstream file(filename);
    string line;

    while (getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue; // skip kosong / comment
        string key, value;
        stringstream ss(line);
        if (getline(ss, key, '=') && getline(ss, value))
        {
            env[key] = value;
        }
    }
    return env;
}

// connect DB
PGconn *connectDB(map<string, string> &env)
{
    string conninfo = "host=" + env["DB_HOST"] +
                      " port=" + env["DB_PORT"] +
                      " dbname=" + env["DB_NAME"] +
                      " user=" + env["DB_USER"] +
                      " password=" + env["DB_PASS"];

    PGconn *conn = PQconnectdb(conninfo.c_str());

    if (PQstatus(conn) != CONNECTION_OK)
    {
        cerr << "Koneksi gagal: " << PQerrorMessage(conn);
        PQfinish(conn);
        exit(1);
    }
    return conn;
}

// tambah fungsi ini di atas (mis. setelah connectDB)
bool recordExists(PGconn *conn, int id)
{
    string query = "SELECT 1 FROM mahasiswa WHERE id = " + to_string(id) + " LIMIT 1;";
    PGresult *res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        cerr << "Gagal cek ID: " << PQerrorMessage(conn);
        PQclear(res);
        return false; // anggap nggak ada bila error, atau sesuaikan sesuai kebutuhan
    }

    bool exists = (PQntuples(res) > 0);
    PQclear(res);
    return exists;
}

// CREATE
void tambahData(PGconn *conn, string nama, int umur)
{
    string query = "INSERT INTO mahasiswa (nama, umur) VALUES ('" + nama + "', " + to_string(umur) + ");";
    PGresult *res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        cerr << "Gagal insert: " << PQerrorMessage(conn);
    }
    else
    {
        cout << "Data berhasil ditambahkan!\n";
    }
    PQclear(res);
}

// READ
void tampilData(PGconn *conn)
{
    PGresult *res = PQexec(conn, "SELECT id, nama, umur FROM mahasiswa;");

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        cerr << "Gagal select: " << PQerrorMessage(conn);
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    cout << "\n=== Data Mahasiswa ===\n";
    for (int i = 0; i < rows; i++)
    {
        cout << "ID: " << PQgetvalue(res, i, 0)
             << ", Nama: " << PQgetvalue(res, i, 1)
             << ", Umur: " << PQgetvalue(res, i, 2) << endl;
    }
    PQclear(res);
}

// UPDATE
void ubahData(PGconn *conn, int id, string nama, int umur)
{
    string query = "UPDATE mahasiswa SET nama='" + nama + "', umur=" + to_string(umur) +
                   " WHERE id=" + to_string(id) + ";";
    PGresult *res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        cerr << "Gagal update: " << PQerrorMessage(conn);
    }
    else
    {
        if (atoi(PQcmdTuples(res)) == 0)
        {
            cout << "ID " << id << " tidak ditemukan!\n";
        }
        else
        {
            cout << "Data berhasil diubah!\n";
        }
    }
    PQclear(res);
}

// DELETE
void hapusData(PGconn *conn, int id)
{
    string query = "DELETE FROM mahasiswa WHERE id=" + to_string(id) + ";";
    PGresult *res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        cerr << "Gagal hapus: " << PQerrorMessage(conn);
    }
    else
    {
        if (atoi(PQcmdTuples(res)) == 0)
        {
            cout << "ID " << id << " tidak ditemukan!\n";
        }
        else
        {
            cout << "Data berhasil dihapus!\n";
        }
    }
    PQclear(res);
}

int main()
{
    auto env = loadEnv(".env"); // baca file .env
    PGconn *conn = connectDB(env);

    int pilihan, id, umur;
    string nama;

    do
    {
        cout << "\n=== MENU CRUD ===\n";
        cout << "1. Tambah Data\n";
        cout << "2. Tampilkan Data\n";
        cout << "3. Ubah Data\n";
        cout << "4. Hapus Data\n";
        cout << "5. Keluar\n";
        cout << "Pilih: ";
        cin >> pilihan;

        switch (pilihan)
        {
        case 1:
            cout << "Masukkan Nama: ";
            cin.ignore();
            getline(cin, nama);
            cout << "Masukkan Umur: ";
            cin >> umur;
            tambahData(conn, nama, umur);
            break;
        case 2:
            tampilData(conn);
            break;
        case 3:
            cout << "Masukkan ID yang mau diubah: ";
            cin >> id;

            // cek dulu sebelum minta input lain
            if (!recordExists(conn, id))
            {
                cout << "ID " << id << " tidak ditemukan!\n";
                break;
            }

            cout << "Masukkan Nama baru: ";
            cin.ignore();
            getline(cin, nama);
            cout << "Masukkan Umur baru: ";
            cin >> umur;
            ubahData(conn, id, nama, umur);
            break;

        case 4:
            cout << "Masukkan ID yang mau dihapus: ";
            cin >> id;
            hapusData(conn, id);
            break;
        case 5:
            cout << "Keluar...\n";
            break;
        default:
            cout << "Pilihan tidak valid!\n";
        }
    } while (pilihan != 5);

    PQfinish(conn);
    return 0;
}
