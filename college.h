#ifndef COLLEGE_H
#define COLLEGE_H

#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <set>
#include <memory>
#include <exception>
#include <regex>

class Course
{
public:
    Course() = delete;
    Course(std::string name, bool is_active = true) : course_name(name),
        active(is_active) {}

    std::string get_name() const
    {
        return course_name;
    }

    bool is_active() const
    {
        return active;
    }

    void change_activeness(bool new_val)
    {
        active = new_val;
    }

private:
    std::string course_name;
    bool active;
};

// Klasa Person jest wirtualna, bo nie mozemy stworzyc obiektu po prostu typu
// osoba. Ale zapewniamy implementacje getterow, zeby nie duplikowac kodu.
class Person
{
public:
    Person() = delete;

    Person(std::string _name, std::string s_name) : name(_name),
        surname(s_name) {}
    virtual ~Person() = default;

    virtual std::string get_name() const;

    virtual std::string get_surname() const;

protected:
    using course_const_sp = std::shared_ptr<const Course>;

    struct my_cmp
    {
        bool operator()(course_const_sp a, course_const_sp b)
        {
            return a->get_name() < b->get_name();
        }
    };

private:
    std::string name;
    std::string surname;
};

std::string Person::get_name() const
{
    return name;
}

std::string Person::get_surname() const
{
    return surname;
}

class Student : public virtual Person
{
public:
    Student() = delete;

    Student(std::string name, std::string surname, bool is_active = true) : Person(name, surname), active(is_active) {}

    ~Student() = default;

    bool is_active() const
    {
        return active;
    }

    const auto& get_courses() const
    {
        return subjects_I_attend;
    }

    friend class College;

protected:
    std::set<std::shared_ptr<const Course>, my_cmp> subjects_I_attend;
    bool active;
};

class Teacher : public virtual Person
{
public:
    Teacher() = delete;

    Teacher(std::string name, std::string surname) : Person(name, surname) {}

    ~Teacher() = default;

    const auto& get_courses() const
    {
        return subjects_I_handle;
    }

protected:
    std::set<std::shared_ptr<const Course>, my_cmp> subjects_I_handle;
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
};

class College
{
public:
    College() = default;

    bool add_course(const std::string& name, bool active = true)
    {
        if (course_names.find(name) == course_names.end())
        {
            auto iter_to_inserted_course = course_set.emplace(std::make_shared<Course>(name, active)).first;

            course_names.emplace(name, iter_to_inserted_course);
            // std::cout << "added course: " << name << "\n";
            return true;
        }
        return false;
    }

    auto find_courses(const std::string& pattern)
    {
        using course_const_sp = std::shared_ptr<const Course>;

        // We need custom comparator for our set, since we want our courses
        // in lexycographic order given by their names.
        auto my_cmp = [](course_const_sp a, course_const_sp b)
        {
            return a->get_name() < b->get_name();
        };

        // We need to make a new set, because we want to return shared pointers
        // that don't allow to modify our courses.
        std::set<course_const_sp, decltype(my_cmp)> matching_courses;

        // course_names = map<course name, iterator to course in course_set>
        for (auto iter = course_names.begin(); iter != course_names.end();
            iter++)
        {
            if (satisfies_pattern(iter->first, pattern))
            {
                matching_courses.emplace(*(iter->second));
            }
        }

        return matching_courses;
    }

    bool change_course_activeness(const std::shared_ptr<Course>& course,
        bool active) noexcept
    {
        auto iter = course_set.find(course);

        if (iter == course_set.end())
            return false;

        (*iter)->change_activeness(active);

        return true;
    }

    bool remove_course(const std::shared_ptr<Course>& course) noexcept
    {
        // Erase with iterator throws nothing, find() also throws nothing.
        auto iter = course_set.find(course);

        if (iter == course_set.end())
            return false;

        // Firstly we remove name of our course from set of courses names.
        auto iter_str = course_names.find((*iter)->get_name());
        course_names.erase(iter_str);

        // We change activeness and remove whole course from courses set.
        (*iter)->change_activeness(false);
        course_set.erase(iter);

        return true;
    }

    template <typename T>
    bool add_person(std::string name, std::string surname, bool active = true)
    {
        if (people_names.find(std::make_pair(name, surname)) ==
            people_names.end())
        {
            people_names.emplace(std::make_pair(name, surname));

            if constexpr (std::is_same<T, Student>::value)
                student_set.emplace(std::make_shared<const Student>(name,
                    surname, active));
            else if constexpr (std::is_same<T, PhDStudent>::value)
                phd_set.emplace(std::make_shared<const PhDStudent>(name,
                    surname, active));
            else
                teacher_set.emplace(std::make_shared<const Teacher>(name, surname));

            return true;
        }
        return false;
    }

    /*bool change_student_activeness(const std::shared_ptr<Student>& student,
                                   bool active) noexcept
    {
        auto iter = person_set.find(student);

        if (iter == person_set.end())
            return false;

        // std::string s_name =  (*iter)->get_name();
        // std::string s_surname = (*iter)->get_surname();
        // auto iter_name = people_names.find(std::make_pair(s_name, s_surname));

        std::dynamic_pointer_cast<Student>(*iter)->active = active;

        return true;
    }*/

    template <typename T>
    auto find(const std::string& name_pattern, const std::string& surname_pattern) const;
    

    template <typename T>
    bool assign_course(const std::shared_ptr<const T>& person,
        const std::shared_ptr<const Course>& course);

private:
    // Person - identified by name and surname (they are unique)
    std::set<std::shared_ptr<const Student>> student_set;
    std::set<std::shared_ptr<const Teacher>> teacher_set;
    std::set<std::shared_ptr<const PhDStudent>> phd_set;
    // std::set<const std::shared_ptr<Person>> person_const_set;
    std::set<std::pair<std::string, std::string>> people_names;

    // Course - identified by its name (name is unique)
    std::set<std::shared_ptr<Course>> course_set;
    // std::set<const std::shared_ptr<Course>> course_const_set;
    std::map<std::string, std::set<std::shared_ptr<Course>>::iterator>
        course_names;

    // Map of people and their courses.
    std::map<std::shared_ptr<const Person>, std::set<std::shared_ptr<const Course>>> student_courses;
    std::map<std::shared_ptr<const Person>, std::set<std::shared_ptr<const Course>>> teacher_courses;

    struct name_cmp {
        bool operator() (std::shared_ptr<const Person> a, std::shared_ptr<const Person> b) const
        {
            if (a->get_surname() != b->get_surname())
            {
                return a->get_surname() < b->get_surname();
            }
            else
            {
                return a->get_name() < b->get_name();
            }
        }
    };

    template<typename T >
    bool add_course_to_person(const std::shared_ptr<const Person>& person,
        const std::shared_ptr<const Course>& course);

    class generic_exception : public std::exception
    {
        virtual const char* what() const throw()
        {
            return "ugabuga";
        }
    };

    template<typename T>
    void find_people(std::set<std::shared_ptr<const T>, name_cmp>&, const std::string&, const std::string&) const;

    template<typename T>
    bool find_person(const std::shared_ptr<const Person>& person)
    {
        if (std::is_same_v<T, Student>)
        {
            for (auto iter = student_set.begin(); iter != student_set.end();
                ++iter)
            {
                if ((*iter) == person)
                {
                    return true;
                }
            }
        }
        else if (std::is_same_v<T, Teacher>)
        {
            for (auto iter = teacher_set.begin(); iter != teacher_set.end();
                ++iter)
            {
                if ((*iter) == person)
                {
                    return true;
                }
            }
        }
        for (auto iter = phd_set.begin(); iter != phd_set.end();
            ++iter)
        {
            if ((*iter) == person)
            {
                return true;
            }
        }

        return false;
    }

    bool find_course(const std::shared_ptr<const Course>& course)
    {
        for (auto iter = course_set.begin(); iter != course_set.end();
            ++iter)
        {
            // Address comparison (I think/hope).
            if ((*iter) == course)
            {
                return true;
            }
        }

        return false;
    }

    bool satisfies_pattern(const std::string& str,
        const std::string& pattern) const noexcept
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

template<>
inline bool College::add_course_to_person<Teacher>(
    const std::shared_ptr<const Person>& person,
    const std::shared_ptr<const Course>& course)
{
    if (teacher_courses.contains(person))
    {
        if (teacher_courses[person].contains(course))
        {
            return false;
        }
        else
        {
            teacher_courses[person].insert(course);
        }
    }
    else
    {
        teacher_courses[person] =
            std::set<std::shared_ptr<const Course>>{ course };
    }
    return true;
}

template<>
inline void College::find_people<Person>(std::set<std::shared_ptr<const Person>, name_cmp>& people,
    const std::string& name_pattern, const std::string& surname_pattern) const
{
    for (auto iter = student_set.begin(); iter != student_set.end();
        ++iter)
    {
        if (satisfies_pattern((*iter)->get_name(), name_pattern) &&
            satisfies_pattern((*iter)->get_surname(), surname_pattern)) {
            people.emplace(*iter);
        }
    }
    for (auto iter = teacher_set.begin(); iter != teacher_set.end();
        ++iter)
    {
        if (satisfies_pattern((*iter)->get_name(), name_pattern) &&
            satisfies_pattern((*iter)->get_surname(), surname_pattern)) {
            people.emplace(*iter);
        }
    }
    for (auto iter = phd_set.begin(); iter != phd_set.end();
        ++iter)
    {
        if (satisfies_pattern((*iter)->get_name(), name_pattern) &&
            satisfies_pattern((*iter)->get_surname(), surname_pattern)) {
            people.emplace(*iter);
        }
    }
}

template<>
inline void College::find_people<Student>(std::set<std::shared_ptr<const Student>, name_cmp>& people,
    const std::string& name_pattern, const std::string& surname_pattern) const
{
    for (auto iter = student_set.begin(); iter != student_set.end();
        ++iter)
    {
        if (satisfies_pattern((*iter)->get_name(), name_pattern) &&
            satisfies_pattern((*iter)->get_surname(), surname_pattern)) {
            people.emplace(*iter);
        }
    }
    for (auto iter = phd_set.begin(); iter != phd_set.end();
        ++iter)
    {
        if (satisfies_pattern((*iter)->get_name(), name_pattern) &&
            satisfies_pattern((*iter)->get_surname(), surname_pattern)) {
            people.emplace(*iter);
        }
    }
}

template<>
inline void College::find_people<Teacher>(std::set<std::shared_ptr<const Teacher>, name_cmp>& people,
    const std::string& name_pattern, const std::string& surname_pattern) const
{
    for (auto iter = teacher_set.begin(); iter != teacher_set.end();
        ++iter)
    {
        if (satisfies_pattern((*iter)->get_name(), name_pattern) &&
            satisfies_pattern((*iter)->get_surname(), surname_pattern)) {
            people.emplace(*iter);
        }
    }
    for (auto iter = phd_set.begin(); iter != phd_set.end();
        ++iter)
    {
        if (satisfies_pattern((*iter)->get_name(), name_pattern) &&
            satisfies_pattern((*iter)->get_surname(), surname_pattern)) {
            people.emplace(*iter);
        }
    }
}

template<>
inline void College::find_people<PhDStudent>(std::set<std::shared_ptr<const PhDStudent>, name_cmp>& people,
    const std::string& name_pattern, const std::string& surname_pattern) const
{
    for (auto iter = phd_set.begin(); iter != phd_set.end();
        ++iter)
    {
        if (satisfies_pattern((*iter)->get_name(), name_pattern) &&
            satisfies_pattern((*iter)->get_surname(), surname_pattern)) {
            people.emplace(*iter);
        }
    }
}

template<>
inline bool College::add_course_to_person<Student>(
    const std::shared_ptr<const Person>& person,
    const std::shared_ptr<const Course>& course)
{
    if (student_courses.contains(person))
    {
        if (student_courses[person].contains(course))
        {
            return false;
        }
        else
        {
            student_courses[person].insert(course);
        }
    }
    else
    {
        student_courses[person] =
            std::set<std::shared_ptr<const Course>>{ course };
    }
    return true;
}

template<>
inline bool College::assign_course<Student>(const std::shared_ptr<const Student>& person,
    const std::shared_ptr<const Course>& course)
{
    if (!find_person<Student>(person) || !find_course(course))
    {
        throw generic_exception();
    }
    const Student* temp_student = dynamic_cast<const Student*>(person.get());
    if (temp_student == nullptr)
    {
        throw generic_exception();
    }
    else if (!temp_student->is_active())
    {
        throw generic_exception();
    }
    return add_course_to_person<Student>(person, course);
}

template<>
inline bool College::assign_course<Teacher>(const std::shared_ptr<const Teacher>& person,
    const std::shared_ptr<const Course>& course)
{
    if (!find_person<Teacher>(person) || !find_course(course))
    {
        throw generic_exception();
    }
    const Teacher* temp_teacher = dynamic_cast<const Teacher*>(person.get());
    if (temp_teacher == nullptr)
    {
        throw generic_exception();
    }
    return add_course_to_person<Teacher>(person, course);
}

template<typename T>
inline bool College::assign_course(const std::shared_ptr<const T>& person,
    const std::shared_ptr<const Course>& course)
{
    if (std::is_same_v<T, Student>)
    {
        if (!find_person<Student>(person) || !find_course(course))
        {
            throw generic_exception();
        }
        const Student* temp_student = dynamic_cast<const Student*>(person.get());
        if (temp_student == nullptr)
        {
            throw generic_exception();
        }
        else if (!temp_student->is_active())
        {
            throw generic_exception();
        }
        return add_course_to_person<Student>(person, course);
    }
    else if (std::is_same_v<T, Teacher>)
    {
        if (!find_person<Teacher>(person) || !find_course(course))
        {
            throw generic_exception();
        }
        const Teacher* temp_teacher = dynamic_cast<const Teacher*>(person.get());
        if (temp_teacher == nullptr)
        {
            throw generic_exception();
        }
        return add_course_to_person<Teacher>(person, course);
    }
    else
    {
        throw generic_exception();
    }
    return true;
}

template<>
inline auto College::find<Person>(const std::string& name_pattern, const std::string& surname_pattern) const
{
    // Result set.
    std::set<std::shared_ptr<const Person>, name_cmp> matching_people;
    find_people<Person>(matching_people, name_pattern, surname_pattern);
    //find_people<Student>(matching_people, name_pattern, surname_pattern);
    //find_people<Teacher>(matching_people, name_pattern, surname_pattern);
    //find_people<PhDStudent>(matching_people, name_pattern, surname_pattern);

    return matching_people;
}

template<>
inline auto College::find<Student>(const std::string& name_pattern, const std::string& surname_pattern) const
{
    std::set<std::shared_ptr<const Student>, name_cmp> matching_people;
    find_people<Student>(matching_people, name_pattern, surname_pattern);
    //find_people<PhDStudent>(matching_people, name_pattern, surname_pattern);
    return matching_people;
}

template<>
inline auto College::find<Teacher>(const std::string& name_pattern, const std::string& surname_pattern) const
{
    std::set<std::shared_ptr<const Teacher>, name_cmp> matching_people;
    find_people<Teacher>(matching_people, name_pattern, surname_pattern);
    //find_people<PhDStudent>(matching_people, name_pattern, surname_pattern);
    return matching_people;
}

template<>
inline auto College::find<PhDStudent>(const std::string& name_pattern, const std::string& surname_pattern) const
{
    std::set<std::shared_ptr<const PhDStudent>, name_cmp> matching_people;
    find_people<PhDStudent>(matching_people, name_pattern, surname_pattern);
    return matching_people;
}

#endif