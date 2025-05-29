#pragma once
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

    void updateProgress(int value);

    bool isCompleted();

    QuestData data;
};

class QuestManager
{
public:
    QuestManager(const QuestManager&) = delete;
    QuestManager& operator=(const QuestManager&) = delete;
    ~QuestManager();

    static QuestManager& getInstance()
    {
        static QuestManager instance;
        return instance;
    }

    void deleteAllQuest();

    void addQuest(const QuestData& data);

    void updateQuests(QuestType type, int value);

    int deleteQuest(Quest* quest);

    std::vector<Quest*> quests;
private:
    QuestManager();
};

#endif // !QUEST
