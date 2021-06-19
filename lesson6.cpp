#include <iostream>
#include <mutex>
#include <thread>
#include <sstream>
#include <vector>
#include <future>
#include <queue>
#include <list>

using namespace std;

// Task 1. Создайте потокобезопасную оболочку для объекта cout. Назовите ее pcout. 
// Необходимо, чтобы несколько потоков могли обращаться к pcout и информация выводилась в консоль. Продемонстрируйте работу pcout

struct pcout : public std::stringstream { // обертка
    static inline std::mutex cout_mutex;
    ~pcout() {
        std::lock_guard<std::mutex> lg{ cout_mutex };
        std::cout << rdbuf();
        std::cout.flush();
    }
};

void print_pcout(int id) {
    pcout{} << "Pcout thread number " << id << "." << std::endl; // вызов обертки
}


// Task 2. Реализовать функцию, возвращающую i-ое простое число (например, миллионное простое число равно 15485863). 
// Вычисления реализовать во вторичном потоке. В консоли отображать прогресс вычисления.

bool isPrime(uint64_t number) {
    if (number < 2)
        return false;
    bool result = true;
    for (uint64_t i = 2; i <= sqrt(number); ++i) {
        if (number % i == 0)
        {
            result = false;
            break;
        }
    }
    return result;
}

uint64_t primeFind(uint32_t number, uint32_t& count) {
    uint32_t current_number = 0;
    while (count < number) {
        ++current_number;
        if (isPrime(current_number))
            ++count;
    }
    return current_number;
}

uint64_t primeFindTimer(uint32_t number) {
    uint32_t count = 0;
    std::future<uint64_t> prime_number = std::async(std::launch::async, primeFind, number, std::ref(count));
    while ((count * 100 / number) < 100) {
        std::cout << (count * 100 / number) << "%" << std::endl;
        std::this_thread::sleep_for(1s);

    }
    return prime_number.get();
}

// Task 3. mutexПромоделировать следующую ситуацию. Есть два человека (2 потока): хозяин и вор. Хозяин приносит домой вещи. При этом у каждой вещи есть своя ценность.
// Вор забирает вещи, каждый раз забирает вещь с наибольшей ценностью.

using item_type = pair<int, int>;
priority_queue<item_type> q;

mutex m_mutex;
bool finished = false;

list<item_type> thing;



void master(size_t items) {
    for (int i = 0; i < items; ++i) {
        this_thread::sleep_for(400ms);
        {
            lock_guard<mutex> lk(m_mutex);
            item_type x = make_pair<int, int>(rand() % 20000, rand());
            cout << "Put thing with # " << x.second << " and price " << x.first << endl;

            q.push(x);
        }
    }
    {
        lock_guard<mutex> lk(m_mutex);
        finished = true;
    }
}


void burglar() {
    this_thread::sleep_for(500ms);
    while (!q.empty())
    {
        this_thread::sleep_for(1s);
        unique_lock<mutex> lk(m_mutex);
        cout << "Got thing #" << q.top().first << "whith price " << q.top().first << endl;
        q.pop();
    }
}


int main()
{
    
    // Task 1.
    std::cout << "Task 1." << std::endl;
    std::vector<std::thread> task1;
    for (size_t i = 1; i < 10; ++i) {
        task1.emplace_back(print_pcout, i);
    }
    for (auto& t : task1) { t.join(); }
    std::cout << std::endl;
    std::cout << std::endl;


   // // Task 2.
    std::cout << "Task 2." << std::endl;

   
   auto prime_number = primeFindTimer(1000);
    std::cout << "Prime number is " << prime_number << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;


    // Task 3.
    std::cout << "Task 3." << std::endl;

   

    thread t1(master, 10);
    thread t2(burglar);

    t1.join();
    t2.join();
    cout << "finished!";
    std::cout << std::endl;
    std::cout << std::endl;

}

