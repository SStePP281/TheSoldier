#ifndef QUEST
#define QUEST

#include "event_system.h"
#include "const.h"
#include <iostream>
#include <vector>

class Quest {
public:
    Quest(const QuestData& data);
    ~Quest() = default;
    void UpdateProgress(int value);
    const bool IsCompleted() const;
    const QuestData GetData() const;
private:
    QuestData data_;
};

class QuestManager {
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
    const int DeleteQuest(Quest* quest);
    const std::vector<Quest*>& GetQuests() const;
private:
    QuestManager();
    ~QuestManager();

    std::vector<Quest*> quests_;
};

#endif // !QUEST
