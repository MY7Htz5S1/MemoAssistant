#include "p_report.h"
#include <QApplication>
#include <QHeaderView>
#include <QSplitter>
#include <QMessageBox>
#include <QDebug>
#include <QPainterPath>
#include <algorithm>
#include <cmath>

// StatisticsCard 实现
StatisticsCard::StatisticsCard(const QString& title, const QString& value,
                               const QString& subtitle, QWidget* parent)
    : QFrame(parent), m_hasProgress(false)
{
    setFrameStyle(QFrame::Box | QFrame::Raised);
    setLineWidth(1);
    setMinimumSize(200, 120);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // 深色主题样式
    setStyleSheet(
        "StatisticsCard {"
        "    background-color: #2b2b2b;"
        "    border: 1px solid #404040;"
        "    border-radius: 8px;"
        "}"
        );

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(15, 10, 15, 10);

    m_titleLabel = new QLabel(title, this);
    m_titleLabel->setStyleSheet("QLabel { color: #b0b0b0; font-size: 12px; }");

    m_valueLabel = new QLabel(value, this);
    m_valueLabel->setStyleSheet("QLabel { color: #ffffff; font-size: 24px; font-weight: bold; }");

    m_subtitleLabel = new QLabel(subtitle, this);
    m_subtitleLabel->setStyleSheet("QLabel { color: #b0b0b0; font-size: 10px; }");

    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
    m_progressBar->setMaximum(100);
    m_progressBar->setTextVisible(false);
    m_progressBar->setFixedHeight(6);
    m_progressBar->setStyleSheet(
        "QProgressBar {"
        "    border: 1px solid #404040;"
        "    border-radius: 3px;"
        "    background-color: #1a1a1a;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: #0078d4;"
        "    border-radius: 2px;"
        "}"
        );

    layout->addWidget(m_titleLabel);
    layout->addWidget(m_valueLabel);
    layout->addWidget(m_subtitleLabel);
    layout->addWidget(m_progressBar);
    layout->addStretch();
}

void StatisticsCard::updateValue(const QString& value, const QString& subtitle)
{
    m_valueLabel->setText(value);
    m_subtitleLabel->setText(subtitle);
}

void StatisticsCard::setProgress(int percentage)
{
    m_hasProgress = true;
    m_progressBar->setVisible(true);
    m_progressBar->setValue(percentage);
}

void StatisticsCard::paintEvent(QPaintEvent* event)
{
    QFrame::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制深色圆角背景
    QPainterPath path;
    path.addRoundedRect(rect().adjusted(1, 1, -1, -1), 8, 8);
    painter.fillPath(path, QColor(43, 43, 43)); // 深色背景
}

// P_Report 实现
P_Report::P_Report(QWidget* parent)
    : ElaScrollPage(parent)
    , m_centralWidget(nullptr)
    , m_mainLayout(nullptr)
    , m_currentTimeRange(ReportTimeRange::Monthly)
    , m_refreshTimer(nullptr)
{
    initUI();

    // 设置自动刷新定时器（每5分钟刷新一次）
    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout, this, &P_Report::updateReportData);
    m_refreshTimer->start(300000); // 5分钟

    // 初始化默认数据
    updateReportData();
}

P_Report::~P_Report()
{
    if (m_refreshTimer) {
        m_refreshTimer->stop();
    }
}

void P_Report::initUI()
{
    m_centralWidget = new QWidget(this);
    m_centralWidget->setStyleSheet(
        "QWidget {"
        "    background-color: #1a1a1a;"
        "    color: #ffffff;"
        "}"
        "QGroupBox {"
        "    color: #ffffff;"
        "    border: 1px solid #404040;"
        "    border-radius: 8px;"
        "    margin-top: 10px;"
        "    padding-top: 10px;"
        "    background-color: #262626;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    left: 10px;"
        "    padding: 0 5px 0 5px;"
        "    background-color: transparent;"
        "    color: #ffffff;"
        "}"
        "QLabel {"
        "    color: #ffffff;"
        "}"
        "QComboBox {"
        "    background-color: #2b2b2b;"
        "    border: 1px solid #404040;"
        "    border-radius: 4px;"
        "    padding: 5px;"
        "    color: #ffffff;"
        "}"
        "QComboBox::drop-down {"
        "    border: none;"
        "}"
        "QDateEdit {"
        "    background-color: #2b2b2b;"
        "    border: 1px solid #404040;"
        "    border-radius: 4px;"
        "    padding: 5px;"
        "    color: #ffffff;"
        "}"
        "QPushButton {"
        "    background-color: #0078d4;"
        "    border: none;"
        "    border-radius: 4px;"
        "    padding: 8px 16px;"
        "    color: #ffffff;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #106ebe;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #005a9e;"
        "}"
        );

    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setSpacing(20);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);

    // 初始化各个部分
    initTimeRangeSelector();
    initStatisticsCards();
    initSummarySection();
    initDetailedTable();

    addCentralWidget(m_centralWidget, true, false, 0);
}

void P_Report::initTimeRangeSelector()
{
    m_controlPanel = new QGroupBox("报告设置", this);
    QHBoxLayout* controlLayout = new QHBoxLayout(m_controlPanel);

    // 时间范围选择
    controlLayout->addWidget(new QLabel("时间范围:"));
    m_timeRangeCombo = new QComboBox(this);
    m_timeRangeCombo->addItem("日报告", static_cast<int>(ReportTimeRange::Daily));
    m_timeRangeCombo->addItem("周报告", static_cast<int>(ReportTimeRange::Weekly));
    m_timeRangeCombo->addItem("月报告", static_cast<int>(ReportTimeRange::Monthly));
    m_timeRangeCombo->addItem("年报告", static_cast<int>(ReportTimeRange::Yearly));
    m_timeRangeCombo->setCurrentIndex(2); // 默认月报告
    connect(m_timeRangeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &P_Report::onTimeRangeChanged);
    controlLayout->addWidget(m_timeRangeCombo);

    controlLayout->addSpacing(20);

    // 日期选择
    controlLayout->addWidget(new QLabel("开始日期:"));
    m_startDateEdit = new QDateEdit(QDate::currentDate().addMonths(-1), this);
    m_startDateEdit->setCalendarPopup(true);
    connect(m_startDateEdit, &QDateEdit::dateChanged, this, &P_Report::onDateRangeChanged);
    controlLayout->addWidget(m_startDateEdit);

    controlLayout->addWidget(new QLabel("结束日期:"));
    m_endDateEdit = new QDateEdit(QDate::currentDate(), this);
    m_endDateEdit->setCalendarPopup(true);
    connect(m_endDateEdit, &QDateEdit::dateChanged, this, &P_Report::onDateRangeChanged);
    controlLayout->addWidget(m_endDateEdit);

    controlLayout->addSpacing(20);

    // 刷新按钮
    m_refreshButton = new QPushButton("刷新报告", this);
    connect(m_refreshButton, &QPushButton::clicked, this, &P_Report::onRefreshClicked);
    controlLayout->addWidget(m_refreshButton);

    controlLayout->addStretch();
    m_mainLayout->addWidget(m_controlPanel);
}

void P_Report::initStatisticsCards()
{
    m_statisticsSection = new QWidget(this);
    m_statsLayout = new QGridLayout(m_statisticsSection);
    m_statsLayout->setSpacing(15);

    // 创建统计卡片
    m_totalTasksCard = new StatisticsCard("总任务数", "0", "", this);
    m_completionRateCard = new StatisticsCard("完成率", "0%", "", this);
    m_totalTimeCard = new StatisticsCard("总时长", "0小时", "", this);
    m_averageTimeCard = new StatisticsCard("平均时长", "0小时", "", this);

    // 设置完成率卡片显示进度条
    m_completionRateCard->setProgress(0);

    // 响应式布局：在较小屏幕上使用2x2布局，在大屏幕上使用1x4布局
    m_statsLayout->addWidget(m_totalTasksCard, 0, 0);
    m_statsLayout->addWidget(m_completionRateCard, 0, 1);
    m_statsLayout->addWidget(m_totalTimeCard, 0, 2);
    m_statsLayout->addWidget(m_averageTimeCard, 0, 3);

    // 如果屏幕宽度不够，自动换行
    m_statsLayout->setColumnStretch(0, 1);
    m_statsLayout->setColumnStretch(1, 1);
    m_statsLayout->setColumnStretch(2, 1);
    m_statsLayout->setColumnStretch(3, 1);

    m_mainLayout->addWidget(m_statisticsSection);
}

void P_Report::initSummarySection()
{
    m_summarySection = new QGroupBox("智能分析总结", this);
    QVBoxLayout* summaryLayout = new QVBoxLayout(m_summarySection);

    m_summaryText = new QTextEdit(this);
    m_summaryText->setReadOnly(true);
    m_summaryText->setMaximumHeight(150);
    m_summaryText->setStyleSheet(
        "QTextEdit {"
        "    background-color: #2b2b2b;"
        "    border: 1px solid #404040;"
        "    border-radius: 6px;"
        "    padding: 10px;"
        "    font-size: 13px;"
        "    line-height: 1.5;"
        "    color: #ffffff;"
        "}"
        );

    summaryLayout->addWidget(m_summaryText);
    m_mainLayout->addWidget(m_summarySection);
}

void P_Report::initDetailedTable()
{
    m_detailSection = new QGroupBox("详细数据", this);
    QVBoxLayout* detailLayout = new QVBoxLayout(m_detailSection);

    m_detailTable = new QTableWidget(this);
    m_detailTable->setColumnCount(7);
    QStringList headers = {"任务名称", "类型", "开始时间", "结束时间", "时长", "优先级", "标签"};
    m_detailTable->setHorizontalHeaderLabels(headers);

    // 设置表格属性和深色主题
    m_detailTable->horizontalHeader()->setStretchLastSection(true);
    m_detailTable->setAlternatingRowColors(true);
    m_detailTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_detailTable->setSortingEnabled(true);
    m_detailTable->setMaximumHeight(300);

    // 深色主题样式
    m_detailTable->setStyleSheet(
        "QTableWidget {"
        "    background-color: #2b2b2b;"
        "    alternate-background-color: #363636;"
        "    color: #ffffff;"
        "    gridline-color: #404040;"
        "    border: 1px solid #404040;"
        "    border-radius: 6px;"
        "}"
        "QTableWidget::item {"
        "    padding: 8px;"
        "    border: none;"
        "}"
        "QTableWidget::item:selected {"
        "    background-color: #0078d4;"
        "}"
        "QHeaderView::section {"
        "    background-color: #363636;"
        "    color: #ffffff;"
        "    padding: 8px;"
        "    border: 1px solid #404040;"
        "    font-weight: bold;"
        "}"
        "QHeaderView::section:horizontal {"
        "    border-top: none;"
        "}"
        "QHeaderView::section:vertical {"
        "    border-left: none;"
        "}"
        );

    // 设置列宽
    m_detailTable->setColumnWidth(0, 200); // 任务名称
    m_detailTable->setColumnWidth(1, 100); // 类型
    m_detailTable->setColumnWidth(2, 150); // 开始时间
    m_detailTable->setColumnWidth(3, 150); // 结束时间
    m_detailTable->setColumnWidth(4, 80);  // 时长
    m_detailTable->setColumnWidth(5, 80);  // 优先级

    detailLayout->addWidget(m_detailTable);
    m_mainLayout->addWidget(m_detailSection);
}

void P_Report::updateTasks(const QVector<Task>& tasks)
{
    m_allTasks = tasks;
    updateReportData();
}

void P_Report::onTimeRangeChanged()
{
    int index = m_timeRangeCombo->currentIndex();
    m_currentTimeRange = static_cast<ReportTimeRange>(m_timeRangeCombo->itemData(index).toInt());

    // 根据时间范围调整日期选择器
    QDate currentDate = QDate::currentDate();
    switch (m_currentTimeRange) {
    case ReportTimeRange::Daily:
        m_startDateEdit->setDate(currentDate);
        m_endDateEdit->setDate(currentDate);
        break;
    case ReportTimeRange::Weekly:
        m_startDateEdit->setDate(currentDate.addDays(-7));
        m_endDateEdit->setDate(currentDate);
        break;
    case ReportTimeRange::Monthly:
        m_startDateEdit->setDate(currentDate.addMonths(-1));
        m_endDateEdit->setDate(currentDate);
        break;
    case ReportTimeRange::Yearly:
        m_startDateEdit->setDate(currentDate.addYears(-1));
        m_endDateEdit->setDate(currentDate);
        break;
    }

    updateReportData();
}

void P_Report::onDateRangeChanged()
{
    updateReportData();
}

void P_Report::onRefreshClicked()
{
    updateReportData();
}

void P_Report::updateReportData()
{
    // 过滤任务数据
    QVector<Task> filteredTasks = filterTasksByTimeRange(m_allTasks, m_currentTimeRange, m_startDateEdit->date());

    // 计算统计数据
    m_currentStats = calculateStatistics(filteredTasks);

    // 更新UI
    updateStatisticsCards();
    updateSummaryText();
    updateDetailTable(filteredTasks);
}

void P_Report::updateStatisticsCards()
{
    // 更新总任务数
    m_totalTasksCard->updateValue(QString::number(m_currentStats.totalTasks));

    // 更新完成率
    double completionRate = m_currentStats.totalTasks > 0 ?
                                (double)m_currentStats.completedTasks / m_currentStats.totalTasks * 100 : 0;
    m_completionRateCard->updateValue(QString::number(completionRate, 'f', 1) + "%");
    m_completionRateCard->setProgress(static_cast<int>(completionRate));

    // 更新总时长
    m_totalTimeCard->updateValue(formatDuration(m_currentStats.totalHours));

    // 更新平均时长
    m_averageTimeCard->updateValue(formatDuration(m_currentStats.averageTaskDuration));
}

void P_Report::updateSummaryText()
{
    QString summary = generateTextSummary(m_currentStats);
    m_summaryText->setHtml(summary);
}

void P_Report::updateDetailTable(const QVector<Task>& tasks)
{
    m_detailTable->setRowCount(tasks.size());

    for (int i = 0; i < tasks.size(); ++i) {
        const Task& task = tasks[i];

        m_detailTable->setItem(i, 0, new QTableWidgetItem(task.taskName));
        m_detailTable->setItem(i, 1, new QTableWidgetItem(task.isContinuous ? "连续任务" : "单次任务"));
        m_detailTable->setItem(i, 2, new QTableWidgetItem(task.startTime.toString("yyyy-MM-dd hh:mm")));

        QString endTimeStr = task.stopTime.isValid() ?
                                 task.stopTime.toString("yyyy-MM-dd hh:mm") : "进行中";
        m_detailTable->setItem(i, 3, new QTableWidgetItem(endTimeStr));

        double duration = calculateTaskDuration(task);
        m_detailTable->setItem(i, 4, new QTableWidgetItem(formatDuration(duration)));
        m_detailTable->setItem(i, 5, new QTableWidgetItem(QString::number(task.priority)));
        m_detailTable->setItem(i, 6, new QTableWidgetItem(task.tags.join(", ")));
    }
}

QVector<Task> P_Report::filterTasksByTimeRange(const QVector<Task>& tasks,
                                               ReportTimeRange range,
                                               const QDate& targetDate)
{
    QVector<Task> filtered;
    QDate startDate = m_startDateEdit->date();
    QDate endDate = m_endDateEdit->date();

    for (const Task& task : tasks) {
        QDate taskDate = task.startTime.date();
        if (taskDate >= startDate && taskDate <= endDate) {
            filtered.append(task);
        }
    }

    return filtered;
}

ReportStatistics P_Report::calculateStatistics(const QVector<Task>& filteredTasks)
{
    ReportStatistics stats;
    stats.totalTasks = filteredTasks.size();

    for (const Task& task : filteredTasks) {
        // 计算完成任务数（有结束时间的任务）
        if (task.stopTime.isValid()) {
            stats.completedTasks++;
        }

        // 计算连续任务数
        if (task.isContinuous) {
            stats.continuousTasks++;
        }

        // 计算总时长
        double duration = calculateTaskDuration(task);
        stats.totalHours += duration;

        // 统计标签
        for (const QString& tag : task.tags) {
            if (!tag.isEmpty()) {
                stats.tagStats[tag]++;
            }
        }

        // 统计优先级
        stats.priorityStats[task.priority]++;

        // 统计每日任务数
        QDate taskDate = task.startTime.date();
        stats.dailyStats[taskDate]++;
    }

    // 计算平均任务时长
    stats.averageTaskDuration = stats.completedTasks > 0 ?
                                    stats.totalHours / stats.completedTasks : 0;

    return stats;
}

double P_Report::calculateTaskDuration(const Task& task)
{
    if (!task.stopTime.isValid()) {
        // 如果任务还在进行中，计算到当前时间
        return task.startTime.secsTo(QDateTime::currentDateTime()) / 3600.0;
    }
    return task.startTime.secsTo(task.stopTime) / 3600.0;
}

QString P_Report::formatDuration(double hours)
{
    if (hours < 1.0) {
        return QString::number(hours * 60, 'f', 0) + "分钟";
    }
    return QString::number(hours, 'f', 1) + "小时";
}

QString P_Report::generateTextSummary(const ReportStatistics& stats)
{
    QString summary = "<div style='font-family: Microsoft YaHei; line-height: 1.6; color: #ffffff;'>";

    // 基本统计
    summary += QString("<p><b style='color: #4fc3f7;'>📊 统计概览</b><br>");
    summary += QString("在选定时间范围内，您共有 <b style='color: #81c784;'>%1</b> 个任务，其中 <b style='color: #81c784;'>%2</b> 个已完成，完成率为 <b style='color: #ffb74d;'>%3%</b>。")
                   .arg(stats.totalTasks)
                   .arg(stats.completedTasks)
                   .arg(stats.totalTasks > 0 ? QString::number((double)stats.completedTasks / stats.totalTasks * 100, 'f', 1) : "0");
    summary += "</p>";

    // 时间分析
    if (stats.totalHours > 0) {
        summary += QString("<p><b style='color: #4fc3f7;'>⏱️ 时间分析</b><br>");
        summary += QString("总计投入时间 <b style='color: #81c784;'>%1</b>，平均每个任务耗时 <b style='color: #81c784;'>%2</b>。")
                       .arg(formatDuration(stats.totalHours))
                       .arg(formatDuration(stats.averageTaskDuration));

        if (stats.continuousTasks > 0) {
            summary += QString("其中连续性任务 <b style='color: #81c784;'>%1</b> 个，占总任务的 <b style='color: #ffb74d;'>%2%</b>。")
                           .arg(stats.continuousTasks)
                           .arg(QString::number((double)stats.continuousTasks / stats.totalTasks * 100, 'f', 1));
        }
        summary += "</p>";
    }

    // 标签分析
    if (!stats.tagStats.isEmpty()) {
        summary += "<p><b style='color: #4fc3f7;'>🏷️ 分类分析</b><br>";
        auto it = stats.tagStats.constBegin();
        QStringList topTags;
        while (it != stats.tagStats.constEnd() && topTags.size() < 3) {
            topTags << QString("<span style='color: #ba68c8;'>%1</span>(<span style='color: #81c784;'>%2个</span>)").arg(it.key()).arg(it.value());
            ++it;
        }
        summary += QString("主要任务类型包括：%1。").arg(topTags.join("、"));
        summary += "</p>";
    }

    // 效率建议
    summary += "<p><b style='color: #4fc3f7;'>💡 效率建议</b><br>";
    if (stats.totalTasks == 0) {
        summary += "<span style='color: #ffb74d;'>暂无任务数据，建议开始规划和记录您的任务。</span>";
    } else {
        double completionRate = (double)stats.completedTasks / stats.totalTasks;
        if (completionRate >= 0.8) {
            summary += "<span style='color: #81c784;'>任务完成率很高，保持良好的执行力！</span>";
        } else if (completionRate >= 0.6) {
            summary += "<span style='color: #ffb74d;'>任务完成情况良好，可以适当提高执行效率。</span>";
        } else {
            summary += "<span style='color: #f06292;'>建议关注未完成任务，优化时间管理和任务规划。</span>";
        }
    }
    summary += "</p>";

    summary += "</div>";
    return summary;
}
