#ifndef COLLEGE_H
#define COLLEGE_H

#include <string>
#include <vector>
#include <memory>
#include <set>

using std::string;
using std::set;
using std::unique_ptr;
using std::make_unique;

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

    std::string get_name() const
    {
        return name;
    }

    std::string get_surname() const
    {
        return surname;
    }

private:
    std::string name;
    std::string surname;
};

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

private:
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

private:
    std::vector<Course> subjects_I_handle;
};

class PhDStudent : public Student, public Teacher
{
public:
    PhDStudent() = delete;
    PhDStudent(std::string name, std::string surname, bool is_active = true) :
        Person(name, surname), Student(name, surname, is_active), Teacher(name, surname)
    {}
};

class College
{
public:
    College(){}


    template <typename T>
    bool add_person(const string&, const string&, bool active = true);

    template <typename T>
    bool add_person(const string&, const string&);

private:
    set<string> used_names;
    set<unique_ptr<Person>> people;

    bool check_existence(const string&, const string&);

    template<typename T>
    concept Person_derived = requires(T a)
    {

    }
};

bool College::check_existence(const string& name, const string& surname)
{
    string concat = name + "_" + surname;
    if (used_names.find(concat) != used_names.end())
    {
        return true;
    }
    return false;
}

template <>
bool College::add_person<Student>(const string& name,
    const string& surname, bool active) {
    if (check_existence(name, surname))
    {
        return false;
    }
    else
    {
        people.insert(make_unique<Student>(name, surname, active));
        return true;
    }
    return false;
}

template <>
bool College::add_person<PhDStudent>(const string& name,
    const string& surname, bool active) {
    if (check_existence(name, surname))
    {
        return false;
    }
    else
    {
        people.insert(make_unique<PhDStudent>(name, surname, active));
        return true;
    }
    return false;
}

template <>
bool College::add_person<Teacher>(const string& name,
    const string& surname) {
    if (check_existence(name, surname))
    {
        return false;
    }
    else
    {
        people.insert(make_unique<Teacher>(name, surname));
        return true;
    }
    return false;
}

#endif