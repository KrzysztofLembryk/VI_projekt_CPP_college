#ifndef COLLEGE_H
#define COLLEGE_H

#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <memory>

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

    bool add_course(std::string name, bool active = true)
    {
        if (course_map.find(name) == course_map.end())
        {
            auto new_course = std::make_shared<Course>(name, active);
            course_map.emplace(name, new_course);
            return true;
        }
        return false;
    }

    auto find_courses(std::string pattern)
    {
    }

    bool change_course_activeness(const std::shared_ptr<Course> &course,
                                  bool active) noexcept
    {
        auto iter = course_map.find(course->get_name()); 

        if (iter == course_map.end())
            return false;

        // We can get course of the same name that is in our college, but its
        // a different course, so we change activeness only if adresses are the
        // same.
        
        if (course == iter->second)
        {
            course_map[course->get_name()]->change_activeness(active);
            return true;
        }
        std::cout << "Given course has the same name, but diff address\n";
        return false;
    }

private:
    // Person - identified by name and surname (they are unique)
    std::map<std::pair<std::string, std::string>, 
        std::shared_ptr<Person>> person_map;
    // Course - identified by its name (name is unique)
    std::map<std::string, std::shared_ptr<Course>> course_map;
};

#endif