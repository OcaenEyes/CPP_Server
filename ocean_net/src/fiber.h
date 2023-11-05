/*
 * @Author: OCEAN.GZY
 * @Date: 2023-11-05 19:42:16
 * @LastEditors: OCEAN.GZY
 * @LastEditTime: 2023-11-05 20:16:09
 * @FilePath: /ocean_net/src/fiber.h
 * @Description: 注释信息
 */
#ifndef __OCEANSERVER_FIBER_H__
#define __OCEANSERVER_FIBER_H__

#include <memory>
#include <functional>
#include <ucontext.h>

namespace oceanserver
{
    class Scheduler;

    /**
     * @description: 协程类
     * @return {*}
     */
    class Fiber : public std::enable_shared_from_this<Fiber>
    {
        friend class Scheduler; // Scheduler是Fiber的友元类，类Scheduler的所有成员函数都是类Fiber的友元函数，能存取类Fiber的私有成员和保护成员
    public:
        typedef std::shared_ptr<Fiber> ptr;

        enum State // 协程状态
        {
            INIT,  // 初始
            HOLD,  // 暂停
            EXEC,  // 执行中
            TERM,  // 结束
            READY, // 可执行
            EXCEPT // 异常
        };

    private:
        /**
         * @description:  无参构造函数， 每个线程第一个协程的构造
         * @return {*}
         */
        Fiber();

    public:
        /**
         * @description: 构造函数
         * @param {function<void()>} cb 协程执行的函数
         * @param {size_t} stacksize 协程栈大小
         * @param {bool} use_caller 是否在MainFiber上调度
         * @return {*}
         */
        Fiber(std::function<void()> cb, size_t stacksize = 0, bool use_caller = false);
        ~Fiber();

        /**
         * @description: 重置协程执行函数，并设置状态
         * @param {function<void()>} cb
         * @return {*}
         */
        void reset(std::function<void()> cb);

        void swapIn(); // 将当前协程切换到运行状态

        void swapOut(); // 将当前协程切换到后台

        void call(); // 将当前线程切换到执行状态，执行当前线程的主协程

        void back(); // 将当前线程切换到后台，执行为该协程，返回线程的主协程

        uint64_t getId() const { return m_id; } // 返回协程id

        State getState() const { return m_state; } // 返回协程状态

    public:
        /**
         * @description: 设置当前线程的运行协程
         * @param {Fiber} *f  运行协程
         * @return {*}
         */
        static void SetThis(Fiber *f);

        static Fiber::ptr GetThis(); // 返回当前所在的协程

        /**
         * @description: 将当前协程切换到后台，并设置为READY状态
         * @return {*}
         */
        static void YieldToReady();

        /**
         * @description: 将当前协程切换到后台，并设置为HOLD状态
         * @return {*}
         */
        static void YieldToHold();

        static uint64_t TotalFibers(); // 返回当前协程的总数量

        static void MainFunc(); // 协程执行函数， 执行完成后返回线程主协程

        static void CallerMainFunc(); // 协程执行函数，执行完成返回到线程调度协程

        static uint64_t GetFiberId(); // 返回当前协程的id

    private:
        uint64_t m_id = 0;          // 协程id
        uint32_t m_stacksize = 0;   // 协程运行栈大小
        State m_state = INIT;       // 协程状态
        ucontext_t m_ctx;           // 协程上下文
        void *m_stack = nullptr;    // 协程运行栈指针
        std::function<void()> m_cb; // 协程运行函数
    };

} // namespace oceanserver

#endif