#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <ncurses.h>
#include <libpq-fe.h>

using namespace std;

map<string, string> loadEnv(const string &filename)
{
    map<string, string> env;
    ifstream file(filename);
    string line;
    while (getline(file, line))
    {
        if (line.empty() || line[0] == '#') continue;
        size_t pos = line.find('=');
        if (pos != string::npos)
            env[line.substr(0, pos)] = line.substr(pos + 1);
    }
    return env;
}

PGconn *connectDB(map<string, string> &env)
{
    string conninfo = "host=" + env["DB_HOST"] + " port=" + env["DB_PORT"] +
                      " dbname=" + env["DB_NAME"] + " user=" + env["DB_USER"] +
                      " password=" + env["DB_PASS"];
    PGconn *conn = PQconnectdb(conninfo.c_str());
    if (PQstatus(conn) != CONNECTION_OK)
    {
        endwin();
        cerr << "Koneksi gagal: " << PQerrorMessage(conn);
        exit(1);
    }
    return conn;
}

void execQuery(PGconn *conn, const string &query, const string &success = "")
{
    PGresult *res = PQexec(conn, query.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK)
        mvprintw(LINES - 1, 2, "Error: %s", PQerrorMessage(conn));
    else if (!success.empty())
        mvprintw(LINES - 1, 2, "%s", success.c_str());
    PQclear(res);
}

vector<vector<string>> getData(PGconn *conn)
{
    vector<vector<string>> data;
    PGresult *res = PQexec(conn, "SELECT id, nama, umur FROM mahasiswa ORDER BY id;");
    if (PQresultStatus(res) == PGRES_TUPLES_OK)
    {
        for (int i = 0; i < PQntuples(res); i++)
            data.push_back({PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2)});
    }
    PQclear(res);
    return data;
}

void drawBox(int y, int x, int h, int w)
{
    // Top border
    mvaddch(y, x, ACS_ULCORNER);
    for (int i = 1; i < w - 1; i++) mvaddch(y, x + i, ACS_HLINE);
    mvaddch(y, x + w - 1, ACS_URCORNER);
    
    // Side borders
    for (int i = 1; i < h - 1; i++)
    {
        mvaddch(y + i, x, ACS_VLINE);
        mvaddch(y + i, x + w - 1, ACS_VLINE);
    }
    
    // Bottom border
    mvaddch(y + h - 1, x, ACS_LLCORNER);
    for (int i = 1; i < w - 1; i++) mvaddch(y + h - 1, x + i, ACS_HLINE);
    mvaddch(y + h - 1, x + w - 1, ACS_LRCORNER);
}

void drawTableGrid(int startY, int startX, int rows, int cols[])
{
    int totalWidth = cols[0] + cols[1] + cols[2] + 4; // +4 for separators
    
    // Top border
    mvaddch(startY, startX, ACS_ULCORNER);
    for (int i = 0; i < cols[0]; i++) mvaddch(startY, startX + 1 + i, ACS_HLINE);
    mvaddch(startY, startX + cols[0] + 1, ACS_TTEE);
    for (int i = 0; i < cols[1]; i++) mvaddch(startY, startX + cols[0] + 2 + i, ACS_HLINE);
    mvaddch(startY, startX + cols[0] + cols[1] + 2, ACS_TTEE);
    for (int i = 0; i < cols[2]; i++) mvaddch(startY, startX + cols[0] + cols[1] + 3 + i, ACS_HLINE);
    mvaddch(startY, startX + totalWidth, ACS_URCORNER);
    
    // Header separator
    mvaddch(startY + 1, startX, ACS_LTEE);
    for (int i = 0; i < cols[0]; i++) mvaddch(startY + 1, startX + 1 + i, ACS_HLINE);
    mvaddch(startY + 1, startX + cols[0] + 1, ACS_PLUS);
    for (int i = 0; i < cols[1]; i++) mvaddch(startY + 1, startX + cols[0] + 2 + i, ACS_HLINE);
    mvaddch(startY + 1, startX + cols[0] + cols[1] + 2, ACS_PLUS);
    for (int i = 0; i < cols[2]; i++) mvaddch(startY + 1, startX + cols[0] + cols[1] + 3 + i, ACS_HLINE);
    mvaddch(startY + 1, startX + totalWidth, ACS_RTEE);
    
    // Data rows
    for (int r = 0; r < rows; r++)
    {
        mvaddch(startY + 2 + r, startX, ACS_VLINE);
        mvaddch(startY + 2 + r, startX + cols[0] + 1, ACS_VLINE);
        mvaddch(startY + 2 + r, startX + cols[0] + cols[1] + 2, ACS_VLINE);
        mvaddch(startY + 2 + r, startX + totalWidth, ACS_VLINE);
    }
    
    // Bottom border
    mvaddch(startY + rows + 2, startX, ACS_LLCORNER);
    for (int i = 0; i < cols[0]; i++) mvaddch(startY + rows + 2, startX + 1 + i, ACS_HLINE);
    mvaddch(startY + rows + 2, startX + cols[0] + 1, ACS_BTEE);
    for (int i = 0; i < cols[1]; i++) mvaddch(startY + rows + 2, startX + cols[0] + 2 + i, ACS_HLINE);
    mvaddch(startY + rows + 2, startX + cols[0] + cols[1] + 2, ACS_BTEE);
    for (int i = 0; i < cols[2]; i++) mvaddch(startY + rows + 2, startX + cols[0] + cols[1] + 3 + i, ACS_HLINE);
    mvaddch(startY + rows + 2, startX + totalWidth, ACS_LRCORNER);
}

int showMenu(const vector<string> &opts)
{
    int sel = 0, key;
    while (true)
    {
        clear();
        
        // Draw title box
        int boxWidth = 30;
        int boxHeight = 3;
        int boxX = COLS/2 - boxWidth/2;
        int boxY = 2;
        
        drawBox(boxY, boxX, boxHeight, boxWidth);
        attron(A_BOLD);
        mvprintw(boxY + 1, COLS/2 - 7, "CRUD MAHASISWA");
        attroff(A_BOLD);
        
        // Draw menu items with boxes
        for (size_t i = 0; i < opts.size(); i++)
        {
            int menuY = boxY + boxHeight + 2 + (i * 2);
            int menuX = COLS/2 - 10;
            
            if (i == sel)
            {
                attron(A_REVERSE);
                mvprintw(menuY, menuX, "  %-16s  ", opts[i].c_str());
                attroff(A_REVERSE);
            }
            else
            {
                mvprintw(menuY, menuX, "  %-16s  ", opts[i].c_str());
            }
        }
        
        mvprintw(LINES - 1, 2, "↑↓: Navigasi | Enter: Pilih");
        refresh();
        
        key = getch();
        if (key == KEY_UP) sel = (sel - 1 + opts.size()) % opts.size();
        else if (key == KEY_DOWN) sel = (sel + 1) % opts.size();
        else if (key == 10) return sel;
    }
}

string getInput(const string &prompt)
{
    clear();
    int boxWidth = 60;
    int boxHeight = 5;
    int boxX = COLS/2 - boxWidth/2;
    int boxY = LINES/2 - boxHeight/2;
    
    drawBox(boxY, boxX, boxHeight, boxWidth);
    mvprintw(boxY + 2, boxX + 2, "%s ", prompt.c_str());
    
    refresh();
    echo();
    char buf[256];
    getnstr(buf, 255);
    noecho();
    return string(buf);
}

void showData(PGconn *conn)
{
    clear();
    auto data = getData(conn);
    
    // Column widths
    int cols[] = {8, 30, 8};
    int startY = 3;
    int startX = 5;
    
    // Draw title
    attron(A_BOLD);
    mvprintw(1, startX, "DATA MAHASISWA");
    attroff(A_BOLD);
    
    // Draw table grid
    int rows = data.size();
    if (rows == 0) rows = 1; // At least show header
    
    drawTableGrid(startY, startX, rows, cols);
    
    // Draw headers
    attron(A_BOLD);
    mvprintw(startY, startX + 2, "ID");
    mvprintw(startY, startX + cols[0] + 3, "NAMA");
    mvprintw(startY, startX + cols[0] + cols[1] + 4, "UMUR");
    attroff(A_BOLD);
    
    // Draw data
    for (size_t i = 0; i < data.size(); i++)
    {
        mvprintw(startY + 2 + i, startX + 2, "%-6s", data[i][0].c_str());
        mvprintw(startY + 2 + i, startX + cols[0] + 3, "%-28s", data[i][1].c_str());
        mvprintw(startY + 2 + i, startX + cols[0] + cols[1] + 4, "%-6s", data[i][2].c_str());
    }
    
    if (data.empty())
    {
        mvprintw(startY + 2, startX + 2, "Tidak ada data");
    }
    
    mvprintw(LINES - 1, 2, "Tekan tombol apapun untuk kembali...");
    refresh();
    getch();
}

bool idExists(PGconn *conn, const string &id)
{
    PGresult *res = PQexec(conn, ("SELECT 1 FROM mahasiswa WHERE id=" + id + " LIMIT 1;").c_str());
    bool exists = false;
    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0)
        exists = true;
    PQclear(res);
    return exists;
}

int main()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    
    auto env = loadEnv(".env");
    PGconn *conn = connectDB(env);
    
    vector<string> menu = {"Tambah", "Lihat", "Ubah", "Hapus", "Keluar"};
    
    while (true)
    {
        int choice = showMenu(menu);
        
        if (choice == 0) // Tambah
        {
            string nama = getInput("Nama:");
            string umur = getInput("Umur:");
            execQuery(conn, "INSERT INTO mahasiswa (nama, umur) VALUES ('" + nama + "', " + umur + ");", "Berhasil ditambahkan!");
            getch();
        }
        else if (choice == 1) // Lihat
        {
            showData(conn);
        }
        else if (choice == 2) // Ubah
        {
            string id = getInput("ID:");
            if (!idExists(conn, id))
            {
                mvprintw(LINES - 2, 2, "ID %s tidak ditemukan! Tekan tombol apapun untuk kembali...", id.c_str());
                refresh();
                getch();
                continue;
            }
            string nama = getInput("Nama baru:");
            string umur = getInput("Umur baru:");
            execQuery(conn, "UPDATE mahasiswa SET nama='" + nama + "', umur=" + umur + " WHERE id=" + id + ";", "Berhasil diubah!");
            getch();
        }
        else if (choice == 3) // Hapus
        {
            string id = getInput("ID:");
            if (!idExists(conn, id))
            {
                mvprintw(LINES - 2, 2, "ID %s tidak ditemukan! Tekan tombol apapun untuk kembali...", id.c_str());
                refresh();
                getch();
                continue;
            }
            execQuery(conn, "DELETE FROM mahasiswa WHERE id=" + id + ";", "Berhasil dihapus!");
            getch();
        }
        else if (choice == 4) // Keluar
        {
            break;
        }
    }
    
    endwin();
    PQfinish(conn);
    cout << "Terima kasih!\n";
    return 0;
}