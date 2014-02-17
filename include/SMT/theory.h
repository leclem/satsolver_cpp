#ifndef SMT_THEORY_H
#define SMT_THEORY_H

#include <vector>
#include <list>
#include <string>
#include <iostream>

// class to define a fuction/variable in a theory
class CFunc
{
public:
    CFunc() {}
    int getArity()
        { return m_args.size();}
    std::vector<CFunc>& args()
        {return m_args; }
    std::string& getName()
        { return m_name; }
    bool operator == (CFunc f2) const
    {
        if(m_name != f2.getName())
            return false;
        for(int i = 0; i<(int)m_args.size(); i++)
            if(m_args[i] != f2.args()[i])
                return false;
        return true;
    }
    bool operator != (CFunc& f2) const
    {
        return !(*this == f2);
    }
    bool operator < (CFunc f2) const
    {
        if(m_name < f2.getName())
            return true;
        else if(m_name > f2.getName())
            return false;
        for(int i = 0; i<(int)m_args.size(); i++)
            if(m_args[i] < f2.args()[i])
                return true;
            else if(m_args[i] == f2.args()[i])
                continue;
            else
                return false;
        return false;
    }
    bool operator > (CFunc f2) const
    {
        return !(*this < f2) && !(*this == f2);
    }
private:
    std::string m_name;
    std::vector<CFunc> m_args;
};

inline std::ostream& operator << (std::ostream& out, CFunc f)
{
    out << f.getName();
    if(f.getArity()>0)
        out << '(';
    for(int i = 0; i<f.getArity(); i++)
        out << ((i>0)?", ":"") << f.args()[i];
    if(f.getArity()>0)
        out <<')';
    return out;
}


// class to define an equality/disequality
class CEqu
{
public:
    CEqu(){}

    CEqu(CFunc _f0, bool eq, CFunc _f1)
        : m_eq(eq), f0(_f0), f1(_f1) {}

    //return the name of the vars of each side
    std::string getLeftName()
        { return f0.getName(); }

    std::string getRightName()
        { return f1.getName(); }

    void getArgs(std::list<CFunc>& r)
    {
        r.clear();
        for(int i = 0; i<(int)f0.args().size(); i++)
            r.push_back(f0.args()[i]);
        for(int i = 0; i<(int)f1.args().size(); i++)
            r.push_back(f1.args()[i]);
    }

    CFunc& left()  { return f0;  }
    CFunc& right() { return f1;  }
    bool& isEqual(){ return m_eq;}

    CEqu operator !()
    {
        return CEqu(f0, !m_eq, f1);
    }

    bool operator == (CEqu& eq2)
    {
        if(m_eq != eq2.isEqual())
            return false;
        if(f0 == eq2.left())
            return f1 == eq2.right();
        else if(f1 == eq2.left())
            return f0 == eq2.right();
        else
            return false;
    }
    bool operator != (CEqu& eq2)
    {
        return !(*this == eq2);
    }
    bool operator < (CEqu e2) const
    {
        if(f0 < e2.right())
            return true;
        if(f0 > e2.right())
            return false;
        if(m_eq < e2.isEqual())
            return true;
        if(m_eq > e2.isEqual())
            return false;
        if(f1 < e2.left())
            return true;
        return false;
    }



private:
    bool m_eq; // true if equal, else false;
    CFunc f0, f1; // two parts
    std::string m_rep; // string representation
};

inline std::ostream& operator << (std::ostream& out, CEqu& eq)
{
    out << eq.left() << ((eq.isEqual())?" = " : " != ") << eq.right();
    return out;
}

class CTheoryParser
{
public:
    // ckeck if the equ has a good syntax
    static bool checkSyntax(std::string s)
    {
        int num_p = 0;
        bool look_var = true;
        bool after_space = true;
        bool eq = false;
        for(int i =0; i<(int)s.size(); i++)
        {
            char ch = s[i];


            if(((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '_'))
            {
                if(after_space && !look_var)
                    return false;
                look_var = false;
                after_space = false;
                continue;
            }

            if(ch == ' ' )
            {
                after_space = true;
                continue;
            }
            after_space = false;

            if(look_var)
                return false;

            switch(ch)
            {
            case ')':
                num_p--;
                if(num_p<0)
                    return false;
                break;
            case '(':
                num_p++;
            case ',':
                if(!num_p)
                    return false;
                look_var = true;
                break;
            case '!':
                if(i == (int)s.size()-1 || s[i+1] != '=')
                    return false;
            case '<':
                if(ch=='<' && (i == (int)s.size()-1 || s[i+1] != '>'))
                    return false;
                i++;
            case '=':
                if(eq)
                    return false;
                eq = true;
                look_var = true;
                break;
            default:
                return false;
            }
        }
        return !num_p && eq;
    }
private:
    // parse a func assuming its syntax is good
    static CFunc parse_f(std::string s)
    {
        CFunc r;
        std::string cur_arg="";
        int num_p = 0;
        for(int i = 0; i<(int)s.size(); i++)
        {
            if(s[i] != ' ' && !num_p)
            {
                r.getName().push_back(s[i]);
                continue;
            }
            if(s[i] == '(')
                num_p++;
            else if(s[i] == ')')
                num_p--;
            else if(s[i] == ',' && num_p == 1)
            {
                r.args().push_back(parse_f(cur_arg));
                cur_arg.clear();
            }
            else
                cur_arg.push_back(s[i]);
        }
        if(cur_arg != "")
            r.args().push_back(parse_f(cur_arg));
        return r;
    }
public:
    // parse the equ assuming its syntax is good
    static CEqu parse(std::string s)
    {
        CEqu r;
        std::string sf0, sf1;
        r.isEqual() = true;
        bool mid = false;
        for(int i = 0; i<(int)s.size(); i++)
        {
            if(s[i] == '=')
                mid = true;
            else if(s[i] == '!' || s[i] == '<')
            {
                mid = true;
                r.isEqual() = false;
                i++;
            }
            else
                ((mid)?sf0:sf1).push_back(s[i]);
        }

        r.left()  = parse_f(sf0);
        r.right() = parse_f(sf1);
        return r;
    }

private:


    CTheoryParser(){}
    ~CTheoryParser(){}
};

#endif // SMT_THEORY_H
