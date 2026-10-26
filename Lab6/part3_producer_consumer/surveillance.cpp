#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <chrono>
#include <vector>

struct Frame
{
    int id;
    std::string camera;
    int complexity;      // складність аналізу кадру (1..3)
};

std::queue<Frame> frames;
std::mutex queueMutex;
std::mutex outputMutex;
std::condition_variable condition;
bool finished = false;

const int FRAME_COUNT = 12;

// Фіктивний аналіз кадру: визначає подію
std::string analyze(const Frame& frame)
{
    if (frame.id % 5 == 0) return "Unknown object detected";
    if (frame.id % 3 == 0) return "Motion detected";
    return "No events";
}

// Потік-виробник: генерує кадри з різних камер
void producer()
{
    const std::string cameras[] =
    {
        "Entrance",
        "Parking",
        "Warehouse",
        "Perimeter"
    };

    for (int i = 1; i <= FRAME_COUNT; ++i)
    {
        Frame frame
        {
            i,
            cameras[(i - 1) % 4],
            i % 3 + 1
        };

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            frames.push(frame);
        }

        {
            std::lock_guard<std::mutex> lock(outputMutex);
            std::cout << "[Producer] Created frame "
                      << frame.id << ", camera: "
                      << frame.camera << '\n';
        }

        condition.notify_one();

        std::this_thread::sleep_for(
            std::chrono::milliseconds(100));
    }

    {
        std::lock_guard<std::mutex> lock(queueMutex);
        finished = true;
    }
    condition.notify_all();

    {
        std::lock_guard<std::mutex> lock(outputMutex);
        std::cout << "[Producer] Generation finished\n";
    }
}

// Потік-споживач: аналізує кадри
void consumer(int consumerId)
{
    while (true)
    {
        Frame frame{};

        {
            std::unique_lock<std::mutex> lock(queueMutex);

            condition.wait(lock, []()
            {
                return !frames.empty() || finished;
            });

            if (frames.empty() && finished)
            {
                break;
            }

            frame = frames.front();
            frames.pop();
        }

        // Аналіз поза критичною секцією
        std::this_thread::sleep_for(
            std::chrono::milliseconds(frame.complexity * 150));

        std::string event = analyze(frame);

        {
            std::lock_guard<std::mutex> lock(outputMutex);
            std::cout << "[Consumer " << consumerId
                      << "] Analyzed frame " << frame.id
                      << ", camera: " << frame.camera
                      << ", complexity: " << frame.complexity
                      << ", event: " << event << '\n';
        }
    }

    {
        std::lock_guard<std::mutex> lock(outputMutex);
        std::cout << "[Consumer " << consumerId << "] Finished\n";
    }
}

int main()
{
    std::thread producerThread(producer);
    std::thread consumer1(consumer, 1);
    std::thread consumer2(consumer, 2);

    producerThread.join();
    consumer1.join();
    consumer2.join();

    std::cout << "\nAll frames have been processed.\n";
    return 0;
}