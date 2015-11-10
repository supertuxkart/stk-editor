#include <memory>
#include <utility>

template<class T>
void dropReferenceCounted(T* ref_counted)
{
    if (ref_counted)
        ref_counted->drop();
}

template<class T>
std::unique_ptr<T, decltype(&dropReferenceCounted<T>)> autoDropped(T* ref_counted)
{
    return std::unique_ptr<T, decltype(&dropReferenceCounted<T>)>(
        ref_counted, dropReferenceCounted);
}
