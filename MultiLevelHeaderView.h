#pragma once

#include <QHeaderView>
#include <QModelIndex>
#include <QMenu>
#include "data_model.h"

enum ItemDataRole
{
    COLUMN_SPAN_ROLE = Qt::UserRole + 1,
    ROW_SPAN_ROLE,
};

class MultiLevelHeaderView : public QHeaderView
{
    Q_OBJECT
public:
    MultiLevelHeaderView(Qt::Orientation orientation, int rows, int columns, QWidget* parent = 0);
    virtual ~MultiLevelHeaderView();

    void setRowHeight(int row, int rowHeight);
    void setColumnWidth(int col, int colWidth);
    void setCellData(int row, int column, int role, const QVariant& value);
    // the below methods is just shortcut for setCellData
    void setCellSpan(int row, int column, int rowSpanCount, int columnSpanCount);
    void setCellBackgroundColor(int row, int column, const QColor&);
    void setCellForegroundColor(int row, int column, const QColor&);
    void setCellText(int row, int column, const QString& text);
    void setCellIcon(int row, int column, const QIcon& icon);

    QModelIndex columnSpanIndex(const QModelIndex& currentIndex) const;
    QModelIndex rowSpanIndex(const QModelIndex& currentIndex) const;

protected:
    // override
    void mousePressEvent(QMouseEvent* event) override;
    QModelIndex indexAt(const QPoint &point) const override;
    void paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const override;
    QSize sectionSizeFromContents(int logicalIndex) const override;

    // inherent features
    int columnSpanSize(int row, int from, int spanCount) const;
    int rowSpanSize(int column, int from, int spanCount) const;
    bool getRootCell(int row, int column, int& rootCellRow, int& rootCellColumn) const;
    // (row, column) must be root cell of merged cells
    QRect getCellRect(int row, int column) const;
    int getSectionRange(QModelIndex& index, int* beginSection, int* endSection) const;
    void init_tool_menu();
    void init_param_menu();

protected slots:
    void onSectionResized(int logicalIdx, int oldSize, int newSize);
    void popup_tool_menu(const QPoint &pos);

    void popup_param_menu(const QPoint &pos);
    // add by hqh
    void on_section_clicked(int pos);

signals:
    void sectionPressed(int from, int to);
    // 表头添加点工具信号
    void header_add_tool(QString tool_name, int pos);

    // 表头添加点参数信号
    void header_add_param(int tool_id, QString param_key, QString value_list, int param_pos);
public:
    QList<ToolNode*> tool_list;
    void add_tool(ToolNode* tool_node);

private:
    QMenu _tool_menu;
    QMenu _param_menu;
};

