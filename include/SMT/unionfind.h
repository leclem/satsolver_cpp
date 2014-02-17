#ifndef SMT_UNIONFIND_H
#define SMT_UNIONFIND_H

#include<map>

template<class T>
class UnionFind
{
public:

    UnionFind(){}

    void Union(T t0, T t1)
    {
        if(t0>t1)
            Union(t1, t0);
        typename std::map<T, int>::iterator it0 = conv.find(t0);
        typename std::map<T, int>::iterator it1 = conv.find(t1);

        int r0, r1;

        if(it0 == conv.end())
            r0 = add(t0);
        else
            r0 = (*it0).second;
        if(it1 == conv.end())
            r1 = add(t1);
        else
            r1 = (*it1).second;

        repr[conv[repr[r0]]] = repr[r1];
    }

    T Find(T t)
    {
        typename std::map<T, int>::iterator it = conv.find(t);
        if(it == conv.end())
            return repr[add(t)];
        else
        {
            T tmp = repr[(*it).second];
            while(tmp != t)
            {
                t = tmp;
                tmp = repr[conv[repr[conv[tmp]]]];
            }
            repr[(*it).second] = t;
            return t;
        }
    }

    bool isIncluded(T t)
        { return conv.find(t) != conv.end(); }

private:

    int add(T t)
    {
        int r = conv[t] = repr.size();
        repr.push_back(t);
        return r;
    }
    // map of conversion between object and its place in the vector
    std::map<T, int> conv;

    // list of representant
    std::vector<T> repr;
};

#endif // SMT_UNIONFIND_H
