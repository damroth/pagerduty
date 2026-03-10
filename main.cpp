#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <atomic>
#include "json.hpp"
#include "http_client.hpp"
using json = nlohmann::json;

const std::string pg_api = "https://api.pagerduty.com/";

auto parse_date = [](const std::string &iso) {
    int y;
    uint m, d;
    char sep;
    std::istringstream ss{iso};
    ss >> y >> sep >> m >> sep >> d;
    return std::chrono::sys_days {
        std::chrono::year{y} / std::chrono::month{m} / std::chrono::day{d}
    };
};

int main() {
    std::cout << R"(
  ____                       ____        _
 |  _ \ __ _  __ _  ___ _ __|  _ \ _   _| |_ _   _
 | |_) / _` |/ _` |/ _ \ '__| | | | | | | __| | | |
 |  __/ (_| | (_| |  __/ |  | |_| | |_| | |_| |_| |
 |_|   \__,_|\__, |\___|_|  |____/ \__,_|\__|\__, |
              |__/   Shift Tracker           |___/
)" << std::endl;

    // read config
    std::string pg_config = std::string(std::getenv("HOME")) + "/.config/pagerduty/pg-config";
    std::ifstream file{pg_config};
    if (!file.is_open()) {
        std::cout << "Cannot open config file: " << pg_config << "\n";
        return 1;
    }
    json conf = json::parse(file);
    const std::string user_id = conf["client"]["user_id"];
    const std::string auth = "Authorization: Token token=" + std::string(conf["api"]["key"]);

    HttpClient client{};
    auto now = std::chrono::system_clock::now();
    auto today = std::chrono::floor<std::chrono::days>(now);
    std::chrono::year_month_day ymd{today};
    auto first_day = ymd.year() / ymd.month() / std::chrono::day{1};
    auto last_day = ymd.year() / ymd.month() / std::chrono::last;
    std::string since = std::format("{:%Y-%m-%d}T00:00:00Z", std::chrono::sys_days{first_day});
    std::string until = std::format("{:%Y-%m-%d}T23:59:59Z", std::chrono::sys_days{last_day});

    std::atomic<bool> loading{true};
    std::thread spinner{[&loading]() {
        const char frames[] = {'|', '/', '-', '\\'};
        int i = 0;
        while (loading) {
            std::cout << "\r [+] Fetching oncalls... " << frames[i++ % 4] << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::cout << "\r  [+] Fetching oncalls... done!\n";
    }};

    std::string res = client.get(pg_api + "oncalls?user_ids[]=" + user_id +
        "&since=" + since + "&until=" + until,
        auth);

    loading = false;
    spinner.join();

    uint total_hours = 0;
    uint total_days = 0;
    if(!res.empty()){
       json shift = json::parse(res);

       for(auto &oncall : shift["oncalls"]) {
        auto start = parse_date(oncall["start"]);
        auto end = parse_date(oncall["end"]);

        for (auto day = start; day < end; day += std::chrono::days{1}) {
            std::chrono::weekday wd{day};
            total_hours += (wd == std::chrono::Saturday || wd == std::chrono::Sunday) ? 24 : 16;
        }

        total_days += (end - start).count();
       }
    }

    std::cout << "\nTotal oncall days: " << total_days << std::endl;
    std::cout << "Total oncall hours: " << total_hours << std::endl;

    return 0;
}
