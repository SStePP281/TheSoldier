#include "Quest.h"
#include "DataBase.h"

Quest::Quest(const QuestData& data) : data{ data } {}

void Quest::updateProgress(int value) { data.progress = std::min(data.progress + value, data.target); }

bool Quest::isCompleted() { return data.progress >= data.target; }

QuestManager::QuestManager()
{
    quests = std::vector<Quest*>(3);
    auto& dataBase = Data::getInstance();
    auto questData = dataBase.getQuest();

    for (size_t i = 0; i < questData.size(); i++)
    {
        addQuest(questData[i]);
    }
}

QuestManager::~QuestManager()
{
    std::vector<QuestData> questData;
    for (auto q : quests)
    {
        if (q) { questData.push_back(q->data); }
    }

    auto& dataBase = Data::getInstance();
    dataBase.saveQuest(questData);
}

void QuestManager::addQuest(const QuestData& data)
{ 
    for (size_t i = 0; i < quests.size(); i++)
    {
        if (!quests[i])
        {
            quests[i] = new Quest(data);
            break;
        }
    }
}

void QuestManager::updateQuests(QuestType type, int value)
{
    for (auto quest : quests) 
    {
        if (quest && quest->data.type == type)
        {
            quest->updateProgress(value);
        }
    }
}

void QuestManager::deleteAllQuest()
{
    for (size_t i = 0; i < quests.size(); i++)
    {
        if (quests[i])
        {
            delete quests[i];
            quests[i] = nullptr;
        }
    }
}

int QuestManager::deleteQuest(Quest* quest) 
{
    int reward = 0;
    for (size_t i = 0; i < quests.size(); i++)
    {
        if (quests[i] == quest)
        {
            reward = quest->data.rewardCoins;
            delete quest;
            quests[i] = nullptr;
            break;
        }
    }

    return reward;
}

