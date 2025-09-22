#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <iostream>
#include <string>
#include <vector>
#include "./libs/sqlite3.h"

class Database {
    sqlite3* db;
    int rc;

    void clear() {
        if (stmt) {
            sqlite3_finalize(stmt);
            stmt = nullptr;
        }
    }

public:
    Database(const char *db_path) : db(nullptr), stmt(nullptr) {
        rc = sqlite3_open(db_path, &db);
        if (rc) {
            std::cerr << "Kann die Datenbank nicht oeffnen: " << sqlite3_errmsg(db) << std::endl;
        }
    }

    void excCommand(const char* sql) {
        char* errMessage = nullptr;
        rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMessage);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL-Fehler: " << errMessage << std::endl;
            sqlite3_free(errMessage);
        }
    }

    std::vector<std::vector<std::string>> executeQuery(const std::string& query) {
        std::vector<std::vector<std::string>> results;

        // Abfrage vorbereiten
        rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Fehler beim Vorbereiten der SQL-Abfrage: " << sqlite3_errmsg(db) << std::endl;
            return results;
        }

        // Zeilen abrufen
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            std::vector<std::string> row;
            int numCols = sqlite3_column_count(stmt);

            for (int col = 0; col < numCols; ++col) {
                const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, col));
                row.push_back(text ? text : "NULL");
            }
            results.push_back(row);
        }

        if (rc != SQLITE_DONE) {
            std::cerr << "Fehler beim Ausführen der Abfrage: " << sqlite3_errmsg(db) << std::endl;
        }

        //std::cout << "Abfrage abgeschlossen. Ergebnisse: " << results.size() << " Zeilen." << std::endl;  // Debug-Ausgabe der Anzahl der Ergebnisse


        clear();
        return results;
    }

    ~Database() {
        if (db) {
            sqlite3_close(db);
        }
    }

private:
    sqlite3_stmt* stmt;
};

#endif
