#include "quest.h"
#include "data_base.h"

Quest::Quest(const QuestData& data) : data_{ data } {}

void Quest::UpdateProgress(int value) { data_.progress = std::min(data_.progress + value, data_.target); }

const bool Quest::IsCompleted() const { return data_.progress >= data_.target; }

const QuestData Quest::GetData() const { return data_; }

QuestManager::QuestManager() {
    quests_ = std::vector<Quest*>(3);
    auto& data_base = Data::GetInstance();
    auto quest_data = data_base.GetQuest();

    for (size_t i = 0; i < quest_data.size(); i++) {
        AddQuest(quest_data[i]);
    }
}

QuestManager::~QuestManager() {
    std::vector<QuestData> quest_data;
    for (const auto& q : quests_) {
        if (q) { 
            quest_data.push_back(q->GetData());
        }
    }

    auto& data_base = Data::GetInstance();
    data_base.SaveQuest(quest_data);
}

void QuestManager::AddQuest(const QuestData& data) { 
    for (size_t i = 0; i < quests_.size(); i++) {
        if (!quests_[i]) {
            quests_[i] = new Quest(data);
            break;
        }
    }
}

void QuestManager::UpdateQuests(QuestType type, int value) {
    for (const auto& quest : quests_)  {
        if (quest && quest->GetData().type == type) {
            quest->UpdateProgress(value);
        }
    }
}

void QuestManager::DeleteAllQuest() {
    for (size_t i = 0; i < quests_.size(); i++) {
        if (quests_[i]) {
            delete quests_[i];
            quests_[i] = nullptr;
        }
    }
}

const int QuestManager::DeleteQuest(Quest* quest) {
    int reward = 0;
    for (size_t i = 0; i < quests_.size(); i++) {
        if (quests_[i] == quest) {
            reward = quest->GetData().reward_coins;
            delete quest;
            quests_[i] = nullptr;
            break;
        }
    }

    return reward;
}

const std::vector<Quest*>& QuestManager::GetQuests() const { return quests_; }
