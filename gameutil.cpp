#include"gameutil.h"

namespace game{
    Log log = Log();

    std::wstring wstoupper(const std::wstring& str){
        std::wstring res(str);
        for(auto& c : res)
        {
            c = std::toupper(c);
        }
        return res;
    }
    std::wstring wstolower(const std::wstring& str){
        std::wstring res(str);
        for(auto& c : res)
        {
            c = std::toupper(c);
        }
        return res;
    }
    int randint(int min, int max){
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<int> dist(min,max);
        return dist(rng);
    }
}