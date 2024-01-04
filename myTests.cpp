#include "college.h"
#include <cassert>

using std::string;
using std::cout;

 bool satisfies_pattern(const std::string &str,
                           const std::string &pattern)
    {
        std::size_t str_idx, ptrn_idx, ptrn_len, str_len;
        std::size_t match;
        int start_Asterisk_Idx;

        str_idx = ptrn_idx = 0;
        ptrn_len = pattern.size();
        str_len = str.size();
        match = 0;
        start_Asterisk_Idx = -1;

        while (str_idx < str_len)
        {
            // If current character in pattern is ? or pattern character and
            // str character match, we simply do ++ on indexes.
            if (ptrn_idx < ptrn_len && (pattern[ptrn_idx] == '?' ||
                                        str[str_idx] == pattern[ptrn_idx]))
            {
                str_idx++;
                ptrn_idx++;
            }
            else if (ptrn_idx < ptrn_len && pattern[ptrn_idx] == '*')
            {
                // **...** == *
                while (ptrn_idx < ptrn_len && pattern[ptrn_idx] == '*')
                    ptrn_idx++;

                ptrn_idx--;

                // If special character * is the last character in pattern
                // we return true, since it means that all not checked
                // characters we have in str are good.
                if (ptrn_idx == ptrn_len - 1)
                    return true;

                // We remember idx of last found *, and idx in str when we
                // found *, we want to be able to come back to them when we
                // won't find a match in first if, or we won't find another *.
                start_Asterisk_Idx = ptrn_idx;
                match = str_idx;

                ptrn_idx++;
            }
            else if (start_Asterisk_Idx != -1)
            {
                // We didnt find match between str and pattern, also we didnt
                // find another *, so we need to come back to previous * that
                // we remembered and increase matched idx. Basically what we do
                // is we find how long substring in our str is assigned to *.
                // i.e. Biology and *o?y we do: 
                // 1) match = 0, start_idx = 0, ptrn_idx = 1
                // 2) o != B but start_idx != -1 so we do match++, ptrn_idx = 1
                // 3) o != i ---||--- 
                // 4) o == o so we do ptrn_idx++ and str_idx++
                // 5) ? == l ---||---
                // 6) y != o but start_idx != -1 so we do match++, ptrn_idx = 1
                // now match = 3 so currently * = Bio 
                // 7) o != l so match++, str_idx = match, * = Biol
                // 8) o == o, 9) ? == g, 10) y == y 
                match++;
                str_idx = match;
                ptrn_idx = start_Asterisk_Idx + 1;
            }
            else
            {
                return false;
            }
                
        }

        // Pattern can be longer than our string so we need to check if in this
        // pattern there arent any characters apart from ? and *.
        while(ptrn_idx < ptrn_len)
        {
            if(pattern[ptrn_idx] != '*' && pattern[ptrn_idx] != '?')
                return false;
            ptrn_idx++;
        }

        // If there are no unchecked characters left in str we found pattern.
        if(str_idx == str_len)
            return true;
        
        return false;
    }


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

    string pattern{"*o?y"};
    string biology("Biology");

    if(satisfies_pattern(biology, pattern))
        cout << "dziala!\n";

    // TEST FOR PATTERN FINDING:
    assert(satisfies_pattern("Biology", "*o?y"));
    assert(satisfies_pattern("baaabab", "*****ba*****ab"));
    assert(satisfies_pattern("Ala", "*"));
    assert(satisfies_pattern("Ala", "???"));
    assert(satisfies_pattern("Biology", "Biology"));
    assert(satisfies_pattern("Biology", "B*l****?g?"));
    assert(satisfies_pattern("Biology", "??????????????"));
    assert(satisfies_pattern("Biology", "*o?o*"));
    assert(!satisfies_pattern("Biology", "*b*"));
    
    // TEST FOR FINDING COURSES
    auto courses = college.find_courses("C++");
    assert(courses.size() == 1);
    assert((*courses.begin())->get_name() == "C++");
    // (*courses.begin())->change_activeness(false); // gives error since we return set of const shared pointers

    auto all_courses = college.find_courses("*");
    assert(all_courses.size() == 7);
    std::vector<string> courses_names{"Algebra", "Analysis", "Biology", "C++",
    "Geometry", "History", "Python" };
    size_t i = 0;
    for(auto course : all_courses)
    {
        cout << i << ", found course: " << course->get_name() << "\n";
        assert(course->get_name() == courses_names[i]);
        i++;
    }

    return 0;
}