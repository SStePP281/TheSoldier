#include "Quest.h"
#include "DataBase.h"

Quest::Quest(const QuestData& data) : data{ data } {}

void Quest::UpdateProgress(int value) { data.progress = std::min(data.progress + value, data.target); }

bool Quest::IsCompleted() { return data.progress >= data.target; }

QuestManager::QuestManager()
{
    quests = std::vector<Quest*>(3);
    auto& data_base = Data::GetInstance();
    auto quest_data = data_base.GetQuest();

    for (size_t i = 0; i < quest_data.size(); i++)
    {
        AddQuest(quest_data[i]);
    }
}

QuestManager::~QuestManager()
{
    std::vector<QuestData> quest_data;
    for (auto q : quests)
    {
        if (q) { quest_data.push_back(q->data); }
    }

    auto& data_base = Data::GetInstance();
    data_base.SaveQuest(quest_data);
}

void QuestManager::AddQuest(const QuestData& data)
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

void QuestManager::UpdateQuests(QuestType type, int value)
{
    for (auto quest : quests) 
    {
        if (quest && quest->data.type == type)
        {
            quest->UpdateProgress(value);
        }
    }
}

void QuestManager::DeleteAllQuest()
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

int QuestManager::DeleteQuest(Quest* quest) 
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

