#include<cctype>
#include<random>
#include<deque>

#ifndef GAMEUTIL_H
#define GAMEUTIL_H

namespace game{
    std::wstring wstoupper(const std::wstring& str);
    std::wstring wstolower(const std::wstring& str);
    class Log{
            std::deque<std::wstring> log;
        public:
            Log(){
                log.emplace_back(L"");
                log.emplace_back(L"");
                log.emplace_back(L"");
            }
            void add(std::wstring str){
                log.pop_front();
                log.push_back(str);
            }
            std::wstring operator[](size_t index){
                return log[index];
            }
    };

    int randint(int min, int max);

    // The implementation of a non-specialized template must be visible to a translation unit that uses it.
    // what the hell c++?
    // implementation of wstoupper() MUST be in .cpp, of choice() MUST be in .h
    // No one could possibly remember this
    // https://stackoverflow.com/questions/10632251/undefined-reference-to-template-function
    template<typename T>
    T choice(const std::vector<T>& list){
        srand ( time(NULL) );
        return list[std::rand()%list.size()];
    }

    extern Log log;
}

#endif