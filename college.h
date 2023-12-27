#ifndef COLLEGE_H
#define COLLEGE_H

#include <string>
#include <vector> 

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

class Student : public Person
{
    public:
        Student() = delete;
        Student(std::string name, std::string surname, bool is_active = true) :
        Person(name, surname), active(is_active) {}

    private:
        std::vector<Course> my_subjects_vec;
        bool active;
};

#endif