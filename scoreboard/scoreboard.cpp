#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <algorithm>

#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include "cgicc/CgiEnvironment.h"
#include "cgicc/CgiDefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

using namespace std;
using namespace cgicc;

const string hex_("0123456789ABCDEF");

struct mytime /* Time since Aug 14 */
{
	int secs;
	mytime() {
		time_t rawtime;
		time(&rawtime);
		struct tm aug14_17 = {0};
		aug14_17.tm_hour = aug14_17.tm_min = aug14_17.tm_sec = 0;
		aug14_17.tm_year = 2017 - 1900;
		aug14_17.tm_mon = 8 - 1;
		aug14_17.tm_mday = 14;
		secs = (int)( difftime(rawtime, mktime(&aug14_17)) + 0.5 );
	}
	mytime(int t) : secs(t) {	}
	int get_sec() const { return secs; }
	string get_pri() const {
		int cp = secs;
		const int sc = cp % 60; cp /= 60;
		const int mn = cp % 60; cp /= 60;
		const int hr = cp % 24;
		string ret("00:00:00");
		ret[0] = hex_[hr/10]; ret[1] = hex_[hr % 10];
		ret[3] = hex_[mn/10]; ret[4] = hex_[mn % 10];
		ret[6] = hex_[sc/10]; ret[7] = hex_[sc % 10];
		return ret;
	}
	bool operator < (const mytime &mt) const { return secs < mt.secs; }
	bool operator ==(const mytime &mt) const { return secs == mt.secs;}
};

#define WWW_DIR "/var/www/indday/"
static vector<int> loadExpected()
{
	ifstream fin(WWW_DIR "quizanswers.txt");
	int n;
	fin >> n;
	vector<int> v(n, 0);
	for(int i = 0; i < n; ++i) {
		int z;
		fin >> z;
		while(z) {
			const int k = z % 10;
			v[i] += (1 << k);
			z /= 10;
		}
	}
	return v;
}	

struct rec {
	int time_, score;
	string name, loc, eid;
	rec() {}
	rec(const string &s) {
		int i = 0, j = 0;
		time_ = 0;
		while(s[j] != ',') {
			time_ = time_ * 10 + s[j++] - '0';
		}
		++j;
		eid.clear();
		while(s[j] != ',') eid += s[j++];
		++j;
		score = 0;
		while(s[j] != ',') {
			score = score * 10 + s[j++] - '0';
		}
		++j;
		++j;
		name.clear();
		while(s[j] != '"') name += s[j++];
		++j;
		++j;
		++j;
		loc.clear();
		while(s[j] != '"') loc += s[j++];
		if(loc == "Blr") loc = "Bangalore";
		else if(loc == "Hyd") loc = "Hyderabad";
	}
	bool operator ==(const rec &r) const {
		return time_ == r.time_ && score == r.score;
	}
	bool operator <(const rec &r) const {
		if(score != r.score) return score > r.score;
		if(time_ != r.time_) return time_ < r.time_;
		return eid < r.eid;
	}
};


static void dumpScoreBoard()
{
	std::map< string, pair<int, rec> > m;
	{
	sem_t *sem_id = sem_open("/b_lock", O_CREAT, 0660, 1);
	if(sem_id == SEM_FAILED) {
		cout << "Something wrong happened.\n<BR>";
		cout << errno << endl;
		return;
	}
	sem_wait(sem_id);
	ifstream fin(WWW_DIR "database.csv");
	/* FIXME: Add penalty code here if required */
	string s;
	std::map<string, int> seencount;
	while( getline(fin, s) ) {
		rec r(s);
		seencount[r.eid]++;
		if(seencount[r.eid] <= 3) {
			auto it = m.find(r.eid);
			if(it == m.end() || it->second.first < r.score) m[r.eid] = make_pair(r.score, r);
		}
	}
	fin.close();
	sem_post(sem_id);
	//sem_close(sem_id);
	sem_unlink("/b_lock");
	}
	vector<rec> vr;
	vr.reserve(m.size());
	for(auto it : m) {
		vr.push_back(it.second.second);
	}
	sort( begin(vr), end(vr) );
	cout << "<table style=\"width:100%;text-align:center;\">\n";
	//cout << "<table style=\"text-align:center;border: 2px solid black;\">\n";
	cout << "<tr><th style=\"border: 1px solid black;width:5%;\"><B>Standing</B></th><th style=\"border: 1px solid black;\"><B>Name</B></th><th style=\"border: 1px solid black;\"><B>Location</B></th><th style=\"border: 1px solid black;\"><B>Score</B></th><th style=\"border: 1px solid black;\"><B>Time</B></th></tr>\n";
	int standing = 1;
	for(auto it : vr) {
		mytime mt_(it.time_);
		cout << "<tr><td style=\"border: 1px solid black;width:5%;\">" << (standing++) << "</td><td style=\"border: 1px solid black;\">" << it.name << "</td><td style=\"border: 1px solid black;\">" << it.loc << "</td><td style=\"border: 1px solid black;\">" << it.score << "</td><td style=\"border: 1px solid black;\">" << mt_.get_pri() << "</td></tr>\n";
	}
	cout << "</table>" << endl;
}

int main(int argc, char **argv)
{
		int good = 0, total = 0;

        // Send HTTP header: Content-type: text/html
        cout << HTTPHTMLHeader() << endl;

        // Print: <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd">
        cout << HTMLDoctype(HTMLDoctype::eStrict) << endl;

        // Print: <html lang="en" dir="LTR">
        cout << html().set("lang", "EN").set("dir", "LTR") << endl;

        // Set up the HTML document
        cout << html() << head() << title("Result") << head() << endl;
        cout << body().set("bgcolor","#cccccc").set("text","#000000").set("link","#0000ff").set("vlink","#000080") << endl;

		   dumpScoreBoard(); 
       // Close the HTML document
       cout << body() << html();
    return 0;   // To avoid Apache errors.
}

