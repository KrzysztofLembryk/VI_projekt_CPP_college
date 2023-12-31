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

// Klasa Person jest wirtualna, bo nie mozemy stworzyc obiektu po prostu typu
// osoba. Ale zapewniamy implementacje getterow, zeby nie duplikowac kodu.
class Person
{
public:
    Person() = delete;

    Person(std::string _name, std::string s_name) : name(_name),
                                                    surname(s_name) {}
    virtual ~Person() = default;

    virtual std::string get_name() const = 0;

    virtual std::string get_surname() const = 0;

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

class Student : virtual public Person
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

class Teacher : virtual public Person
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
        PhDStudent(std::string name, std::string surname, 
        bool is_active = true) : Person(name, surname),  {}
        //Person(name, surname){}

};

#endif