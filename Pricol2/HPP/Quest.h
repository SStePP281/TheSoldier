#pragma once
#ifndef QUEST
#define QUEST

#include <iostream>
#include <vector>
#include "EventSystem.h"

enum QuestType
{
    KillMonster, CollectionMoney, CollectionDetails
};

struct QuestData
{
    QuestType type;
    int progress;
    int target;
    int rewardCoins;
};

class Quest 
{
public:
    Quest(QuestData& data);

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

    void addQuest(QuestData& data);

    void updateQuests(QuestType type, int value);

    int deleteQuest(Quest* quest);

    std::vector<Quest*> quests;
private:
    QuestManager();
};

#endif // !QUEST
