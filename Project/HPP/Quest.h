#ifndef QUEST
#define QUEST

#include "EventSystem.h"
#include "CONST.h"
#include <iostream>
#include <vector>

class Quest 
{
public:
    Quest(const QuestData& data);

    ~Quest() = default;

    void UpdateProgress(int value);

    bool IsCompleted();

    QuestData data;
};

class QuestManager
{
public:
    QuestManager(const QuestManager&) = delete;
    QuestManager& operator=(const QuestManager&) = delete;

    static QuestManager& GetInstance()
    {
        static QuestManager instance;
        return instance;
    }

    void DeleteAllQuest();

    void AddQuest(const QuestData& data);

    void UpdateQuests(QuestType type, int value);

    int DeleteQuest(Quest* quest);

    std::vector<Quest*> quests;
private:
    QuestManager();
    ~QuestManager();
};

#endif // !QUEST
