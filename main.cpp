#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <fstream>
#include <random>
#include <cstdint>

using namespace std;
/*
NOTE: Agentic or chatbot AI use is not allowed. Google (including AI-generated answers to a google query) and Stack Overflow is.

Write a C++ programs that:

0a. Implement a program writing 1 million random integers with values 0 to 2^30-1 to a file "points.txt"
0b. Implement a program writing 10,000 random ranges in format "a:b" where a is 0 to 2^29-1 and b = a + (random 128 to 1024) to a file "ranges.txt"

1. Implement a program that has following :

range:
   uint32_t start_point
   uint32_t end_point
   uint32_t value_count

rage_set:
   range_set(uint32_t max_value_count, uint32_t max_point_distance)
   void add_point(uint32_t)
   void add_range(range)
   bool contains(uint32_t)

   vector<range> get_ranges()

get_ranges should produce minimal amount of ranges given limits of value_count and point_distance
get_ranges can be called at any point in program life

single point counts as 1 value, when merging ranges their value count should be added

2. Your program should take max_value_count and max_point_distance as cli arguments and produce "output.txt" in format

"start_point:end_point:value_count"

it must run in a reasonable time (under 10 seconds)

3. Edge cases have been deliberately left out. As you write the code, you should include "README.md" detailing them and how you decided to handle them
*/

struct range_t
{
    uint32_t start_point;
    uint32_t end_point;
    uint32_t value_count;
};

class range_set
{
    map<uint32_t, pair<uint32_t, uint64_t>> iv;
    // could change to uint64_t
    uint32_t max_value_count;
    uint32_t max_point_distance;
    static bool overlaps_or_adjacent(uint32_t a1, uint32_t b1, uint32_t a2, uint32_t b2)
    {
        return a2 <= (uint32_t)(b1 + 1u);
    }

public:
    range_set(uint32_t max_value_count_, uint32_t max_point_distance_) : max_value_count(max_value_count_), max_point_distance(max_point_distance_) {}

    void add_point(uint32_t p)
    {
        add_range(range_t{p, p, 1});
    }

    void add_range(range_t r)
    {
        if (r.end_point < r.start_point)
            swap(r.start_point, r.end_point);
        uint32_t a = r.start_point;
        uint32_t b = r.end_point;
        uint64_t cnt = r.value_count;
        if (cnt == 0)
        {
            return;
        }

        auto it = iv.upper_bound(a);
        if (it != iv.begin())
        {
            auto pit = prev(it);
            if (overlaps_or_adjacent(pit->first, pit->second.first, a, b))
            {
                a = min(a, pit->first);
                b = max(b, pit->second.first);
                cnt += pit->second.second;
                iv.erase(pit);
            }
        }

        while (it != iv.end() && overlaps_or_adjacent(a, b, it->first, it->second.first))
        {
            a = min(a, it->first);
            b = max(b, it->second.first);
            cnt += it->second.second;
            it = iv.erase(it);
        }

        iv[a] = {b, cnt};
    }

    bool contains(uint32_t p) const
    {
        auto it = iv.upper_bound(p);
        if (it == iv.begin())
            return false;
        --it;
        return it->first <= p && p <= it->second.first;
    }

    vector<range_t> get_ranges() const
    {
        vector<pair<uint32_t, pair<uint32_t, uint64_t>>> v;
        v.reserve(iv.size());
        for (auto &e : iv)
            v.push_back(e);

        vector<range_t> out;
        out.reserve(v.size());

        size_t i = 0;
        while (i < v.size())
        {
            uint32_t cur_start = v[i].first;
            uint32_t cur_end = v[i].second.first;
            uint64_t cur_cnt = v[i].second.second;

            size_t j = i + 1;
            while (j < v.size())
            {
                uint32_t next_start = v[j].first;
                uint32_t next_end = v[j].second.first;
                uint64_t next_cnt = v[j].second.second;

                uint32_t new_start = cur_start;
                uint32_t new_end = max(cur_end, next_end);

                bool ok_span = (uint64_t)new_end - (uint64_t)new_start <= max_point_distance;
                bool ok_cnt = cur_cnt + next_cnt <= (uint64_t)max_value_count;

                if (!ok_span || !ok_cnt)
                    break;

                cur_end = new_end;
                cur_cnt += next_cnt;
                ++j;
            }

            range_t rr;
            rr.start_point = cur_start;
            rr.end_point = cur_end;
            rr.value_count = (cur_cnt > numeric_limits<uint32_t>::max()) ? numeric_limits<uint32_t>::max() : (uint32_t)cur_cnt;
            out.push_back(rr);
            i = j;
        }
        return out;
    }
};

static void gen_points_file(const string &path = "points.txt", uint32_t n = 1000000)
{
    ofstream f(path);
    random_device rd;
    mt19937 rng(rd());
    uniform_int_distribution<uint32_t> dist(0, (1u << 30) - 1u);
    for (uint32_t i = 0; i < n; ++i)
    {
        f << dist(rng) << "\n";
    }
}

static void gen_ranges_file(const string &path = "ranges.txt", uint32_t n = 10000)
{
    ofstream f(path);
    random_device rd;
    mt19937 rng(rd());
    uniform_int_distribution<uint32_t> distA(0, (1u << 29) - 1u);
    uniform_int_distribution<uint32_t> distB(128u, 1024u);
    for (uint32_t i = 0; i < n; ++i)
    {
        uint32_t a = distA(rng);
        uint32_t b = a + distB(rng);
        f << a << ":" << b << "\n";
    }
}

static bool parse_range_line(const string &s, range_t &r)
{
    size_t colon = s.find(':');
    if (colon == string::npos)
        return false;
    string sa = s.substr(0, colon);
    string sb = s.substr(colon + 1);
    auto trim = [](string &t)
    {
        size_t i = 0;
        while (i < t.size() && isspace((unsigned char)t[i]))
            i++;
        size_t j = t.size();
        while (j > i && isspace((unsigned char)t[j - 1]))
            j--;
        t = t.substr(i, j - i);
    };
    trim(sa);
    trim(sb);
    //   boost::trim(sa); boost::trim(sb);
    if (sa.empty() || sb.empty())
        return false;
    uint64_t a = 0, b = 0;
    try
    {
        a = stoull(sa);
        b = stoull(sb);
    }
    catch (...)
    {
        return false;
    }
    if (a > numeric_limits<uint32_t>::max() || b > numeric_limits<uint32_t>::max())
        return false;
    r.start_point = (uint32_t)a;
    r.end_point = (uint32_t)b;
    r.value_count = r.end_point >= r.start_point ? (r.end_point - r.start_point) : (r.start_point - r.end_point);
    return true;
}

int main(int argc, char **argv)
{
    cin.tie(nullptr);

    if (argc < 3)
    {
        cerr << "need more than 3 arguments" << endl;
        return 1;
    }

    uint64_t mvc64 = 0, mpd64 = 0;
    try
    {
        mvc64 = stoull(argv[1]);
        mpd64 = stoull(argv[2]);
    }
    catch (...)
    {
        cerr << "invalid numeric args" << endl;
        return 1;
    }

    if (mvc64 == 0 || mvc64 > numeric_limits<uint32_t>::max() || mpd64 > numeric_limits<uint32_t>::max())
    {
        cerr << "arguments out of range" << endl;
        return 1;
    }

    // by default, we will feed the points and ranges from points.txt and ranges.txt to our range_set object before writing that value to output.txt
    // we will have 3 flags:
    // --gen for when we want to randomly generate all points and ranges into points.txt and ranges.txt
    // --no-points to indicate we don't want to add the points from points.txt into range_set
    // --no-ranges to indicate we don't want to add the rangess from range.txt into range_set

    bool do_gen = false;
    bool use_points = true;
    bool use_ranges = true;
    for (int i = 3; i < argc; ++i)
    {
        string opt = argv[i];
        if (opt == "--gen")
            do_gen = true;
        else if (opt == "--no-points")
            use_points = false;
        else if (opt == "--no-ranges")
            use_ranges = false;
    }

    if (do_gen)
    {
        if (use_points)
            gen_points_file();
        if (use_ranges)
            gen_ranges_file();
    }

    range_set rs((uint32_t)mvc64, (uint32_t)mpd64);

    if (use_points)
    {
        ifstream f("points.txt");
        if (f)
        {
            uint64_t x;
            while (f >> x)
            {
                if (x <= numeric_limits<uint32_t>::max())
                    rs.add_point((uint32_t)x);
            }
        }
    }

    if (use_ranges)
    {
        ifstream f("ranges.txt");
        if (f)
        {
            string line;
            while (getline(f, line))
            {
                if (line.empty())
                    continue;
                range_t r;
                if (parse_range_line(line, r))
                    rs.add_range(r);
            }
        }
    }

    auto out = rs.get_ranges();
    ofstream fo("output.txt");
    for (auto &r : out)
    {
        fo << r.start_point << ";" << r.end_point << ";" << r.value_count << endl;
    }

    return 0;
}