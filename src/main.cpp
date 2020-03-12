#include <iostream>

using namespace std;

int main() {
    // Read contents from stdin
    // How many students? What kind of relationships?
    int numStudents, numChains;
    cin >> numStudents;
    cin.ignore(1); // Ignore the comma (,)
    cin >> numChains;

    int grade;
    int studentFrom, studentTo;

    for (int i = 0; i < numStudents; i++) {
        cin >> grade;
        cout << grade << endl;
    }
    for (int i = 0; i < numChains; i++) {
        cin >> studentFrom;
        cin >> studentTo;
        cout << "Student " << studentFrom << " is sharing answers with student " << studentTo << endl;
    }

    return 0;
}
