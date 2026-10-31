// Logika murni (tanpa header Android) supaya bisa diuji di komputer biasa.
#pragma once
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

// Baris config: paket|key|manufacturer|model
// Cocok bila nama proses == paket, atau proses turunan "paket:nama".
static inline bool comatoze_lookup(const char *cfgpath, const std::string &proc,
                                   std::string &man, std::string &model) {
    FILE *f = fopen(cfgpath, "r");
    if (!f) return false;
    char *line = nullptr;
    size_t cap = 0;
    bool found = false;
    while (!found && getline(&line, &cap, f) > 0) {
        std::string l(line);
        while (!l.empty() && (l.back() == '\n' || l.back() == '\r')) l.pop_back();
        size_t p1 = l.find('|');
        if (p1 == std::string::npos) continue;
        size_t p2 = l.find('|', p1 + 1);
        if (p2 == std::string::npos) continue;
        size_t p3 = l.find('|', p2 + 1);
        if (p3 == std::string::npos) continue;
        std::string pkg = l.substr(0, p1);
        if (pkg.empty()) continue;
        bool match = (proc == pkg) ||
                     (proc.size() > pkg.size() && proc.compare(0, pkg.size(), pkg) == 0 && proc[pkg.size()] == ':');
        if (!match) continue;
        std::string m = l.substr(p2 + 1, p3 - p2 - 1);
        std::string d = l.substr(p3 + 1);
        if (m.empty() || d.empty()) continue;
        man = m;
        model = d;
        found = true;
    }
    free(line);
    fclose(f);
    return found;
}
