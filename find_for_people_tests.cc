#include "college.h"
#include <cassert>

class StrangeClass
{
public:
	StrangeClass() = delete;
	StrangeClass(int arg)
	{
		strangeNumber = arg;
	}
	~StrangeClass() = default;

private:
	int strangeNumber;
};

void add_people(College& college)
{
	assert(college.add_person<Student>("Jan", "Kowalski"));
	assert(!college.add_person<Teacher>("Jan", "Kowalski"));
	assert(college.add_person<Student>("Pawel", "Kowalski", false));
	assert(college.add_person<Teacher>("Jacek", "Chlebus"));
	assert(college.add_person<Teacher>("Kamil", "Litwiniuk"));
	assert(college.add_person<PhDStudent>("Alicja", "Fiszer"));
	assert(college.add_person<PhDStudent>("Jakub", "Kubanski"));
}

void add_special_people(College& college)
{
	assert(college.add_person<Student>("Jacek", "Kowalski"));
	assert(college.add_person<Student>("Pawel", "Kowalski", false));
	assert(college.add_person<Teacher>("Jacek", "Litwiniuk"));
	assert(college.add_person<PhDStudent>("Jacek", "Fiszer"));
	assert(college.add_person<PhDStudent>("Kowalski", "Jan"));
}

int PeczarsTests(College& college)
{
	auto people = college.find<Person>("*", "*");
	assert(people.size() == 6);
	assert((*people.begin())->get_name() == "Jacek");
	assert((*people.begin())->get_surname() == "Chlebus");

	for (auto const& p : people)
		std::clog << p->get_name() << ' ' << p->get_surname() << '\n';

	people = college.find<Person>("*", "*k*");
	assert(people.size() == 4);

	for (auto const& p : people)
		std::clog << p->get_name() << ' ' << p->get_surname() << '\n';

	assert(college.find<Student>("*", "*").size() == 4);
	assert(college.find<PhDStudent>("*", "*").size() == 2);
	assert(college.find<Teacher>("*", "*").size() == 4);

	return 0;
}

// The point of this test is to check whether find() works properly 
// with "strange" types like int etc.
int garbageDataTypesTest(College& college)
{
	StrangeClass sc(69);
	assert(college.find<StrangeClass>("*", "*").size() == 0);
	assert(college.find<decltype(sc)>("*", "*").size() == 0);
	assert(college.find<std::string>("*", "*").size() == 0);
	assert(college.find<std::vector<std::string>>("*", "*").size() == 0);

	return 0;
}

// Test if the fin() function correctly looks up names and surnames.
int find_names_and_surnames_test(College& college)
{
	std::string name_pattern{ "Jan" };
	std::string name2_pattern{ "Jacek" };
	std::string surname_pattern{ "Kowalski" };
	assert(college.find<Person>(name_pattern, surname_pattern).size() == 0);
	assert(college.find<Person>(surname_pattern, name_pattern).size() == 1);
	assert(college.find<Person>(name2_pattern, surname_pattern).size() == 1);
	assert(college.find<Person>(name2_pattern, "*").size() == 3);
	assert(college.find<Person>("*", surname_pattern).size() == 2);

	return 0;
}

int main()
{
	College college;
	add_people(college);
	assert(PeczarsTests(college) == 0);

	College college2;
	add_people(college2);
	assert(garbageDataTypesTest(college2) == 0);

	College college3;
	add_special_people(college3);
	assert(find_names_and_surnames_test(college3) == 0);

	return 0;
}