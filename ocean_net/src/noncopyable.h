#ifndef __OCEANSERVER_NONCOPYABLE_H__
#define __OCEANSERVER_NONCOPYABLE_H__
namespace oceanserver
{
    class Noncopyable
    {
    public:
        Noncopyable() = default;  // 默认构造函数
        ~Noncopyable() = default; // 默认析构函数

        Noncopyable(const Noncopyable &) = delete; // 拷贝构造函数(禁用)

        Noncopyable &operator=(const Noncopyable &) = delete; // 赋值函数(禁用)
    };

} // namespace oceanserver

#endif