#include "college.h"
#include <cassert>


int main()
{
    PhDStudent phd1("Krzys", "Lembrok");

    College college;
    
    assert(college.add_course("Analysis"));
    assert(college.add_course("Algebra"));
    assert(college.add_course("Geometry"));
    assert(college.add_course("C++"));
    assert(college.add_course("Python"));
    assert(college.add_course("History", false));
    assert(college.add_course("Biology"));
    assert(!college.add_course("C++"));
    assert(!college.add_course("Python"));

    auto analysis = std::make_shared<Course>("Analysis");
    assert(college.change_course_activeness(analysis, true) == false);

    return 0;
}