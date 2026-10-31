// Logika murni (tanpa header Android) supaya bisa diuji di komputer biasa.
#pragma once
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

// Baris config: paket|key|manufacturer|model[|on[|full]]
//   on   : "0" = spoof dimatikan untuk paket ini (default 1 bila kosong / format lama)
//   full : "1" = ikut ganti Build.BRAND/DEVICE/PRODUCT (default 0)
// Cocok bila nama proses == paket, atau proses turunan "paket:nama".
static inline bool comatoze_lookup(const char *cfgpath, const std::string &proc,
                                   std::string &man, std::string &model, bool &full) {
    FILE *f = fopen(cfgpath, "r");
    if (!f) return false;
    char *line = nullptr;
    size_t cap = 0;
    bool found = false;
    while (!found && getline(&line, &cap, f) > 0) {
        std::string l(line);
        while (!l.empty() && (l.back() == '\n' || l.back() == '\r')) l.pop_back();
        std::vector<std::string> v;
        size_t start = 0;
        for (;;) {
            size_t p = l.find('|', start);
            if (p == std::string::npos) { v.push_back(l.substr(start)); break; }
            v.push_back(l.substr(start, p - start));
            start = p + 1;
        }
        if (v.size() < 4) continue;
        const std::string &pkg = v[0];
        if (pkg.empty()) continue;
        bool match = (proc == pkg) ||
                     (proc.size() > pkg.size() && proc.compare(0, pkg.size(), pkg) == 0 && proc[pkg.size()] == ':');
        if (!match) continue;
        if (v.size() > 4 && v[4] == "0") continue;  // dimatikan
        if (v[2].empty() || v[3].empty()) continue;
        man = v[2];
        model = v[3];
        full = (v.size() > 5 && v[5] == "1");
        found = true;
    }
    free(line);
    fclose(f);
    return found;
}
