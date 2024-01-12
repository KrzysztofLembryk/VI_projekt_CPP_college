#include "college.h"
#include <cassert>

int main()
{

    Person *p1 = new Person("Janina", "Kowalska");
    Person *p2 = new Student("Piotr", "Nowak");
    Person *p3 = new Teacher("Wirgiliusz", "Kosobrudzki");
    Person *p4 = new PhDStudent("Cecylia", "Dorotea");

    delete p1;
    delete p2;
    delete p3;
    delete p4;

    // Tworzymy obiekt reprezentujący uczelnię i zarządzamy nią.
    College college;

    assert(college.add_course("Analysis"));
    assert(college.add_course("Algebra"));
    assert(college.add_course("Geometry"));
    assert(college.add_course("C++"));
    assert(college.add_course("Python"));
    assert(college.add_course("History", false));
    assert(college.add_course("Biology"));

    auto courses = college.find_courses("C++");
    assert(courses.size() == 1);
    assert((*courses.begin())->get_name() == "C++");

    courses = college.find_courses("A*");
    assert(courses.size() == 2);
    auto it1 = courses.begin();
    assert((*it1)->get_name() == "Algebra");
    assert((*++it1)->get_name() == "Analysis");

    courses = college.find_courses("*o?y");
    assert(courses.size() == 2);
    it1 = courses.begin();
    assert((*it1)->get_name() == "Biology");
    assert((*++it1)->get_name() == "History");

    assert(college.add_person<Student>("Jan", "Kowalski"));
    assert(!college.add_person<Teacher>("Jan", "Kowalski"));
    assert(college.add_person<Student>("Pawel", "Kowalski", false));
    assert(college.add_person<Teacher>("Jacek", "Chlebus"));
    assert(college.add_person<Teacher>("Kamil", "Litwiniuk"));
    assert(college.add_person<PhDStudent>("Alicja", "Fiszer"));
    assert(college.add_person<PhDStudent>("Jakub", "Kubanski"));

    assert(!college.add_person<Student>("Jan", "Kowalski"));
    assert(!college.add_person<Student>("Jan", "Kowalski", true));
    assert(!college.add_person<Student>("Jan", "Kowalski", false));
    assert(!college.add_person<PhDStudent>("Jan", "Kowalski"));
    assert(!college.add_person<Teacher>("Jan", "Kowalski"));
    assert(!college.add_person<Teacher>("Alicja", "Fiszer"));
    assert(!college.add_person<PhDStudent>("Jacek", "Chlebus"));

    // auto people = college.find<Person>("*", "*");
    // assert(people.size() == 6);
    // assert((*people.begin())->get_name() == "Jacek");
    // assert((*people.begin())->get_surname() == "Chlebus");

    // for (auto const &p : people)
    //     std::clog << p->get_name() << ' ' << p->get_surname() << '\n';

    // people = college.find<Person>("*", "*k*");
    // assert(people.size() == 4);

    // std::cout << "CHUK\n";

    // for (auto const &p : people)
    //     std::clog << p->get_name() << ' ' << p->get_surname() << '\n';

    std::cout << "Finding students:\n";

    assert(college.find<Student>("*", "*").size() == 4);

    std::cout << "Finding PHDDstudents:\n";
    assert(college.find<PhDStudent>("*", "*").size() == 2);

    std::cout << "Finding Teachers:\n";
    assert(college.find<Teacher>("*", "*").size() == 4);

    

    auto jan_kowalski = *college.find<Student>("Jan", "Kowalski").begin();
    auto pawel_kowalski = *college.find<Student>("Pawel", "Kowalski").begin();
    auto alicja_fiszer = *college.find<PhDStudent>("Alicja", "Fiszer").begin();
    auto jakub_kubanski = *college.find<PhDStudent>("Jakub", "Kubanski").begin();
    auto jacek_chlebus = *college.find<Teacher>("Jacek", "Chlebus").begin();
    auto cxx = *college.find_courses("C++").begin();
    auto algebra = *college.find_courses("Algebra").begin();
    auto analysis = *college.find_courses("Analysis").begin();
    auto geometry = *college.find_courses("Geometry").begin();
    auto history = *college.find_courses("History").begin();
    std::cout << "KONIEC\n";
    return 0;
}