#ifndef EWSTABLEVIEW_H
#define EWSTABLEVIEW_H

#include "MultiLevelHeaderView.h"
#include "data_model.h"

#include <QStandardItemModel>
#include <QTableView>
#include <QMenu>

class EwsTableView : public QTableView
{
    Q_OBJECT
public:
    EwsTableView(QWidget *parent = nullptr);


    void init_table_header();

    void popup_tool_menu(const QPoint &pos);

public slots:

    void add_tool(QString tool_name, int pos);

    void add_param(int tool_col, QString param_key, QString value_list, int param_pos);
private:

    int header_row_num = 2;
    int header_col_num = 8;
    void init_horizontal_header();
    void init_vertical_header();

    QList<ToolNode*> tool_list;

    QMenu _tool_menu;

    QMenu _param_menu;

    MultiLevelHeaderView *pHeader = nullptr;
    QStandardItemModel* m_pDataModel;
};

#endif // EWSTABLEVIEW_H
