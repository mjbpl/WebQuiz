#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main() {
	string s;
	ofstream fout("quizanswers.txt");
	getline(cin, s);
	fout << s << '\n';
	const int ncases = stoi(s);
	cout << "Content-type: text/html\n\n";
	cout << "<!DOCTYPE html>\n<html><body bgcolor=\"#FFFFFF\" text=\"#000000\" link=\"#0000ff\" vlink=\"#000080\"><h2>Imagination's Independence day competetion</h2>\n";
//	cout << "<!DOCTYPE html>\n<html><body text=\"#000000\" link=\"#0000ff\" vlink=\"#000080\" background=\"http://bestanimations.com/Flags/Asia/india/india-flag-waving-animated-gif-12.gif\"><h2>Imagination's Independence day competetion</h2>\n";
	cout << "<img src=\"http://bestanimations.com/Flags/Asia/india/india-flag-waving-animated-gif-12.gif\"><hr>\n";
	cout << "<form action=\"quizsubmit.img\" method=\"post\"><h3>Name:<br> <input type=\"text\" name=\"name\"><br>\n";
	cout << "Employee id:<br> <input type=\"number\" name=\"eid\"><br> Location:<br><select name=\"Location\">\n";
	cout << "<option value=\"Pune\">Pune</option> <option value=\"Blr\">Bangalore</option><option value=\"Hyd\">Hyderabad</option>\n";
	cout << "</select></h3><hr>\n";
	for(int i = 1; i <= ncases; ++i) {
		getline(cin, s);
		cout << "<h3> " << i << ". " << s << "</h3>\n";
		for(int j = 1; j <= 4; ++j) {
			getline(cin, s);
			cout << "<input type=\"checkbox\" name=\"Answers" << "\" value=\"Q" << i << 'A' << j << "\"> " << s << "<br>\n";
		}
  		cout << "<br>\n";
		getline(cin, s);
		fout << s << '\n';
	}
	cout << "<input type=\"submit\" value=\"Submit Answers\">\n</form>\n";
	cout << "<BR><BR><h4><a href=\"scoreboard.img\">Check Scoreboard</h4>\n";
	cout << "</body></html>\n";
}
