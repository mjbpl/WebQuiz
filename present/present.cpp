#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <algorithm>

using namespace std;

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
static void loadEditorials()
{
#if 1
	system("cat " WWW_DIR "editorials.html");
#else
	ifstream fin(WWW_DIR "editorials.html");
	string s;
	while(getline(fin, s)) cout << s << '\n';
#endif
}
static void loadQuestions()
{
	ifstream fin(WWW_DIR "mathpuz.html");
	string s;
	while(getline(fin, s)) cout << s << '\n';
}
static void loadRules()
{
	ifstream fin(WWW_DIR "mathdummy.html");
	string s;
	while(getline(fin, s)) cout << s << '\n';
}	

int main(int argc, char **argv)
{
	mytime mtime;
	const int duration = 45;		// End time: Start time + 45 min
	const int startTime = (24 + 11) * 60 * 60 + 30 * 60;	// Start time 11:30 on 30th
	const int endTime = startTime + duration * 60;
	if(mtime.get_sec() < startTime) loadRules();
	else if(mtime.get_sec() > endTime) loadEditorials();
	else loadQuestions();
    return 0;   // To avoid Apache errors.
}

