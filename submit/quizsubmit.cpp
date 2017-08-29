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

struct mytime /* Time since Aug 29 */
{
	int secs;
	mytime() {
		time_t rawtime;
		time(&rawtime);
		struct tm aug29_17 = {0};
		aug29_17.tm_hour = aug29_17.tm_min = aug29_17.tm_sec = 0;
		aug29_17.tm_year = 2017 - 1900;
		aug29_17.tm_mon = 8 - 1;
		aug29_17.tm_mday = 29;
		secs = (int)( difftime(rawtime, mktime(&aug29_17)) + 0.5 );
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

static void updateData(const string &eid, int good, const string &name, const string &clientid, const string &loc, const mytime &mt, const string &response)
{
	const int contestEnd = (24 + 12) * 60 * 60 + 15 * 60; // Ends at 12:15
	if(mt.secs > contestEnd) {
		cout << "Time is up. Competetion close time : Aug 16, 2017 12:00:00 passed\n<BR>";
		return;
	}
	sem_t *sem_id = sem_open("/b_lock", O_CREAT, 0660, 1);
	if(sem_id == SEM_FAILED) {
		const int code = errno;
		cout << "Something wrong happened. Please contact the organizers [Code = " << code << "]\n<BR>";
		return;
	}
	sem_wait(sem_id);

	ofstream fout(WWW_DIR "database.csv", std::ofstream::app);
	if(fout) {
		fout << mt.get_sec() << ',' << eid << ',' << good << ',' << '"' << name << "\",\"" << loc << "\"," << clientid << ',' << response << endl;
	} else {
		cout << "Something wrong happened. Please conatct the organizers [Code = database open error]\n<BR>";
	}
	fout.close();

	sem_post(sem_id);
	sem_close(sem_id);
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
		const int e = errno;
		cout << "Something wrong happened. Please inform the organizers [Error code = " << e << "]\n<BR>";
		return;
	}
	sem_wait(sem_id);
	ifstream fin(WWW_DIR "database.csv");
	/* FIXME: Add penalty code here if required */
	string s;
	std::map<string, int> countAttempts;
	while( getline(fin, s) ) {
		rec r(s);
		countAttempts[r.eid]++;
		if(countAttempts[r.eid] <= 3) {
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
	mytime mtime;
	vector<int> expected = loadExpected();
	vector<int> actual(expected.size(), 0);
	try {
        Cgicc formData;
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

 	    string clientid, name, eid, loc, response;
 	    {
		   const CgiEnvironment& env = formData.getEnvironment();
		   clientid = env.getRemoteHost();
		   if(clientid.empty()) clientid = env.getRemoteAddr();
	    }

        form_iterator fvalue1 = formData.getElement("name");
        if( !fvalue1->isEmpty() && fvalue1 != (*formData).end()) {
          // cout << "Name: " << **fvalue1 << endl;
		  name = **fvalue1;
        } else {
          cout << h2("Please go back and enter your name") << endl;
		  cout << clientid << endl;
		  goto EXIT_LABEL;
	    }

        cout << p();

        fvalue1 = formData.getElement("eid");
        if( !fvalue1->isEmpty() && fvalue1 != (*formData).end()) {
          // Note this is just a different way to access the string class.
          // See the YoLinux GNU string class tutorial.
          //cout << "Employee id: " << (**fvalue1).c_str() << endl;
		  eid = **fvalue1;
        } else {
          cout << h2("Please go back and enter your employee id") << endl;
		  cout << clientid << endl;
		  goto EXIT_LABEL;
	    }

        cout << p();

        fvalue1 = formData.getElement("Location");
        if( !fvalue1->isEmpty() && fvalue1 != (*formData).end()) {
		  loc = **fvalue1;
          //cout << "Location: " << **fvalue1 << endl;
        }

        cout << p();
		{
	       form_iterator fvalue4 = formData.getElement("Answers");
		   while( !fvalue4->isEmpty() && fvalue4 != (*formData).end()) {
				string val(**fvalue4);
				int i = 1, sz = static_cast<int>(val.size()), q = 0;
				do {
					q = q * 10 + val[i++] - '0';
				} while(val[i] != 'A');
				actual[q-1] += (1 << (val[i + 1] - '0'));

				++fvalue4;
		   }
		   for(auto it1 = begin(expected), it2 = begin(actual); it1 != end(expected); ++it1, ++it2) {
				   ++total;
				   if(*it1 == *it2) ++good;
				   response += hex_[*it2];
		   }
		   cout << "<h3>Dear <u>" << name << "</u>, you answered <u>" << good << "</u> answers correct out of total <u>" << total << "</u> questions<h3><BR>" << endl;
		   updateData(eid, good, name, clientid, loc, mtime, response);
		   cout << "<hr>" << endl;
		   dumpScoreBoard(); 
		}
EXIT_LABEL:
       // Close the HTML document
       cout << body() << html();
    }
    catch(exception& e) {
       // handle any errors here.
       cout << "ERROR!!" << endl;
    }
    return 0;   // To avoid Apache errors.
}

