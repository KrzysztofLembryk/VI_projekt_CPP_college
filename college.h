#ifndef COLLEGE_H
#define COLLEGE_H

#include <string>
#include <iostream>
#include <map>
#include <set>
#include <memory>
#include <concepts>
#include <exception>

class Course
{
public:
    Course() = delete;
    Course(std::string name, bool is_active = true) : course_name(name),
                                                      active(is_active) {}

    std::string get_name() const noexcept
    {
        return course_name;
    }

    bool is_active() const noexcept
    {
        return active;
    }

    void change_activeness(bool new_val) noexcept
    {
        active = new_val;
    }

private:
    std::string course_name;
    bool active;
};

class Person
{
public:
    Person() = delete;

    Person(std::string _name, std::string s_name) : name(_name),
                                                    surname(s_name) {}
    virtual ~Person() = default;

    virtual std::string get_name() const noexcept
    {
        return name;
    }

    virtual std::string get_surname() const noexcept
    {
        return surname;
    }

protected:
    // using course_const_sp = std::shared_ptr<Course>;

    struct people_cmp
    {
        bool operator()(const std::shared_ptr<Person> a,
                        const std::shared_ptr<Person> b) const
        {
            if (a->get_name() != b->get_name())
            {
                return a->get_name() < b->get_name();
            }
            else
            {
                return a->get_surname() < b->get_surname();
            }
        }
    };

    struct my_cmp
    {
        bool operator()(const std::shared_ptr<Course> a,
                        const std::shared_ptr<Course> b) const
        {
            return a->get_name() < b->get_name();
        }
    };

    struct my_cmp_const
    {
        bool operator()(const std::shared_ptr<const Course> a,
                        const std::shared_ptr<const Course> b) const
        {
            return a->get_name() < b->get_name();
        }
    };

private:
    std::string name;
    std::string surname;
};

class Student : public virtual Person
{
public:
    Student() = delete;

    Student(std::string name, std::string surname, bool is_active = true) : 
        Person(name, surname), active(is_active) {}

    ~Student() = default;

    bool is_active() const noexcept
    {
        return active;
    }

    const auto &get_courses() const noexcept
    {
        return subjects_I_attend_const;
    }

    friend class College;

protected:
    std::set<std::shared_ptr<const Course>, my_cmp_const> 
        subjects_I_attend_const;
    std::set<std::shared_ptr<Course>, my_cmp> subjects_I_attend;
    bool active;
};

class Teacher : public virtual Person
{
public:
    Teacher() = delete;

    Teacher(std::string name, std::string surname) : Person(name, surname) {}

    ~Teacher() = default;

    const auto &get_courses() const
    {
        return subjects_I_handle_const;
    }

    friend class College;

    friend class College;

protected:
    std::set<std::shared_ptr<const Course>, my_cmp_const> 
        subjects_I_handle_const;
    std::set<std::shared_ptr<Course>, my_cmp> subjects_I_handle;
};

class PhDStudent : public Student, public Teacher
{
public:
    PhDStudent() = delete;
    // Thanks to virtual inheritance, Person constructor invoked only once.
    PhDStudent(std::string name, std::string surname,
               bool is_active = true) : Person(name, surname),
                                        Student(name, surname, is_active),
                                        Teacher(name, surname) {}
    friend class College;
};

/**
 * We make concept that checks whether given type is one of correct types for
 * our classes. We need this to implement find(pattern) method easily in one
 * body without is_same or explicit specializations.
 */
template <typename T>
bool constexpr is_type_academic() { return false; }

template <>
bool constexpr is_type_academic<Person>() { return true; }

template <>
bool constexpr is_type_academic<Student>() { return true; }

template <>
bool constexpr is_type_academic<Teacher>() { return true; }

template <>
bool constexpr is_type_academic<PhDStudent>() { return true; }

template <typename T>
concept IsAcademic = is_type_academic<T>();

template <typename T>
bool constexpr is_student_teacher() { return false; }

template <>
bool constexpr is_student_teacher<Student>() { return true; }

template <>
bool constexpr is_student_teacher<Teacher>() { return true; }

template <typename T>
concept StudentTeacher = is_student_teacher<T>();

class College
{
public:
    College() = default;

    /**
     * Function checks if course of given name is present in our college
     * (names of courses are unique), and if not it creates such 
     * course and adds it to set of courses and returns true.
     */
    bool add_course(const std::string &name, bool active = true)
    {
        if (course_names.find(name) == course_names.end())
        {
            auto iter_to_inserted_course = course_set.emplace(
                std::make_shared<Course>(name, active)).first;

            course_names.emplace(name, iter_to_inserted_course);

            return true;
        }
        return false;
    }

    /**
     * Function returns set of shared_ptrs to courses which names satisfy given
     * pattern. Courses in set are in lexycographic order by their names.
     * Function does not modify anything in our college.
     */
    auto find_courses(const std::string &pattern) const
    {
        // We need custom comparator for our set, since we want our courses
        // in lexycographic order given by their names.
        auto my_cmp = [](std::shared_ptr<Course> a, std::shared_ptr<Course> b)
        {
            return a->get_name() < b->get_name();
        };

        // We need to make a new set, cause we can get many different patterns
        // so each time we have to make new set of found elems and return it.
        std::set<std::shared_ptr<Course>, decltype(my_cmp)> matching_courses;

        // course_names = map<course name, iterator to course in course_set>
        for (auto iter = course_names.begin(); iter != course_names.end();
             iter++)
        {
            if (satisfies_pattern(iter->first, pattern))
                matching_courses.emplace(*(iter->second));
        }

        return matching_courses;
    }

    bool change_course_activeness(const std::shared_ptr<Course> &course,
                                  bool active) noexcept
    {
        auto iter = course_set.find(course);

        if (iter == course_set.end())
            return false;

        (*iter)->change_activeness(active);

        return true;
    }

    /**
     * Function removes given course from our college. Course is found by
     * comparing shared ptrs, not courses' names, since we can have two courses
     * with the same name, but in different colleges, so looking for course
     * using its name would be a mistake.
     */
    bool remove_course(const std::shared_ptr<Course> &course) noexcept
    {
        // Erase with iterator throws nothing, find() also throws nothing.
        auto iter = course_set.find(course);

        if (iter == course_set.end())
            return false;

        // Firstly we remove name of our course from set of courses names.
        // We need iterator so that it is noexcept.
        // We need iterator so that it is noexcept.
        auto iter_str = course_names.find((*iter)->get_name());
        course_names.erase(iter_str);

        // We change activeness and remove whole course from courses set.
        (*iter)->change_activeness(false);
        course_set.erase(iter);

        return true;
    }

    /**
     * Function add new person to college if person of given name and surname
     * isn't alread present in it. We need specializations since some
     * constructors need active parameter while others don't.
     */
    template <typename T>
    bool add_person(std::string name, std::string surname, bool active = true);

    /**
     * Function finds given student by comparing shared pointers in college 
     * set, since we can have students of same names but in different colleges.
     * Function is noexcept because even if dynamic_ptr_cast fails, target type
     * is not reference type, so there will be no exception thrown, only
     * nullptr returned.
     */
    bool change_student_activeness(const std::shared_ptr<Student> &student,
                                   bool active) noexcept
    {
        auto iter = person_set.find(student);

        if (iter == person_set.end())
            return false;

        std::dynamic_pointer_cast<Student>(*iter)->active = active;

        return true;
    }

    /**
     * Function finds people in college that name and surname have given
     * patterns and these people are of type T. We use concept IsAcademic here
     * since we need to ensure that type T has get_surname and get_name methods
     * in order not to make many similar outside of class specializations.
     */
    template <IsAcademic T>
    auto find(const std::string &name_pattern,
              const std::string &surname_pattern) const
    {
        // Custom lexicographical comparator for our result set. Sorted by
        // surname then name.

        auto name_cmp = [](std::shared_ptr<T> a, std::shared_ptr<T> b)
        {
            if (a->get_surname() != b->get_surname())
                return a->get_surname() < b->get_surname();
            else
                return a->get_name() < b->get_name();
        };

        // Result set.
        std::set<std::shared_ptr<T>, decltype(name_cmp)> matching_people;

        for (auto iter = person_set.begin(); iter != person_set.end();
             ++iter)
        {
            if (satisfies_pattern((*iter)->get_name(), name_pattern) &&
                satisfies_pattern((*iter)->get_surname(), surname_pattern))
            {
                auto found_person = std::dynamic_pointer_cast<T>(*iter);
                // We need to check if cast was successful meaning != nullptr,
                // cause we cannot cast i.e. teacher to student. If it was
                // successful it means that *iter type matches type T.
                if (found_person != nullptr)
                    matching_people.emplace(found_person);
            }
        }

        return matching_people;
    }

    template <typename T>
    auto find(const std::shared_ptr<Course>& course);

    /**
    * Functions assigns the given course to the given person, as long as both
    * the person and course are active (person must be active if 
    * it is a student), and both course and person exist in this 
    * college object. StudentTeacher concept ensures that this function
    * will only accept students or teachers (or classes derived from them).
    */
    template <StudentTeacher T>
    bool assign_course(const std::shared_ptr<T> &person,
                       const std::shared_ptr<Course> &course)
    {
        if (!person_set.contains(person))
            throw non_existing_person_exception();
        else if (!course_set.contains(course))
            throw non_existing_course_exception();
        if (!course->is_active())
            throw inactive_course_exception();
        Student *temp_student = dynamic_cast<Student *>(person.get());
        Teacher *temp_teacher = dynamic_cast<Teacher *>(person.get());
        if (temp_student != nullptr)
        {
            if (!temp_student->is_active())
                throw inactive_student_exception();
            if (temp_student->subjects_I_attend.contains(course))
                return false;
            else
            {
                temp_student->subjects_I_attend.emplace(course);
                temp_student->subjects_I_attend_const.emplace(
                    std::make_shared<const Course>(course->get_name(), 
                        course->is_active()));
                return true;
            }
        }
        else if (temp_teacher != nullptr)
        {
            if (temp_teacher->subjects_I_handle.contains(course))
                return false;
            else
            {
                temp_teacher->subjects_I_handle.emplace(course);
                temp_teacher->subjects_I_handle_const.emplace(
                    std::make_shared<const Course>(course->get_name(),
                        course->is_active()));
                return true;
            }
        }
        return false;
    }

private:
    // Person - identified by name and surname (they are unique)
    std::set<std::shared_ptr<Person>> person_set;

    // Set of names and surnames to quickly checking if person is in college.

    // Set of names and surnames to quickly checking if person is in college.
    std::set<std::pair<std::string, std::string>> people_names;

    // Course - identified by its name (name is unique)
    std::set<std::shared_ptr<Course>> course_set;

    // Map needed to quickly find course by its name and change sth in it.

    // Map needed to quickly find course by its name and change sth in it.
    std::map<std::string, std::set<std::shared_ptr<Course>>::iterator>
        course_names;

    // Exceptions for differents cases. Naming is self-explanatory.
    class inactive_student_exception : public std::exception
    {
        virtual const char* what() const throw()
        {
            return "Incorrect operation for an inactive student.";
        }
    };

    class inactive_course_exception : public std::exception
    {
        virtual const char* what() const throw()
        {
            return "Incorrect operation on an inactive course.";
        }
    };

    class non_existing_person_exception : public std::exception
    {
        virtual const char* what() const throw()
        {
            return "Non-existing person.";
        }
    };

    class non_existing_course_exception : public std::exception
    {
        virtual const char* what() const throw()
        {
            return "Non-existing course.";
        }
    };

    // Function checks whether given string satisfies pattern that has * and ?
    bool satisfies_pattern(const std::string &str,
                           const std::string &pattern) const noexcept
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
        while (ptrn_idx < ptrn_len)
        {
            if (pattern[ptrn_idx] != '*' && pattern[ptrn_idx] != '?')
                return false;
            ptrn_idx++;
        }

        // If there are no unchecked characters left in str we found pattern.
        if (str_idx == str_len)
            return true;

        return false;
    }
};

// Specializations:
// We need add_person specialization cause constructors may differ, and in some
// of them we need to add active, whereas in others we don't.
template <>
inline bool College::add_person<Student>(std::string name, 
    std::string surname, bool active)
{
    if (people_names.find(std::make_pair(name, surname)) == people_names.end())
    {
        people_names.emplace(std::make_pair(name, surname));
        person_set.emplace(std::make_shared<Student>(name, surname, active));
        return true;
    }
    return false;
}

template <>
inline bool College::add_person<Teacher>(std::string name, 
    std::string surname, bool active)
{
    active = true;
    if (people_names.find(std::make_pair(name, surname)) == people_names.end())
    {
        people_names.emplace(std::make_pair(name, surname));
        person_set.emplace(std::make_shared<Teacher>(name, surname));

        return active;
    }
    return false;
}

template <>
inline bool College::add_person<PhDStudent>(std::string name, 
    std::string surname, bool active)
{
    if (people_names.find(std::make_pair(name, surname)) == people_names.end())
    {
        people_names.emplace(std::make_pair(name, surname));
        person_set.emplace(std::make_shared<PhDStudent>(name, surname,
                                                        active));
        return true;
    }
    return false;
}

// We need assign_course specializations, because PhDStudent is both a teacher
// and a student, and we need to assign the given course to the correct
// data structure based on the specialized type.
template <>
inline bool College::assign_course<Student>(
    const std::shared_ptr<Student> &person, 
    const std::shared_ptr<Course> &course)
{
    if (!person_set.contains(person))
        throw non_existing_person_exception();
    else if (!course_set.contains(course))
        throw non_existing_course_exception();

    if (!course->is_active())
        throw inactive_course_exception();

    if (!person->is_active())
        throw inactive_student_exception();

    if (person->subjects_I_attend.contains(course))
        return false;
    else
    {
        person->subjects_I_attend.emplace(course);
        person->subjects_I_attend_const.emplace(
            std::make_shared<const Course>(course->get_name(), 
            course->is_active()));
        return true;
    }

    return false;
}

template <>
inline bool College::assign_course<Teacher>(
    const std::shared_ptr<Teacher> &person,
    const std::shared_ptr<Course> &course)
{
    if (!person_set.contains(person))
        throw non_existing_person_exception();
    else if (!course_set.contains(course))
        throw non_existing_course_exception();

    if (!course->is_active())
        throw inactive_course_exception();

    if (person->subjects_I_handle.contains(course))
        return false;
    else
    {
        person->subjects_I_handle.emplace(course);
        person->subjects_I_handle_const.emplace(
            std::make_shared<const Course>(course->get_name(), 
                course->is_active()));
        return true;
    }

    return false;
}

// We need find() specializations because PhDStudent is both a teacher
// and a student and we want to checkan appropriate data structure
// for the given course.
template <>
inline auto College::find<Student>(const std::shared_ptr<Course> &course)
{
    std::set<std::shared_ptr<Person>, Person::people_cmp> matching_people;
    for (auto iter = person_set.begin(); iter != person_set.end();
         ++iter)
    {
        Student *temp = dynamic_cast<Student *>(iter->get());
        if (temp != nullptr && temp->subjects_I_attend.contains(course))
        {
            for (auto iter2 = temp->subjects_I_attend.begin(); 
                iter2 != temp->subjects_I_attend.end();
                ++iter2)
            {
                if ((*iter2) == course)
                {
                    matching_people.emplace(*iter);
                    break;
                }
            }
        }
    }

    return matching_people;
}

template <>
inline auto College::find<Teacher>(const std::shared_ptr<Course> &course)
{
    std::set<std::shared_ptr<Person>, Person::people_cmp> matching_people;

    for (auto iter = person_set.begin(); iter != person_set.end(); ++iter)
    {
        Teacher *temp = dynamic_cast<Teacher *>(iter->get());
        if (temp != nullptr && temp->subjects_I_handle.contains(course))
        {
            for (auto iter2 = temp->subjects_I_handle.begin();
                iter2 != temp->subjects_I_handle.end();
                ++iter2)
            {
                if ((*iter2) == course)
                {
                    matching_people.emplace(*iter);
                    break;
                }
            }
        }
    }

    return matching_people;
}

#endif
