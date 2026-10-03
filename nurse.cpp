#include <SQLiteCpp/SQLiteCpp.h>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <optional>
#include <print>
#include <sstream>
#include <string>
#include <vector>

class nurse {
  private:
    static std::filesystem::path get_db_path() {
        const char *home = std::getenv("HOME");
        if (!home)
            throw std::runtime_error("HOME environment variable not set.");

        std::filesystem::path dir = std::filesystem::path(home) / ".local" / "share" / "nurse";
        if (!std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }

        return dir / "nurse.db";
    }

    static void init_db(SQLite::Database &db) {
        db.exec("CREATE TABLE IF NOT EXISTS logs ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "date TEXT, time TEXT, user TEXT, tag TEXT, command TEXT)");
    }

  public:
    static void print_help() {
        std::println("Usage:  \n\tnurse :tag: user_command\n");
        std::println("\t--show\t\tShows the logged commands");
        std::println("\t--clear\t\tClears all the logged commands");

        std::println("\nExamples:");
        std::println("\t (1.) nurse :node: pip install nodejs-24");
        std::println("\t (2.) nurse :Flutter_package: pip install fl-sdk-v17 nexpress fl-deps");
    }

    static void log_data(const std::string &tag, const std::string &command) {
        try {
            SQLite::Database db(get_db_path().string(),
                                SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
            init_db(db);

            const char *user = std::getenv("USER");
            auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            struct tm *tm_info = std::localtime(&now);
            char date_buf[32], time_buf[32];
            std::strftime(date_buf, sizeof(date_buf), "%Y-%m-%d", tm_info);
            std::strftime(time_buf, sizeof(time_buf), "%H:%M:%S", tm_info);

            SQLite::Statement query(
                db, "INSERT INTO logs (date, time, user, tag, command) VALUES (?, ?, ?, ?, ?)");
            query.bind(1, date_buf);
            query.bind(2, time_buf);
            query.bind(3, user ? user : "unknown");
            query.bind(4, tag);
            query.bind(5, command);
            query.exec();

        } catch (std::exception &e) {
            std::println("Exception logging data: {}", e.what());
        }
    }

    static void show_logs() {
        try {
            SQLite::Database db(get_db_path().string(), SQLite::OPEN_READONLY);

            std::println("{:<4} | {:<12} | {:<10} | {:<10} | {:<15} | {}", "ID", "Date", "Time",
                         "User", "Tag", "Command");
            std::println("{:-<4}-+-{:-<12}-+-{:-<10}-+-{:-<10}-+-{:-<15}-+-{:-<30}", "", "", "", "",
                         "", "");

            SQLite::Statement query(
                db, "SELECT id, date, time, user, tag, command FROM logs ORDER BY id ASC");
            while (query.executeStep()) {
                std::println("{:<4} | {:<12} | {:<10} | {:<10} | {:<15} | {}",
                             query.getColumn(0).getInt(), query.getColumn(1).getString(),
                             query.getColumn(2).getString(), query.getColumn(3).getString(),
                             query.getColumn(4).getString(), query.getColumn(5).getString());
            }
        } catch (std::exception &e) {
            std::println("No logs found or unable to access database: {}", e.what());
        }
    }

    static void clear_logs() {
        try {
            SQLite::Database db(get_db_path().string(), SQLite::OPEN_READWRITE);
            db.exec("DROP TABLE IF EXISTS logs");
            std::println("Logs cleared successfully.");
        } catch (std::exception &e) {
            std::println("No logs to clear or database error: {}", e.what());
        }
    }
};

class parser {
    using tokens_list = std::vector<std::string>;
    tokens_list words {};

  public:
    parser(int argc, char **raw_input) {
        words.reserve(argc);
        for (int i {}; i < argc; i++)
            words.push_back(raw_input[i]);
    }

    struct parsed_data {
        std::string tag;
        std::string command;
    };

    auto parse() -> std::optional<parsed_data> {
        if ((words.size() > 2) && (words[1] != "--help")) {
            if (words[1].size() > 2) {
                if (words[1].starts_with(":") && words[1].ends_with(":")) {
                    std::ostringstream stream {};
                    for (size_t i {2}; i < words.size(); i++) {
                        stream << words[i];
                        if (i + 1 < words.size())
                            stream << " ";
                    }
                    std::string tag = words[1].substr(1, words[1].size() - 2);
                    return parsed_data {tag, stream.str()};
                }
            }
        }
        return std::nullopt;
    }

    bool is_show() const { return words.size() == 2 && words[1] == "--show"; }
    bool is_clear() const { return words.size() == 2 && words[1] == "--clear"; }
};

auto main(int argc, char **argv) -> int {
    parser raw_input {argc, argv};

    if (raw_input.is_show()) {
        nurse::show_logs();
        return 0;
    }

    if (raw_input.is_clear()) {
        nurse::clear_logs();
        return 0;
    }

    if (auto input {raw_input.parse()}; input) {
        nurse::log_data(input->tag, input->command);
        std::system(input->command.c_str());
    } else {
        nurse::print_help();
    }
}