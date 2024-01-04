#ifndef COLLEGE_H
#define COLLEGE_H

#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <set>
#include <memory>
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

    bool equals(Course &c)
    {
        return this == &c;
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

    bool is_active() const
    {
        return active;
    }

    const std::vector<Course> &get_courses() const
    {
        // we need to sort our vec of subjects when we add them
        return subjects_I_attend;
    }

    friend class College;

protected:
    std::vector<Course> subjects_I_attend;
    bool active;
};

class Teacher : public virtual Person
{
public:
    Teacher() = delete;

    Teacher(std::string name, std::string surname) : Person(name, surname) {}

    const std::vector<Course> &get_courses() const
    {
        // we need to sort our vec of subjects when we add them
        return subjects_I_handle;
    }

protected:
    std::vector<Course> subjects_I_handle;
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

    bool add_course(const std::string &name, bool active = true)
    {
        if (course_names.find(name) == course_names.end())
        {
            auto iter_to_inserted_course = course_set.emplace(std::make_shared<Course>(name, active)).first;

            course_names.emplace(name, iter_to_inserted_course);
            std::cout << "added course: " << name << "\n";
            return true;
        }
        return false;
    }

    auto find_courses(const std::string &pattern) const
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
        for(auto iter = course_names.begin(); iter != course_names.end(); 
            iter++)
        {
            if(satisfies_pattern(iter->first, pattern))
            {
                matching_courses.emplace(*(iter->second));
            }
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

    bool remove_course(const std::shared_ptr<Course> &course) noexcept
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
                person_set.emplace(std::make_shared<Student>(name,
                                                             surname, active));
            else if constexpr (std::is_same<T, PhDStudent>::value)
                person_set.emplace(std::make_shared<PhDStudent>(name,
                                                                surname, active));
            else
                person_set.emplace(std::make_shared<Teacher>(name, surname));

            return true;
        }
        return false;
    }

    bool change_student_activeness(const std::shared_ptr<Student> &student,
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
    }

private:
    // Person - identified by name and surname (they are unique)
    std::set<std::shared_ptr<Person>> person_set;
    // std::set<const std::shared_ptr<Person>> person_const_set;
    std::set<std::pair<std::string, std::string>> people_names;

    // Course - identified by its name (name is unique)
    std::set<std::shared_ptr<Course>> course_set;
    // std::set<const std::shared_ptr<Course>> course_const_set;
    std::map<std::string, std::set<std::shared_ptr<Course>>::iterator>
        course_names;

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
};

#endif