#ifndef P_REPORT_H
#define P_REPORT_H

#include "ElaScrollPage.h"
#include "../publicheader.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QProgressBar>
#include <QGroupBox>
#include <QTimer>
#include <QFrame>
#include <QTextEdit>
#include <QSplitter>
#include <QPainter>
#include <QStyleOption>

// 报告时间范围枚举
enum class ReportTimeRange {
    Daily,      // 日报告
    Weekly,     // 周报告
    Monthly,    // 月报告
    Yearly      // 年报告
};

// 统计数据结构
struct ReportStatistics {
    int totalTasks = 0;              // 总任务数
    int completedTasks = 0;          // 已完成任务数
    int continuousTasks = 0;         // 连续任务数
    double totalHours = 0.0;         // 总时长（小时）
    double averageTaskDuration = 0.0; // 平均任务时长
    QMap<QString, int> tagStats;     // 标签统计
    QMap<int, int> priorityStats;    // 优先级统计
    QMap<QDate, int> dailyStats;     // 每日任务统计
};

// 自定义统计卡片组件
class StatisticsCard : public QFrame
{
    Q_OBJECT
public:
    explicit StatisticsCard(const QString& title, const QString& value,
                            const QString& subtitle = "", QWidget* parent = nullptr);
    void updateValue(const QString& value, const QString& subtitle = "");
    void setProgress(int percentage);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QLabel* m_titleLabel;
    QLabel* m_valueLabel;
    QLabel* m_subtitleLabel;
    QProgressBar* m_progressBar;
    bool m_hasProgress;
};

class P_Report : public ElaScrollPage
{
    Q_OBJECT

public:
    explicit P_Report(QWidget* parent = nullptr);
    ~P_Report();

    // 更新任务数据
    void updateTasks(const QVector<Task>& tasks);

private slots:
    void onTimeRangeChanged();
    void onDateRangeChanged();
    void onRefreshClicked();
    void updateReportData();

private:
    // 初始化函数
    void initUI();
    void initTimeRangeSelector();
    void initStatisticsCards();
    void initDetailedTable();
    void initSummarySection();

    // 数据处理函数
    ReportStatistics calculateStatistics(const QVector<Task>& filteredTasks);
    QVector<Task> filterTasksByTimeRange(const QVector<Task>& tasks,
                                         ReportTimeRange range,
                                         const QDate& targetDate);
    double calculateTaskDuration(const Task& task);
    QString formatDuration(double hours);
    QString generateTextSummary(const ReportStatistics& stats);

    // UI更新函数
    void updateStatisticsCards();
    void updateDetailTable(const QVector<Task>& tasks);
    void updateSummaryText();

    // UI组件
    QWidget* m_centralWidget;
    QVBoxLayout* m_mainLayout;

    // 控制面板
    QGroupBox* m_controlPanel;
    QComboBox* m_timeRangeCombo;
    QDateEdit* m_startDateEdit;
    QDateEdit* m_endDateEdit;
    QPushButton* m_refreshButton;

    // 统计卡片
    QWidget* m_statisticsSection;
    QGridLayout* m_statsLayout;
    StatisticsCard* m_totalTasksCard;
    StatisticsCard* m_completionRateCard;
    StatisticsCard* m_totalTimeCard;
    StatisticsCard* m_averageTimeCard;

    // 汇总文本区域
    QGroupBox* m_summarySection;
    QTextEdit* m_summaryText;

    // 详细表格
    QGroupBox* m_detailSection;
    QTableWidget* m_detailTable;

    // 数据
    QVector<Task> m_allTasks;
    ReportStatistics m_currentStats;
    ReportTimeRange m_currentTimeRange;

    // 自动刷新定时器
    QTimer* m_refreshTimer;
};

#endif // P_REPORT_H
